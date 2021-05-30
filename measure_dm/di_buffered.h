#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    #include "main.h"

    BOOL DI_wait_for_single_event_buffered(void);
    int DI_wait_for_events_buffered(int numEvents);

#ifdef __cplusplus
}
#endif
