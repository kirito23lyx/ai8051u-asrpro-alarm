#ifndef A12_EEPROM_H
#define A12_EEPROM_H

#include "a01_config.h"

u8 Eeprom_LoadAlarm(u8 *hour, u8 *minute, u8 *second, u8 *enabled);
u8 Eeprom_SaveAlarm(u8 hour, u8 minute, u8 second, u8 enabled);

#endif
