#ifndef A05_BEEP_H
#define A05_BEEP_H

#include "a01_config.h"

#define BEEP_NOTE_DO    0
#define BEEP_NOTE_RE    1
#define BEEP_NOTE_MI    2

void Beep_On(u8 note);
void Beep_Off(void);

#endif
