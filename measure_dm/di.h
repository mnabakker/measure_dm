#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    #include "main.h"

    #define DI_BUFFERED_SIZE    24

    extern LONG LX, LY;
#ifdef DI_USE_BUFFERED_WITH_EVENT_NOTIFICATION
    extern HANDLE hEvent;
#endif
    extern IDirectInputDevice8** devices;

    BOOL DI_Open(DI_mode_e DI_mode);
    void DI_Close(void);
    void DI_flush(void);
    BOOL DI_wait_for_single_event(void);
    int DI_wait_for_events(int numEvents);

#ifdef __cplusplus
}
#endif
