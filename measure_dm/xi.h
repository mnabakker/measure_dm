#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    #include "main.h"

    extern unsigned int xinputCount;

    BOOL XI_IsXInputDevice(const GUID* pGuidProductFromDirectInput);
    BOOL XI_Open(unsigned int xinputCount);
    void XI_Close(void);
    BOOL XI_wait_for_single_event(void);
    int XI_wait_for_events(int numEvents);

#ifdef __cplusplus
}
#endif
