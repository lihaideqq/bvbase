/*
 * Copyright (c) 2000-2003 Fabrice Bellard
 *
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVBase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "config.h"

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#if BV_HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif
#if BV_HAVE_UNISTD_H
#include <unistd.h>
#endif
#if BV_HAVE_WINDOWS_H
#include <windows.h>
#endif

#include "time.h"
#include "error.h"

int64_t bv_gettime(void)
{
#if BV_HAVE_GETTIMEOFDAY
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
#elif BV_HAVE_GETSYSTEMTIMEASFILETIME
    FILETIME ft;
    int64_t t;
    GetSystemTimeAsFileTime(&ft);
    t = (int64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime;
    return t / 10 - 11644473600000000; /* Jan 1, 1601 */
#else
    return -1;
#endif
}

int64_t bv_gettime_relative(void)
{
#if BV_HAVE_CLOCK_GETTIME && defined(CLOCK_MONOTONIC)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
#else
    return bv_gettime() + 42 * 60 * 60 * INT64_C(1000000);
#endif
}

int bv_gettime_relative_is_monotonic(void)
{
#if BV_HAVE_CLOCK_GETTIME && defined(CLOCK_MONOTONIC)
    return 1;
#else
    return 0;
#endif
}

int bv_usleep(unsigned usec)
{
#if BV_HAVE_NANOSLEEP
    struct timespec ts = { usec / 1000000, usec % 1000000 * 1000 };
    while (nanosleep(&ts, &ts) < 0 && errno == EINTR);
    return 0;
#elif BV_HAVE_USLEEP
    return usleep(usec);
#elif BV_HAVE_SLEEP
    Sleep(usec / 1000);
    return 0;
#else
    return BVERROR(ENOSYS);
#endif
}
