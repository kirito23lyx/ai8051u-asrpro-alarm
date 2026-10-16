#ifndef A01_CONFIG_H
#define A01_CONFIG_H

#define HIRC        40000000UL
#define FOSC        40000000UL
#define SYSCLK      FOSC
#define MAIN_Fosc   FOSC

#include <ai8051u.h>
#include <intrins.h>

#define u8   unsigned char
#define u16  unsigned int
#define u32  unsigned long
#define s8   signed char
#define s16  signed int
#define s32  signed long

#define KEY_NONE    0xFF

void SYS_Init(void);

#endif
