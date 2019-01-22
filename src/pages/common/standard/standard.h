#ifndef _COMMON_STANDARD_H_
#define _COMMON_STANDARD_H_
#define VIEW_ID 0

#include "mixer_standard.h"

typedef enum {
    PITTHROMODE_NORMAL = 0,
    PITTHROMODE_IDLE1,
    PITTHROMODE_IDLE2,
    PITTHROMODE_HOLD
} PitThroMode;

typedef enum {
    CURVESMODE_PITCH = 0,
    CURVESMODE_THROTTLE,
} CurvesMode;

void STANDARD_Init(int pageid,
    const char *(*value)(guiObject_t *obj, int dir, void *data),
    void (*tgl)(guiObject_t *obj, void *data));

void STANDARD_DrawCurvePoints(guiLabel_t vallbl[], guiTextSelect_t val[],
        u8 selectable_bitmap,
        void (*press_cb)(guiObject_t *obj, void *data),
        const char *(*set_pointval_cb)(guiObject_t *obj, int value, void *data));

const char *STDMIX_channelname_cb(guiObject_t *obj, const void *data);
int STDMIX_GetMixers(struct Mixer **mixers, u8 dest_channel, int count);
const char *STDMIX_ModeName(PitThroMode pit_mode);
s32 STDMIX_EvalMixerCb(s32 xval, struct Mixer *mix, s32 max_value, s32 min_value);
const char *STDMIX_TitleString(guiObject_t *obj, const void *data);
#define SET_TITLE_DATA(x, y) (void *)((unsigned long)(x) | ((unsigned long)(y) << 16))
#endif
