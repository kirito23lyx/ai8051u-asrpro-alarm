#include "a09_alarm_app.h"
#include "a04_key.h"
#include "a05_beep.h"
#include "a06_oled.h"
#include "a08_clock.h"
#include "a12_eeprom.h"

#define VIEW_TIME       0
#define VIEW_PARAM      1
#define VIEW_SET        2
#define VIEW_ALARM      3

#define FIELD_HOUR      0
#define FIELD_MINUTE    1
#define FIELD_SECOND    2

#define RING_NOTE_OFF   0xFF

static u8 view;
static u8 view_before_alarm;
static bit render_needed;

static u8 alarm_hour;
static u8 alarm_minute;
static u8 alarm_second;
static bit alarm_enabled;
static bit alarm_match_latched;
static u8 edit_field;

static bit ringing;
static u16 ring_ms;
static u8 ring_note;
static u8 eeprom_status;

static void App_RequestRender(void)
{
    render_needed = 1;
}

static void App_PutTimeString(char *buf, u8 hour, u8 minute, u8 second)
{
    buf[0] = (char)(hour / 10 + '0');
    buf[1] = (char)(hour % 10 + '0');
    buf[2] = ':';
    buf[3] = (char)(minute / 10 + '0');
    buf[4] = (char)(minute % 10 + '0');
    buf[5] = ':';
    buf[6] = (char)(second / 10 + '0');
    buf[7] = (char)(second % 10 + '0');
    buf[8] = '\0';
}

static void App_ShowCurrentTitle(void)
{
    OLED_ShowHzXY(32, 0, HZ_DANG);
    OLED_ShowHzXY(48, 0, HZ_QIAN);
    OLED_ShowHzXY(64, 0, HZ_SHI);
    OLED_ShowHzXY(80, 0, HZ_JIAN);
}

static void App_ShowParamTitle(void)
{
    OLED_ShowHzXY(0, 0, HZ_CAN);
    OLED_ShowHzXY(16, 0, HZ_SHU);
    OLED_ShowHzXY(32, 0, HZ_SHE);
    OLED_ShowHzXY(48, 0, HZ_ZHI);
}

static void App_ShowSetTitle(void)
{
    OLED_ShowHzXY(32, 0, HZ_SHI);
    OLED_ShowHzXY(48, 0, HZ_JIAN);
    OLED_ShowHzXY(64, 0, HZ_SHE);
    OLED_ShowHzXY(80, 0, HZ_ZHI);
}

static void App_ShowTimeAt(u8 page, u8 hour, u8 minute, u8 second)
{
    char buf[9];

    App_PutTimeString(buf, hour, minute, second);
    OLED_ShowStringXY(32, page, buf);
}

static void App_ShowEditTime(void)
{
    char buf[9];

    App_PutTimeString(buf, alarm_hour, alarm_minute, alarm_second);

    if (!gClockBlink)
    {
        if (edit_field == FIELD_HOUR)
        {
            buf[0] = ' ';
            buf[1] = ' ';
        }
        else if (edit_field == FIELD_MINUTE)
        {
            buf[3] = ' ';
            buf[4] = ' ';
        }
        else
        {
            buf[6] = ' ';
            buf[7] = ' ';
        }
    }

    OLED_ShowStringXY(32, 2, buf);
}

static void App_ShowSetHint(void)
{
    OLED_ShowStringXY(0, 4, "s6");
    OLED_ShowHzXY(16, 4, HZ_JIA);
    OLED_ShowStringXY(32, 4, "s7");
    OLED_ShowHzXY(48, 4, HZ_JIAN2);
    OLED_ShowStringXY(64, 4, "s2");
    OLED_ShowHzXY(80, 4, HZ_QIE);
    OLED_ShowStringXY(96, 4, "s5");
    OLED_ShowHzXY(112, 4, HZ_TUI);
}

static void App_AdjustEdit(s16 step)
{
    s16 value;

    if (edit_field == FIELD_HOUR)
    {
        value = (s16)alarm_hour + step;
        while (value < 0) value += 24;
        while (value >= 24) value -= 24;
        alarm_hour = (u8)value;
    }
    else if (edit_field == FIELD_MINUTE)
    {
        value = (s16)alarm_minute + step;
        while (value < 0) value += 60;
        while (value >= 60) value -= 60;
        alarm_minute = (u8)value;
    }
    else
    {
        value = (s16)alarm_second + step;
        while (value < 0) value += 60;
        while (value >= 60) value -= 60;
        alarm_second = (u8)value;
    }

    gClockBlink = 1;
    App_RequestRender();
}

static void App_AdjustVoiceEdit(s16 step)
{
    if ((edit_field == FIELD_HOUR) || (edit_field == FIELD_MINUTE))
    {
        App_AdjustEdit((step > 0) ? 1 : -1);
    }
    else
    {
        App_AdjustEdit(step);
    }
}

static void App_SelectNextEditField(void)
{
    if (++edit_field > FIELD_SECOND)
    {
        edit_field = FIELD_HOUR;
    }
    gClockBlink = 1;
    App_RequestRender();
}

static void App_EnterSetView(void)
{
    if (ringing)
    {
        return;
    }

    view = VIEW_SET;
    edit_field = FIELD_SECOND;
    gClockBlink = 1;
    App_RequestRender();
}

static void App_SaveAlarmToView(u8 next_view)
{
    u8 saved_hour;
    u8 saved_minute;
    u8 saved_second;
    u8 saved_enabled;

    alarm_enabled = 1;
    alarm_match_latched = 0;
    eeprom_status = Eeprom_SaveAlarm(alarm_hour, alarm_minute, alarm_second, 1);
    if (eeprom_status == 0)
    {
        if (Eeprom_LoadAlarm(&saved_hour, &saved_minute, &saved_second, &saved_enabled))
        {
            if ((saved_hour != alarm_hour) ||
                (saved_minute != alarm_minute) ||
                (saved_second != alarm_second) ||
                (saved_enabled == 0))
            {
                eeprom_status = 3;
            }
        }
        else
        {
            eeprom_status = 4;
        }
    }
    view = next_view;
    App_RequestRender();
}

static void App_StartRinging(void)
{
    if (ringing)
    {
        return;
    }

    ringing = 1;
    view_before_alarm = view;
    view = VIEW_ALARM;
    ring_ms = 0;
    ring_note = RING_NOTE_OFF;
    App_RequestRender();
}

static void App_StopRinging(void)
{
    ringing = 0;
    Beep_Off();
    ring_note = RING_NOTE_OFF;
    view = view_before_alarm;
    App_RequestRender();
}

static void App_CheckAlarmMatch(void)
{
    u8 matched;

    matched = 0;
    if (alarm_enabled && !ringing && (view != VIEW_SET))
    {
        if ((gClockHour == alarm_hour) &&
            (gClockMinute == alarm_minute) &&
            (gClockSecond == alarm_second))
        {
            matched = 1;
        }
    }

    if (matched)
    {
        if (!alarm_match_latched)
        {
            alarm_match_latched = 1;
            App_StartRinging();
        }
    }
    else
    {
        alarm_match_latched = 0;
    }
}

static void App_UpdateRingSound(void)
{
    u8 next_note;

    if (!ringing)
    {
        return;
    }

    if (ring_ms < 200)
    {
        next_note = BEEP_NOTE_DO;
    }
    else if (ring_ms < 400)
    {
        next_note = BEEP_NOTE_RE;
    }
    else if (ring_ms < 600)
    {
        next_note = BEEP_NOTE_MI;
    }
    else
    {
        next_note = RING_NOTE_OFF;
    }

    if (next_note != ring_note)
    {
        ring_note = next_note;
        if (ring_note == RING_NOTE_OFF)
        {
            Beep_Off();
        }
        else
        {
            Beep_On(ring_note);
        }
    }

    if (++ring_ms >= 1600)
    {
        ring_ms = 0;
    }
}

void App_Init(void)
{
    u8 saved_enabled;

    view = VIEW_TIME;
    view_before_alarm = VIEW_TIME;
    render_needed = 1;

    alarm_hour = 10;
    alarm_minute = 0;
    alarm_second = 0;
    alarm_enabled = 0;
    alarm_match_latched = 0;
    edit_field = FIELD_SECOND;
    eeprom_status = 0xFF;

    if (Eeprom_LoadAlarm(&alarm_hour, &alarm_minute, &alarm_second, &saved_enabled))
    {
        alarm_enabled = (saved_enabled != 0);
    }

    ringing = 0;
    ring_ms = 0;
    ring_note = RING_NOTE_OFF;
}

void App_Tick1ms(void)
{
    App_UpdateRingSound();
}

void App_Tick500ms(void)
{
    u8 second_changed;

    second_changed = Clock_Tick500ms();

    if (view == VIEW_SET)
    {
        App_RequestRender();
    }

    if (second_changed)
    {
        App_CheckAlarmMatch();
        if (view == VIEW_TIME)
        {
            App_RequestRender();
        }
    }
}

void App_HandleKeyEvent(u8 evt)
{
    u8 type;
    u8 key;

    if (evt == KEY_EVT_NONE)
    {
        return;
    }

    type = KEY_EVENT_TYPE(evt);
    key = KEY_EVENT_KEY(evt);

    if (ringing)
    {
        return;
    }

    if ((type == KEY_EVT_SHORT) && (key == KEY_S4))
    {
        if (view == VIEW_TIME)
        {
            view = VIEW_PARAM;
            App_RequestRender();
        }
        else if (view == VIEW_PARAM)
        {
            view = VIEW_TIME;
            App_RequestRender();
        }
        return;
    }

    if ((type == KEY_EVT_LONG) && (key == KEY_S5))
    {
        if (view == VIEW_PARAM)
        {
            App_EnterSetView();
        }
        else if (view == VIEW_SET)
        {
            App_SaveAlarmToView(VIEW_PARAM);
        }
        return;
    }

    if (view != VIEW_SET)
    {
        return;
    }

    if ((type == KEY_EVT_SHORT) && (key == KEY_S2))
    {
        App_SelectNextEditField();
    }
    else if (((type == KEY_EVT_SHORT) || (type == KEY_EVT_REPEAT)) && (key == KEY_S6))
    {
        App_AdjustEdit(1);
    }
    else if (((type == KEY_EVT_SHORT) || (type == KEY_EVT_REPEAT)) && (key == KEY_S7))
    {
        App_AdjustEdit(-1);
    }
}

void App_HandleVoiceCommand(u8 cmd)
{
    switch (cmd)
    {
        case VOICE_CMD_SET_ALARM:
            App_EnterSetView();
            break;

        case VOICE_CMD_ADD:
            if (view == VIEW_SET)
            {
                App_AdjustVoiceEdit(10);
            }
            break;

        case VOICE_CMD_SUB:
            if (view == VIEW_SET)
            {
                App_AdjustVoiceEdit(-10);
            }
            break;

        case VOICE_CMD_DONE:
            if (view == VIEW_SET)
            {
                App_SaveAlarmToView(VIEW_TIME);
            }
            break;

        case VOICE_CMD_WAKE_DONE:
            if (ringing)
            {
                App_StopRinging();
            }
            break;

        case VOICE_CMD_QUERY_TIME:
            if (!ringing)
            {
                view = VIEW_TIME;
                App_RequestRender();
            }
            break;

        case VOICE_CMD_LIGHT_ALARM:
            App_StartRinging();
            break;

        case VOICE_CMD_NEXT_FIELD:
            if (view == VIEW_SET)
            {
                App_SelectNextEditField();
            }
            break;

        default:
            break;
    }
}

void App_RenderIfNeeded(void)
{
    if (!render_needed)
    {
        return;
    }
    render_needed = 0;

    if (view == VIEW_ALARM)
    {
        OLED_ShowWakeupBitmap();
        return;
    }

    OLED_Clear();

    if (view == VIEW_TIME)
    {
        App_ShowCurrentTitle();
        App_ShowTimeAt(2, gClockHour, gClockMinute, gClockSecond);
    }
    else if (view == VIEW_PARAM)
    {
        char eeprom_buf[3];

        App_ShowParamTitle();
        OLED_ShowHzXY(0, 2, HZ_CHANG);
        OLED_ShowStringXY(16, 2, "s5");
        OLED_ShowHzXY(32, 2, HZ_JIN);
        OLED_ShowHzXY(48, 2, HZ_NAO);
        OLED_ShowHzXY(64, 2, HZ_ZHONG);

        if (eeprom_status != 0xFF)
        {
            eeprom_buf[0] = 'E';
            eeprom_buf[1] = (char)((eeprom_status % 10) + '0');
            eeprom_buf[2] = '\0';
            OLED_ShowStringXY(104, 2, eeprom_buf);
        }
    }
    else if (view == VIEW_SET)
    {
        App_ShowSetTitle();
        App_ShowEditTime();
        App_ShowSetHint();
    }
}
