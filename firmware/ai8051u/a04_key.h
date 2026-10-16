#ifndef A04_KEY_H
#define A04_KEY_H

#include "a01_config.h"

#define KEY_S0      0
#define KEY_S1      1
#define KEY_S2      2
#define KEY_S3      3
#define KEY_S4      4
#define KEY_S5      5
#define KEY_S6      6
#define KEY_S7      7

#define KEY_EVT_NONE    0
#define KEY_EVT_SHORT   1
#define KEY_EVT_LONG    2
#define KEY_EVT_REPEAT  3

#define KEY_MAKE_EVENT(type, key)   (u8)(((type) << 4) | ((key) & 0x0F))
#define KEY_EVENT_TYPE(evt)         (u8)((evt) >> 4)
#define KEY_EVENT_KEY(evt)          (u8)((evt) & 0x0F)

void Key_Init(void);
void Key_Scan1ms(void);
u8 Key_GetEvent(void);

#endif
