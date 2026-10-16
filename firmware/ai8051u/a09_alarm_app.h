#ifndef A09_ALARM_APP_H
#define A09_ALARM_APP_H

#include "a01_config.h"

#define VOICE_CMD_SET_ALARM     0x10
#define VOICE_CMD_ADD           0x11
#define VOICE_CMD_SUB           0x12
#define VOICE_CMD_DONE          0x13
#define VOICE_CMD_WAKE_DONE     0x14
#define VOICE_CMD_QUERY_TIME    0x15
#define VOICE_CMD_LIGHT_ALARM   0x16
#define VOICE_CMD_NEXT_FIELD    0x17

void App_Init(void);
void App_Tick1ms(void);
void App_Tick500ms(void);
void App_HandleKeyEvent(u8 evt);
void App_HandleVoiceCommand(u8 cmd);
void App_RenderIfNeeded(void);

#endif
