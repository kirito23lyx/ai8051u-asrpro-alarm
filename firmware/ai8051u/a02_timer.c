#include "a02_timer.h"

#define TIMER0_RELOAD   (65536UL - (FOSC / 1000UL))

bit f1ms;
bit f500ms;
bit fBeepEn;

static u16 cnt500;
static u16 timer1_reload;

void Timer0_Init(void)
{
    AUXR |= 0x80;
    TMOD &= 0xF0;
    TL0 = (u8)TIMER0_RELOAD;
    TH0 = (u8)(TIMER0_RELOAD >> 8);
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;

    cnt500 = 0;
    f1ms = 0;
    f500ms = 0;
}

void Timer1_Init(void)
{
    AUXR &= ~0x40;
    TMOD &= 0x0F;
    TL1 = 0x00;
    TH1 = 0x00;
    TF1 = 0;
    TR1 = 1;
    ET1 = 1;

    timer1_reload = 0;
    fBeepEn = 0;
    P50 = 0;
}

void Timer1_SetReload(u16 reload)
{
    timer1_reload = reload;
    TL1 = (u8)reload;
    TH1 = (u8)(reload >> 8);
}

void Timer0_Isr(void) interrupt 1
{
    TL0 = (u8)TIMER0_RELOAD;
    TH0 = (u8)(TIMER0_RELOAD >> 8);

    f1ms = 1;
    if (++cnt500 >= 500)
    {
        cnt500 = 0;
        f500ms = 1;
    }
}

void Timer1_Isr(void) interrupt 3
{
    if (fBeepEn)
    {
        TL1 = (u8)timer1_reload;
        TH1 = (u8)(timer1_reload >> 8);
        P50 = !P50;
    }
    else
    {
        P50 = 0;
    }
}
