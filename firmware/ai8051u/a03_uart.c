#include "a03_uart.h"

void Uart2_Init(void)
{
    S2CON = 0x50;
    AUXR |= 0x04;
    T2L = 0xA9;
    T2H = 0xFF;
    AUXR |= 0x10;
}

void Uart2_Send(u8 dat)
{
    S2BUF = dat;
    while (!S2TI);
    S2TI = 0;
}
