#include "a01_config.h"
#include "a02_timer.h"
#include "a03_uart.h"
#include "a04_key.h"
#include "a06_oled.h"
#include "a08_clock.h"
#include "a09_alarm_app.h"

void SYS_Init(void)
{
    EAXFR = 1;
    WTST = 0x00;
    CKCON = 0x00;

    P0M0 = 0x00; P0M1 = 0x00;
    P1M0 = 0x00; P1M1 = 0x00;
    P2M0 = 0x00; P2M1 = 0x00;
    P3M0 = 0x00; P3M1 = 0x00;
    P4M0 = 0x00; P4M1 = 0x00;
    P5M0 = 0x01; P5M1 = 0x00;
    P6M0 = 0x00; P6M1 = 0x00;
    P7M0 = 0x00; P7M1 = 0x00;

    P0 = 0xFF;
    P50 = 0;

    S1_S1 = 0;
    S1_S0 = 1;

    Timer0_Init();
    Timer1_Init();

    S2_S = 1;
    Uart2_Init();
}

void main(void)
{
    u8 dat;
    u8 key_evt;

    SYS_Init();
    OLED_Init();
    Key_Init();
    Clock_Init();
    App_Init();
    App_RenderIfNeeded();

    EA = 1;

    while (1)
    {
        if (f1ms)
        {
            f1ms = 0;
            Key_Scan1ms();
            key_evt = Key_GetEvent();
            App_HandleKeyEvent(key_evt);
            App_Tick1ms();
        }

        if (f500ms)
        {
            f500ms = 0;
            App_Tick500ms();
        }

        if (S2RI)
        {
            dat = S2BUF;
            S2RI = 0;
            App_HandleVoiceCommand(dat);
        }

        App_RenderIfNeeded();
    }
}
