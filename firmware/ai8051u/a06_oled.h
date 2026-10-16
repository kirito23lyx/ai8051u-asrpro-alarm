#ifndef A06_OLED_H
#define A06_OLED_H

#include "a01_config.h"
#include "a07_oled_font.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ClearLine(u8 line);
void OLED_ShowCharXY(u8 x, u8 page, char ch);
void OLED_ShowStringXY(u8 x, u8 page, char *s);
void OLED_ShowHzXY(u8 x, u8 page, u8 hz_index);
void OLED_ShowWakeupBitmap(void);

#endif
