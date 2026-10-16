#include "a12_eeprom.h"

#define EEPROM_ALARM_ADDR       0x000040UL
#define EEPROM_ALARM_LEN        7
#define EEPROM_MAGIC0           0xA5
#define EEPROM_MAGIC1           0x5A

#define IAP_CMD_READ            1
#define IAP_CMD_WRITE           2
#define IAP_CMD_ERASE           3
#define IAP_ENABLE_BIT          (1 << 7)

static void Eeprom_Disable(void)
{
    IAP_CONTR = 0;
    IAP_CMD = 0;
    IAP_TRIG = 0;
    IAP_ADDRE = 0xFF;
    IAP_ADDRH = 0xFF;
    IAP_ADDRL = 0xFF;
}

static void Eeprom_Enable(u8 cmd)
{
    IAP_CONTR = IAP_ENABLE_BIT;
    IAP_TPS = (u8)(FOSC / 1000000UL);
    IAP_CMD = cmd;
}

static void Eeprom_Trigger(void)
{
    F0 = EA;
    EA = 0;
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    EA = F0;
}

static void Eeprom_SetAddress(u32 address)
{
    IAP_ADDRE = (u8)(address >> 16);
    IAP_ADDRH = (u8)(address >> 8);
    IAP_ADDRL = (u8)address;
}

static void Eeprom_SectorErase(u32 address)
{
    Eeprom_Enable(IAP_CMD_ERASE);
    Eeprom_SetAddress(address);
    Eeprom_Trigger();
    Eeprom_Disable();
}

static void Eeprom_ReadN(u32 address, u8 *buf, u8 len)
{
    if (len == 0)
    {
        return;
    }

    Eeprom_Enable(IAP_CMD_READ);
    do
    {
        Eeprom_SetAddress(address);
        Eeprom_Trigger();
        *buf = IAP_DATA;
        address++;
        buf++;
    } while (--len);
    Eeprom_Disable();
}

static u8 Eeprom_WriteN(u32 address, u8 *buf, u8 len)
{
    u32 start_address;
    u8 *start_buf;
    u8 start_len;

    if (len == 0)
    {
        return 1;
    }

    start_address = address;
    start_buf = buf;
    start_len = len;

    Eeprom_Enable(IAP_CMD_WRITE);
    do
    {
        Eeprom_SetAddress(address);
        IAP_DATA = *buf;
        Eeprom_Trigger();
        address++;
        buf++;
    } while (--len);
    Eeprom_Disable();

    address = start_address;
    buf = start_buf;
    len = start_len;

    Eeprom_Enable(IAP_CMD_READ);
    do
    {
        Eeprom_SetAddress(address);
        Eeprom_Trigger();
        if (*buf != IAP_DATA)
        {
            Eeprom_Disable();
            return 2;
        }
        address++;
        buf++;
    } while (--len);
    Eeprom_Disable();

    return 0;
}

static u8 Eeprom_Checksum(u8 *buf)
{
    return (u8)(buf[0] ^ buf[1] ^ buf[2] ^ buf[3] ^ buf[4] ^ buf[5] ^ 0x3C);
}

u8 Eeprom_LoadAlarm(u8 *hour, u8 *minute, u8 *second, u8 *enabled)
{
    u8 buf[EEPROM_ALARM_LEN];

    Eeprom_ReadN(EEPROM_ALARM_ADDR, buf, EEPROM_ALARM_LEN);

    if ((buf[0] != EEPROM_MAGIC0) || (buf[1] != EEPROM_MAGIC1))
    {
        return 0;
    }
    if (buf[6] != Eeprom_Checksum(buf))
    {
        return 0;
    }
    if ((buf[2] >= 24) || (buf[3] >= 60) || (buf[4] >= 60) || (buf[5] > 1))
    {
        return 0;
    }

    *hour = buf[2];
    *minute = buf[3];
    *second = buf[4];
    *enabled = buf[5];
    return 1;
}

u8 Eeprom_SaveAlarm(u8 hour, u8 minute, u8 second, u8 enabled)
{
    u8 buf[EEPROM_ALARM_LEN];

    if ((hour >= 24) || (minute >= 60) || (second >= 60))
    {
        return 1;
    }

    buf[0] = EEPROM_MAGIC0;
    buf[1] = EEPROM_MAGIC1;
    buf[2] = hour;
    buf[3] = minute;
    buf[4] = second;
    buf[5] = enabled ? 1 : 0;
    buf[6] = Eeprom_Checksum(buf);

    Eeprom_SectorErase(EEPROM_ALARM_ADDR);
    return Eeprom_WriteN(EEPROM_ALARM_ADDR, buf, EEPROM_ALARM_LEN);
}
