#include "a04_key.h"

#define KEY_DEBOUNCE_MS     20
#define KEY_S5_LONG_MS      800
#define KEY_FAST_START_MS   2000
#define KEY_FAST_PERIOD_MS  200

static u8 raw_last;
static u8 stable_key;
static u8 debounce;
static u8 event_code;
static u16 hold_ms;
static u16 repeat_ms;
static bit long_sent;

static u8 Key_ReadRaw(void)
{
    u8 key;

    key = KEY_NONE;
    P0 = 0xFF;

    P06 = 0;
    P07 = 1;
    _nop_();
    _nop_();
    if (!P00) key = KEY_S0;
    else if (!P01) key = KEY_S1;
    else if (!P02) key = KEY_S2;
    else if (!P03) key = KEY_S3;
    else
    {
        P07 = 0;
        P06 = 1;
        _nop_();
        _nop_();
        if (!P00) key = KEY_S4;
        else if (!P01) key = KEY_S5;
        else if (!P02) key = KEY_S6;
        else if (!P03) key = KEY_S7;
    }

    P0 = 0xFF;
    return key;
}

void Key_Init(void)
{
    raw_last = KEY_NONE;
    stable_key = KEY_NONE;
    debounce = 0;
    event_code = KEY_EVT_NONE;
    hold_ms = 0;
    repeat_ms = 0;
    long_sent = 0;
}

void Key_Scan1ms(void)
{
    u8 raw;
    u8 old_key;

    raw = Key_ReadRaw();

    if (raw != raw_last)
    {
        raw_last = raw;
        debounce = KEY_DEBOUNCE_MS;
        return;
    }

    if (debounce != 0)
    {
        if (--debounce == 0)
        {
            if (stable_key != raw)
            {
                old_key = stable_key;
                stable_key = raw;

                if (stable_key != KEY_NONE)
                {
                    hold_ms = 0;
                    repeat_ms = 0;
                    long_sent = 0;
                }
                else
                {
                    if ((old_key != KEY_NONE) && (!long_sent) && (event_code == KEY_EVT_NONE))
                    {
                        event_code = KEY_MAKE_EVENT(KEY_EVT_SHORT, old_key);
                    }
                }
            }
        }
        return;
    }

    if (stable_key == KEY_NONE)
    {
        return;
    }

    if (hold_ms < 60000)
    {
        hold_ms++;
    }

    if ((stable_key == KEY_S5) && (!long_sent) && (hold_ms >= KEY_S5_LONG_MS))
    {
        long_sent = 1;
        if (event_code == KEY_EVT_NONE)
        {
            event_code = KEY_MAKE_EVENT(KEY_EVT_LONG, stable_key);
        }
    }

    if ((stable_key == KEY_S6) || (stable_key == KEY_S7))
    {
        if (hold_ms >= KEY_FAST_START_MS)
        {
            long_sent = 1;
            if (repeat_ms == 0)
            {
                repeat_ms = KEY_FAST_PERIOD_MS;
                if (event_code == KEY_EVT_NONE)
                {
                    event_code = KEY_MAKE_EVENT(KEY_EVT_REPEAT, stable_key);
                }
            }
            else if (--repeat_ms == 0)
            {
                repeat_ms = KEY_FAST_PERIOD_MS;
                if (event_code == KEY_EVT_NONE)
                {
                    event_code = KEY_MAKE_EVENT(KEY_EVT_REPEAT, stable_key);
                }
            }
        }
    }
}

u8 Key_GetEvent(void)
{
    u8 evt;

    evt = event_code;
    event_code = KEY_EVT_NONE;
    return evt;
}
