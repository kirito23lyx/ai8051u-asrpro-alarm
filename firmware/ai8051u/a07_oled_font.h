#ifndef A07_OLED_FONT_H
#define A07_OLED_FONT_H

#include "a01_config.h"

#define HZ_DANG      0
#define HZ_QIAN      1
#define HZ_SHI       2
#define HZ_JIAN      3
#define HZ_CAN       4
#define HZ_SHU       5
#define HZ_SHE       6
#define HZ_ZHI       7
#define HZ_CHANG     8
#define HZ_JIN       9
#define HZ_NAO       10
#define HZ_ZHONG     11
#define HZ_JIA       12
#define HZ_JIAN2     13
#define HZ_QIE       14
#define HZ_TUI       15
#define HZ_QI        16
#define HZ_CHUANG    17
#define HZ_COUNT    18

extern unsigned char code OLED_ASCII_8x16[95][16];
extern unsigned char code OLED_HZ_16x16[HZ_COUNT][32];
extern unsigned char code OLED_WAKEUP_128x64[1024];

#endif
