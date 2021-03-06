/*************************************************************************
    > File Name: onvif_ptz.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月14日 星期日 17时17分33秒
 ************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should hbve received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#include "bvdevice.h"
#include <soapH.h>
#include <wsseapi.h>

#include <libbvutil/bvstring.h>

typedef struct OnvifPTZContext {
    const BVClass *bv_class;
    int timeout;
    char *user;
    char *passwd;
    char svrurl[1024];
    char *token;
    char *ptz_url;
    char profile_name[32];
    char profile_token[32];
    struct soap *soap;
} OnvifPTZContext;

//Onvif default timeout    0.5s
#define ONVIF_TMO    (-500000)
#define MEMSET_STRUCT(X)    memset(&(X), 0, sizeof((X)));

static struct soap *bv_soap_new(OnvifPTZContext *onvif_ptz)
{
    int timeout = onvif_ptz->timeout;
    struct soap *soap = NULL;
    soap = soap_new();
    if (soap == NULL) {
        return NULL;
    }
    soap_set_namespaces(soap, namespaces);
    if (!timeout) {
        timeout = ONVIF_TMO;
    }

    soap->recv_timeout = timeout;
    soap->send_timeout = timeout;
    soap->connect_timeout = timeout;
    return soap;
}

static void bv_soap_free(struct soap *soap)
{
    if (soap == NULL) {
        return;
    }
    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);
}

static int bv_onvif_service_uri(OnvifPTZContext *onvif_ptz)
{
    int retval = SOAP_OK;
    struct soap *soap = onvif_ptz->soap;
    struct _tds__GetCapabilities request;
    struct _tds__GetCapabilitiesResponse response;
    enum tt__CapabilityCategory Category = tt__CapabilityCategory__PTZ;

    MEMSET_STRUCT(request);
    MEMSET_STRUCT(response);

    if (onvif_ptz->user && onvif_ptz->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvif_ptz->user, onvif_ptz->passwd);
    }

    request.Category = &Category;
    request.__sizeCategory = 1;
    retval = soap_call___tds__GetCapabilities(soap, onvif_ptz->svrurl, NULL, &request, &response);
    if(retval != SOAP_OK) {
        bv_log(onvif_ptz, BV_LOG_ERROR, "get PTZService URI error");
        bv_log(onvif_ptz, BV_LOG_ERROR, "[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        return retval;
    } else {
        onvif_ptz->ptz_url = bv_strdup(response.Capabilities->PTZ->XAddr);
    }
    bv_log(onvif_ptz, BV_LOG_INFO, "Get PTZService URI %s\n", onvif_ptz->ptz_url);
    return retval;
}


//svrurl likes onvif_ptz://admin:123456@192.168.6.149:8899/onvif/device_services/?token=0000?timeout=5000
static int onvif_ptz_open(BVDeviceContext *h)
{
    OnvifPTZContext *onvif_ptz = h->priv_data;
    char *p, *q;
    int size;
    if (h->url[0] == '\0') {
        bv_log(onvif_ptz, BV_LOG_ERROR, "url is NULL\n");
        return BVERROR(EINVAL);
    }
    if (!onvif_ptz->token) {
        bv_log(onvif_ptz, BV_LOG_ERROR, "onvif ptz token is NULL\n");
        return BVERROR(EINVAL);
    }
    p = bv_strsub(onvif_ptz->token, "/", 1);
    q = bv_strsub(onvif_ptz->token, "/", 2);
    if (!p) {
        bv_log(h, BV_LOG_ERROR, "onvif_ptz token error\n");
        return BVERROR(EINVAL);
    }
    size = sizeof(onvif_ptz->profile_name);
    size = size > p - onvif_ptz->token ? p - onvif_ptz->token : size;
    bv_strlcpy(onvif_ptz->profile_name, onvif_ptz->token, p - onvif_ptz->token);
    size = sizeof(onvif_ptz->profile_token);
    size = size > q - p ? q - p : size;
    bv_strlcpy(onvif_ptz->profile_token, p, size);

    p = bv_sreplace(h->url, "onvif_ptz", "http");
    if (!p) {
        return BVERROR(ENOMEM);
    }
    bv_strlcpy(onvif_ptz->svrurl, p, sizeof(onvif_ptz->svrurl));
    bv_free(p);

    onvif_ptz->soap = bv_soap_new(onvif_ptz);
    if (!onvif_ptz->soap)
        return BVERROR(ENOMEM);
    if (bv_onvif_service_uri(onvif_ptz)) {
        bv_soap_free(onvif_ptz->soap);
        return BVERROR(EINVAL);
    }
    bv_log(h, BV_LOG_DEBUG, "ptz svr %s\n", onvif_ptz->ptz_url);

    return 0;
}

static int onvif_ptz_probe(BVDeviceContext *h, const char *args)
{
    if (strcmp("onvif_ptz", args) == 0) {
        return 100;
    }
    return 0;
}

static int onvif_ptz_continuous_move(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int ret = 0;
    int retval = SOAP_OK;
    struct _tptz__ContinuousMove tptz__ContinuousMove; 
    struct tt__PTZSpeed Velocity;
    struct tt__Vector2D PanTilt;
    struct tt__Vector1D Zoom;
    struct _tptz__ContinuousMoveResponse tptz__ContinuousMoveResponse;
    struct SOAP_ENV__Header header;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZContinuousMove *continuous_move = (BVPTZContinuousMove *) pkt_in->data;
    struct soap *soap = onvif_ptz->soap;

    soap_default_SOAP_ENV__Header(soap, &header);
    MEMSET_STRUCT(tptz__ContinuousMove);
    MEMSET_STRUCT(tptz__ContinuousMoveResponse);
    MEMSET_STRUCT(Velocity);
    MEMSET_STRUCT(PanTilt);
    MEMSET_STRUCT(Zoom);
    tptz__ContinuousMove.ProfileToken = onvif_ptz->profile_token;
    tptz__ContinuousMove.Velocity = &Velocity;
    tptz__ContinuousMove.Timeout = &continuous_move->duration;
    Velocity.PanTilt = &PanTilt;
    Velocity.Zoom = & Zoom;

    if (!onvif_ptz->ptz_url) {
        return BVERROR(ENOSYS);
    }
    if (onvif_ptz->user && onvif_ptz->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvif_ptz->user, onvif_ptz->passwd);
    }

    PanTilt.x = continuous_move->velocity.pan_tilt.x;
    PanTilt.y = continuous_move->velocity.pan_tilt.y;
    Zoom.x = continuous_move->velocity.zoom.x;
    
    retval = soap_call___tptz__ContinuousMove(soap, onvif_ptz->ptz_url, NULL, &tptz__ContinuousMove, &tptz__ContinuousMoveResponse);

    if (retval != SOAP_OK) {
        printf("PTZ ContinuousMove error");
        printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    return ret;
}

static int onvif_ptz_stop(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int ret = 0;
    int retval = SOAP_OK;
    struct SOAP_ENV__Header header;
    struct _tptz__Stop tptz__Stop;
    struct _tptz__StopResponse tptz__StopResponse;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZStop *stop = (BVPTZStop *)pkt_in->data;
    struct soap *soap = onvif_ptz->soap;

    MEMSET_STRUCT(header);
    MEMSET_STRUCT(tptz__Stop);
    MEMSET_STRUCT(tptz__StopResponse);
    soap_default_SOAP_ENV__Header(soap, &header);

    if (!onvif_ptz->ptz_url) {
        return BVERROR(ENOSYS);
    }
    if (onvif_ptz->user && onvif_ptz->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvif_ptz->user, onvif_ptz->passwd);
    }

    tptz__Stop.ProfileToken = onvif_ptz->profile_token;
    tptz__Stop.PanTilt = (enum xsd__boolean *)&stop->pan_tilt;
    tptz__Stop.Zoom = (enum xsd__boolean *)&stop->zoom;

    retval = soap_call___tptz__Stop(soap, onvif_ptz->ptz_url, NULL, &tptz__Stop, &tptz__StopResponse);
    if (retval != SOAP_OK) {
        printf("PTZ ContinuousMove error");
        printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    return ret;
}

static int onvif_ptz_set_preset(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int ret = 0;
    int retval = SOAP_OK;
    struct SOAP_ENV__Header header;
    struct _tptz__SetPreset tptz__SetPreset;
    struct _tptz__SetPresetResponse tptz__SetPresetResponse;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZPreset *preset = (BVPTZPreset *)pkt_in->data;
    struct soap *soap = onvif_ptz->soap;

    MEMSET_STRUCT(header);
    MEMSET_STRUCT(tptz__SetPreset);
    MEMSET_STRUCT(tptz__SetPresetResponse);

    if (!onvif_ptz->ptz_url) {
        return BVERROR(ENOSYS);
    }
    soap_default_SOAP_ENV__Header(soap, &header);
    tptz__SetPreset.ProfileToken = onvif_ptz->profile_token;
    tptz__SetPreset.PresetName = preset->name;
    tptz__SetPreset.PresetToken = preset->token;
    if (onvif_ptz->user && onvif_ptz->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvif_ptz->user, onvif_ptz->passwd);
    }

    retval = soap_call___tptz__SetPreset(soap, onvif_ptz->ptz_url, NULL, &tptz__SetPreset, &tptz__SetPresetResponse);
    if (retval != SOAP_OK) {
        printf("PTZ SetPreset error");
        printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    return ret;
}

static int onvif_ptz_goto_preset(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int ret = 0;
    int retval = SOAP_OK;
    struct SOAP_ENV__Header header;
    struct _tptz__GotoPreset tptz__GotoPreset;
    struct _tptz__GotoPresetResponse tptz__GotoPresetResponse;
    struct tt__PTZSpeed Speed;
    struct tt__Vector2D PanTilt;
    struct tt__Vector1D Zoom;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    BVPTZGotoPreset *goto_preset = (BVPTZGotoPreset*)pkt_in->data;
    struct soap *soap = onvif_ptz->soap;

    MEMSET_STRUCT(header);
    MEMSET_STRUCT(tptz__GotoPreset);
    MEMSET_STRUCT(tptz__GotoPresetResponse);

    MEMSET_STRUCT(Speed);
    MEMSET_STRUCT(PanTilt);
    MEMSET_STRUCT(Zoom);

    if (!onvif_ptz->ptz_url) {
        return BVERROR(ENOSYS);
    }
    soap_default_SOAP_ENV__Header(soap, &header);
    tptz__GotoPreset.ProfileToken = onvif_ptz->profile_token;
    tptz__GotoPreset.PresetToken = goto_preset->token;
    tptz__GotoPreset.Speed = &Speed;
    if (onvif_ptz->user && onvif_ptz->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvif_ptz->user, onvif_ptz->passwd);
    }

    Speed.PanTilt = &PanTilt;
    Speed.Zoom = & Zoom;

    PanTilt.x = goto_preset->speed.pan_tilt.x;
    PanTilt.y = goto_preset->speed.pan_tilt.y;
    Zoom.x = goto_preset->speed.zoom.x;

    retval = soap_call___tptz__GotoPreset(soap, onvif_ptz->ptz_url, NULL, &tptz__GotoPreset, &tptz__GotoPresetResponse);
    if (retval != SOAP_OK) {
        printf("PTZ SetPreset error");
        printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    return ret;
}

static int onvif_ptz_remove_preset(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int ret = 0;
    int retval = SOAP_OK;
    struct SOAP_ENV__Header header;
    struct _tptz__RemovePreset tptz__RemovePreset;
    struct _tptz__RemovePresetResponse tptz__RemovePresetResponse;
    OnvifPTZContext *onvif_ptz = h->priv_data;
    struct soap *soap = onvif_ptz->soap;
    BVPTZPreset *preset = (BVPTZPreset *)pkt_in->data;

    MEMSET_STRUCT(header);
    MEMSET_STRUCT(tptz__RemovePreset);
    MEMSET_STRUCT(tptz__RemovePresetResponse);

    if (!onvif_ptz->ptz_url) {
        return BVERROR(ENOSYS);
    }

    soap_default_SOAP_ENV__Header(soap, &header);
    tptz__RemovePreset.ProfileToken = onvif_ptz->profile_token;
    tptz__RemovePreset.PresetToken = preset->token;
    if (onvif_ptz->user && onvif_ptz->passwd) {
        soap_wsse_add_UsernameTokenDigest(soap, "user", onvif_ptz->user, onvif_ptz->passwd);
    }

    retval = soap_call___tptz__RemovePreset(soap, onvif_ptz->ptz_url, NULL, &tptz__RemovePreset, &tptz__RemovePresetResponse);
    if (retval != SOAP_OK) {
        printf("PTZ RemovePreset error");
        printf("[%d]: recv soap error :%d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
        ret = -1;
    }

    return ret;
}

static int onvif_ptz_control(BVDeviceContext *h, enum BVDeviceMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVDeviceMessageType type;
        int (*control)(BVDeviceContext *h, const BVControlPacket *, BVControlPacket *);
    } ptz_control[] = {
        {BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, onvif_ptz_continuous_move},
        {BV_DEV_MESSAGE_TYPE_PTZ_STOP, onvif_ptz_stop},
        {BV_DEV_MESSAGE_TYPE_PTZ_SET_PRESET, onvif_ptz_set_preset},
        {BV_DEV_MESSAGE_TYPE_PTZ_GOTO_PRESET, onvif_ptz_goto_preset},
        {BV_DEV_MESSAGE_TYPE_PTZ_REMOVE_PRESET, onvif_ptz_remove_preset},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(ptz_control); i++) {
       if (ptz_control[i].type == type)
          return ptz_control[i].control(h, pkt_in, pkt_out); 
    }
    bv_log(h, BV_LOG_ERROR, "Not Support This command \n");
    return BVERROR(ENOSYS);
}

static int onvif_ptz_close(BVDeviceContext*h)
{
    OnvifPTZContext *onvif_ptz = h->priv_data;
    if (!onvif_ptz)
        return -1;
    bv_soap_free(onvif_ptz->soap);
    return 0;
}

#define OFFSET(x) offsetof(OnvifPTZContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    {"timeout", "read write time out", OFFSET(timeout), BV_OPT_TYPE_INT, {.i64 =  -500000}, INT_MIN, INT_MAX, DEC},
    {"user", "user name", OFFSET(user), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"passwd", "user password", OFFSET(passwd), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"token", "token name", OFFSET(token), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"ptz_url", "ptz service utl", OFFSET(ptz_url), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {NULL}
};

static const BVClass onvif_class = {
    .class_name         = "onvif ptz device",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEVICE,
};

BVDevice bv_onvif_ptz_device = {
    .name               = "onvif_ptz",
    .type               = BV_DEVICE_TYPE_ONVIF_PTZ,
    .priv_data_size     = sizeof(OnvifPTZContext),
    .dev_open           = onvif_ptz_open,
    .dev_probe          = onvif_ptz_probe,
    .dev_control        = onvif_ptz_control,
    .dev_close          = onvif_ptz_close,
    .priv_class         = &onvif_class,
};
