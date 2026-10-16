#include "a06_oled.h"

sbit OLED_SCK = P3^2;
sbit OLED_SDA = P3^3;
sbit OLED_RES = P4^7;
sbit OLED_DC  = P1^1;
sbit OLED_CS  = P3^5;

static void OLED_DelayMs(u16 ms)
{
    u16 i;

    while (ms--)
    {
        i = MAIN_Fosc / 6000;
        while (--i);
    }
}

static void OLED_SPI_Init(void)
{
    P_SW1 = (P_SW1 & ~(3 << 2)) | (3 << 2);
    HSSPI_CFG2 |= 0x40;

    P3M0 |= 0x2C;
    P3M1 &= ~0x2C;
    P1M0 |= 0x02;
    P1M1 &= ~0x02;
    P4M0 |= 0x80;
    P4M1 &= ~0x80;
    P3SR &= ~0x2C;
    P3DR |= 0x2C;

    SSIG = 1;
    SPEN = 1;
    DORD = 0;
    MSTR = 1;
    CPOL = 0;
    CPHA = 0;
    SPCTL = (SPCTL & ~3) | 3;

    OLED_SCK = 0;
    OLED_SDA = 1;
    OLED_DC = 1;
    OLED_CS = 1;
    OLED_RES = 1;
    SPIF = 1;
    WCOL = 1;
}

static void OLED_SendByte(u8 dat)
{
    SPDAT = dat;
    while (SPIF == 0);
    SPIF = 1;
    WCOL = 1;
}

static void OLED_WriteCommand(u8 cmd)
{
    OLED_SCK = 0;
    OLED_DC = 0;
    OLED_CS = 0;
    OLED_SendByte(cmd);
    OLED_CS = 1;
}

static void OLED_WriteData(u8 dat)
{
    OLED_SCK = 0;
    OLED_DC = 1;
    OLED_CS = 0;
    OLED_SendByte(dat);
    OLED_CS = 1;
}

static void OLED_SetCursor(u8 page, u8 x)
{
    OLED_WriteCommand((u8)(0xB0 | page));
    OLED_WriteCommand((u8)(0x10 | ((x & 0xF0) >> 4)));
    OLED_WriteCommand((u8)(0x00 | (x & 0x0F)));
}

void OLED_Clear(void)
{
    u8 page;
    u8 x;

    for (page = 0; page < 8; page++)
    {
        OLED_SetCursor(page, 0);
        for (x = 0; x < 128; x++)
        {
            OLED_WriteData(0x00);
        }
    }
}

void OLED_ClearLine(u8 line)
{
    u8 page;
    u8 x;

    if (line >= 4)
    {
        return;
    }

    for (page = (u8)(line * 2); page < (u8)(line * 2 + 2); page++)
    {
        OLED_SetCursor(page, 0);
        for (x = 0; x < 128; x++)
        {
            OLED_WriteData(0x00);
        }
    }
}

void OLED_ShowCharXY(u8 x, u8 page, char ch)
{
    u8 i;
    u8 idx;

    if ((ch < ' ') || (ch > '~'))
    {
        ch = ' ';
    }
    idx = (u8)(ch - ' ');

    OLED_SetCursor(page, x);
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_ASCII_8x16[idx][i]);
    }

    OLED_SetCursor((u8)(page + 1), x);
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_ASCII_8x16[idx][i + 8]);
    }
}

void OLED_ShowStringXY(u8 x, u8 page, char *s)
{
    while (*s != '\0')
    {
        if (x > 120)
        {
            break;
        }
        OLED_ShowCharXY(x, page, *s);
        x = (u8)(x + 8);
        s++;
    }
}

void OLED_ShowHzXY(u8 x, u8 page, u8 hz_index)
{
    u8 i;

    if (hz_index >= HZ_COUNT)
    {
        return;
    }

    OLED_SetCursor(page, x);
    for (i = 0; i < 16; i++)
    {
        OLED_WriteData(OLED_HZ_16x16[hz_index][i]);
    }

    OLED_SetCursor((u8)(page + 1), x);
    for (i = 0; i < 16; i++)
    {
        OLED_WriteData(OLED_HZ_16x16[hz_index][i + 16]);
    }
}

void OLED_ShowWakeupBitmap(void)
{
    u8 page;
    u8 x;
    u16 index;

    index = 0;
    for (page = 0; page < 8; page++)
    {
        OLED_SetCursor(page, 0);
        for (x = 0; x < 128; x++)
        {
            OLED_WriteData(OLED_WAKEUP_128x64[index++]);
        }
    }
}

void OLED_Init(void)
{
    OLED_SPI_Init();

    OLED_RES = 1;
    OLED_DelayMs(100);
    OLED_RES = 0;
    OLED_DelayMs(200);
    OLED_RES = 1;
    OLED_DelayMs(200);

    OLED_WriteCommand(0xAE);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x10);
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(0x81);
    OLED_WriteCommand(0xCF);
    OLED_WriteCommand(0xA1);
    OLED_WriteCommand(0xC8);
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0xA8);
    OLED_WriteCommand(0x3F);
    OLED_WriteCommand(0xD3);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0xD5);
    OLED_WriteCommand(0x80);
    OLED_WriteCommand(0xD9);
    OLED_WriteCommand(0xF1);
    OLED_WriteCommand(0xDA);
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0xDB);
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(0x20);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x8D);
    OLED_WriteCommand(0x14);
    OLED_WriteCommand(0xA4);
    OLED_WriteCommand(0xA6);
    OLED_WriteCommand(0xAF);

    OLED_Clear();
}
