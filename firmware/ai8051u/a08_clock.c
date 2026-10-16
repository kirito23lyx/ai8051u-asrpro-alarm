#include "a08_clock.h"

u8 gClockHour;
u8 gClockMinute;
u8 gClockSecond;
bit gClockBlink;

static bit half_second;

void Clock_Init(void)
{
    gClockHour = 12;
    gClockMinute = 0;
    gClockSecond = 0;
    gClockBlink = 1;
    half_second = 0;
}

u8 Clock_Tick500ms(void)
{
    half_second = !half_second;
    gClockBlink = !gClockBlink;

    if (half_second)
    {
        return 0;
    }

    if (++gClockSecond >= 60)
    {
        gClockSecond = 0;
        if (++gClockMinute >= 60)
        {
            gClockMinute = 0;
            if (++gClockHour >= 24)
            {
                gClockHour = 0;
            }
        }
    }

    return 1;
}
