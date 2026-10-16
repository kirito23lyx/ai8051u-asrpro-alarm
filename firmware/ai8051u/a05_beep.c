#include "a05_beep.h"
#include "a02_timer.h"

#define BEEP_RELOAD_DO      0xE71C
#define BEEP_RELOAD_RE      0xE9D3
#define BEEP_RELOAD_MI      0xEC3F

void Beep_On(u8 note)
{
    switch (note)
    {
        case BEEP_NOTE_DO:
            Timer1_SetReload(BEEP_RELOAD_DO);
            break;
        case BEEP_NOTE_RE:
            Timer1_SetReload(BEEP_RELOAD_RE);
            break;
        case BEEP_NOTE_MI:
            Timer1_SetReload(BEEP_RELOAD_MI);
            break;
        default:
            Beep_Off();
            return;
    }

    fBeepEn = 1;
}

void Beep_Off(void)
{
    fBeepEn = 0;
    P50 = 0;
}
