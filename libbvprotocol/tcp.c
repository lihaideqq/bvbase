/*
 * TCP protocol
 * Copyright (c) 2002 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "bvurl.h"
#include "libbvutil/parseutils.h"
#include "libbvutil/opt.h"
#include "libbvutil/time.h"
#include "libbvutil/bvstring.h"

//#include "internal.h"
#include "libbvutil/network.h"
#include "libbvutil/os_support.h"
#if BV_HAVE_POLL_H
#include <poll.h>
#endif

typedef struct TCPContext {
    const BVClass *class;
    int fd;
    int listen;
    int open_timeout;
    int rw_timeout;
    int listen_timeout;
} TCPContext;

#define OFFSET(x) offsetof(TCPContext, x)
#define D BV_OPT_FLAG_DECODING_PARAM
#define E BV_OPT_FLAG_ENCODING_PARAM
static const BVOption options[] = {
    { "listen", "Listen for incoming connections",  OFFSET(listen), BV_OPT_TYPE_INT, { .i64 = 0 }, 0, 1, .flags = D|E },
    { "timeout", "set timeout (in microseconds) of socket I/O operations", OFFSET(rw_timeout), BV_OPT_TYPE_INT, { .i64 = -1 }, -1, INT_MAX, .flags = D|E },
    { "listen_timeout", "Connection awaiting timeout", OFFSET(listen_timeout), BV_OPT_TYPE_INT, { .i64 = -1 }, -1, INT_MAX, .flags = D|E },
    { NULL }
};

static const BVClass tcp_class = {
    .class_name = "tcp",
    .item_name  = bv_default_item_name,
    .option     = options,
    .version    = LIBBVUTIL_VERSION_INT,
};

/* return non zero if error */
static int tcp_open(BVURLContext *h, const char *uri, int flags, BVDictionary **options)
{
    struct addrinfo hints = { 0 }, *ai, *cur_ai;
    int port, fd = -1;
    TCPContext *s = h->priv_data;
    const char *p;
    char buf[256];
    int ret;
    char hostname[1024],proto[1024],path[1024];
    char portstr[10];
    s->open_timeout = 5000000;

    bv_url_split(proto, sizeof(proto), NULL, 0, hostname, sizeof(hostname),
        &port, path, sizeof(path), uri);
    if (strcmp(proto, "tcp"))
        return BVERROR(EINVAL);
    if (port <= 0 || port >= 65536) {
        bv_log(h, BV_LOG_ERROR, "Port missing in uri\n");
        return BVERROR(EINVAL);
    }
    p = strchr(uri, '?');
    if (p) {
        if (bv_find_info_tag(buf, sizeof(buf), "listen", p)) {
            char *endptr = NULL;
            s->listen = strtol(buf, &endptr, 10);
            /* assume if no digits were found it is a request to enable it */
            if (buf == endptr)
                s->listen = 1;
        }
        if (bv_find_info_tag(buf, sizeof(buf), "timeout", p)) {
            s->rw_timeout = strtol(buf, NULL, 10);
        }
        if (bv_find_info_tag(buf, sizeof(buf), "listen_timeout", p)) {
            s->listen_timeout = strtol(buf, NULL, 10);
        }
    }
    if (s->rw_timeout >= 0) {
        s->open_timeout =
        h->rw_timeout   = s->rw_timeout;
    }
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    snprintf(portstr, sizeof(portstr), "%d", port);
    if (s->listen)
        hints.ai_flags |= AI_PASSIVE;
    if (!hostname[0])
        ret = getaddrinfo(NULL, portstr, &hints, &ai);
    else
        ret = getaddrinfo(hostname, portstr, &hints, &ai);
    if (ret) {
        bv_log(h, BV_LOG_ERROR,
               "Failed to resolve hostname %s: %s\n",
               hostname, gai_strerror(ret));
        return BVERROR(EIO);
    }

    cur_ai = ai;

 restart:
    fd = bv_socket(cur_ai->ai_family,
                   cur_ai->ai_socktype,
                   cur_ai->ai_protocol);
    if (fd < 0) {
        ret = bv_neterrno();
        goto fail;
    }

    if (s->listen) {
        if ((fd = bv_listen_bind(fd, cur_ai->ai_addr, cur_ai->ai_addrlen,
                                 s->listen_timeout, &h->interrupt_callback)) < 0) {
            ret = fd;
            goto fail1;
        }
    } else {
        if ((ret = bv_listen_connect(fd, cur_ai->ai_addr, cur_ai->ai_addrlen,
                                     s->open_timeout / 1000, &h->interrupt_callback, !!cur_ai->ai_next)) < 0) {

            if (ret == BVERROR_EXIT)
                goto fail1;
            else
                goto fail;
        }
    }

    h->is_streamed = 1;
    s->fd = fd;
    freeaddrinfo(ai);
    return 0;

 fail:
    if (cur_ai->ai_next) {
        /* Retry with the next sockaddr */
        cur_ai = cur_ai->ai_next;
        if (fd >= 0)
            closesocket(fd);
        ret = 0;
        goto restart;
    }
 fail1:
    if (fd >= 0)
        closesocket(fd);
    freeaddrinfo(ai);
    return ret;
}

static int tcp_read(BVURLContext *h, uint8_t *buf, size_t size)
{
    TCPContext *s = h->priv_data;
    int ret;

    if (!(h->flags & BV_IO_FLAG_NONBLOCK)) {
        ret = bv_network_wait_fd_timeout(s->fd, 0, h->rw_timeout, &h->interrupt_callback);
        if (ret)
            return ret;
    }
    ret = recv(s->fd, buf, size, 0);
    return ret < 0 ? bv_neterrno() : ret;
}

static int tcp_write(BVURLContext *h, const uint8_t *buf, size_t size)
{
    TCPContext *s = h->priv_data;
    int ret;

    if (!(h->flags & BV_IO_FLAG_NONBLOCK)) {
        ret = bv_network_wait_fd_timeout(s->fd, 1, h->rw_timeout, &h->interrupt_callback);
        if (ret)
            return ret;
    }
    ret = send(s->fd, buf, size, MSG_NOSIGNAL);
    return ret < 0 ? bv_neterrno() : ret;
}

static int tcp_shutdown(BVURLContext *h, int flags)
{
    TCPContext *s = h->priv_data;
    int how;

    if (flags & BV_IO_FLAG_WRITE && flags & BV_IO_FLAG_READ) {
        how = SHUT_RDWR;
    } else if (flags & BV_IO_FLAG_WRITE) {
        how = SHUT_WR;
    } else {
        how = SHUT_RD;
    }

    return shutdown(s->fd, how);
}

static int tcp_close(BVURLContext *h)
{
    TCPContext *s = h->priv_data;
    closesocket(s->fd);
    return 0;
}

static int tcp_get_file_handle(BVURLContext *h)
{
    TCPContext *s = h->priv_data;
    return s->fd;
}

BVURLProtocol bv_tcp_protocol = {
    .name                = "tcp",
    .url_open            = tcp_open,
    .url_read            = tcp_read,
    .url_write           = tcp_write,
    .url_close           = tcp_close,
    .url_get_file_handle = tcp_get_file_handle,
    .url_shutdown        = tcp_shutdown,
    .priv_data_size      = sizeof(TCPContext),
    .flags               = BV_URL_PROTOCOL_FLAG_NETWORK,
    .priv_class          = &tcp_class,
};
