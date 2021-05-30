#include "di_buffered.h"
#include "di.h"

#ifdef DI_USE_BUFFERED_WITH_EVENT_NOTIFICATION

BOOL DI_wait_for_single_event_buffered(void)
{
    DIDEVICEOBJECTDATA rgdod[DI_BUFFERED_SIZE];
    const DWORD dwResult = WaitForSingleObject(hEvent, 1000);
    if (dwResult == WAIT_OBJECT_0)
    {
        DWORD dwItems = DI_BUFFERED_SIZE;
        const HRESULT hres = devices[0]->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
        if (!SUCCEEDED(hres))
            return FALSE;
        if (dwItems == 0)
            return FALSE;
        if (rgdod[0].dwOfs == DIJOFS_Y)
            LY = (LONG)rgdod[0].dwData;
        else if (rgdod[0].dwOfs == DIJOFS_X)
            LX = (LONG)rgdod[0].dwData;
        else
            return FALSE;
        return (dwItems == 1);
    }
    return FALSE;
}

int DI_wait_for_events_buffered(int numEvents)
{
#ifdef SAVE_THROUGHPUT_POSITIONS_TO_FILE
    FILE* fLog = NULL;
    fopen_s(&fLog, "di_log.txt", "wt");
#endif
    DIDEVICEOBJECTDATA rgdod[DI_BUFFERED_SIZE];
    int received_count = 0;

    while (received_count < numEvents)
    {
        const DWORD dwResult = WaitForSingleObject(hEvent, 2000);
        if (dwResult != WAIT_OBJECT_0)
            break;
        DWORD dwItems = DI_BUFFERED_SIZE;
        const HRESULT hres = devices[0]->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
        if (!SUCCEEDED(hres))
            break;
        // dwItems = Number of elements read (could be zero).
        if (hres == DI_BUFFEROVERFLOW) {
            wprintf_s(L"DirectInput queue had overflowed.\n");
        }
        if (dwItems == 0)
            continue;
        for (DWORD i = 0; i < dwItems; i++)
        {
            // note: do not need to check if values change, they will always change
            if (rgdod[i].dwOfs == DIJOFS_Y)
                LY = (LONG)rgdod[i].dwData;
            else if (rgdod[i].dwOfs == DIJOFS_X)
                LX = (LONG)rgdod[i].dwData;
            else
                break;
#ifdef SAVE_THROUGHPUT_POSITIONS_TO_FILE
            fprintf(fLog, "[% 4d] x=% 5d, y= % 5d\n", received_count, (int)LX, (int)LY * -1);
#endif
            received_count++;
        }
    }

#ifdef SAVE_THROUGHPUT_POSITIONS_TO_FILE
    fclose(fLog);
#endif
    return received_count;
}

#else

BOOL DI_wait_for_single_event_buffered(void)
{
    DIDEVICEOBJECTDATA rgdod[DI_BUFFERED_SIZE];
    while (1)
    {
        DWORD dwItems = DI_BUFFERED_SIZE;
        const HRESULT hres = devices[0]->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
        if (!SUCCEEDED(hres))
            break;
        if (dwItems == 0)
            continue;
        if (rgdod[0].dwOfs == DIJOFS_Y)
            LY = (LONG)rgdod[0].dwData;
        else if (rgdod[0].dwOfs == DIJOFS_X)
            LX = (LONG)rgdod[0].dwData;
        else
            break;
        return (dwItems == 1);
    }
    return FALSE;
}

int DI_wait_for_events_buffered(int numEvents)
{
#ifdef SAVE_THROUGHPUT_POSITIONS_TO_FILE
    FILE* fLog = NULL;
    fopen_s(&fLog, "di_log.txt", "wt");
#endif
    DIDEVICEOBJECTDATA rgdod[DI_BUFFERED_SIZE];
    int received_count = 0;
    
    while (received_count < numEvents)
    {
        DWORD dwItems = DI_BUFFERED_SIZE;
        const HRESULT hres = devices[0]->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), rgdod, &dwItems, 0);
        if (!SUCCEEDED(hres))
            break;
        // dwItems = Number of elements read (could be zero).
        if (hres == DI_BUFFEROVERFLOW) {
            // Buffer had overflowed.
            wprintf_s(L"DirectInput queue had overflowed.\n");
        }
        if (dwItems == 0)
            continue;
        for (DWORD i = 0; i < dwItems; i++)
        {
            // note: do not need to check if values change, they will always change?
            if (rgdod[i].dwOfs == DIJOFS_Y)
                LY = (LONG)rgdod[i].dwData;
            else if (rgdod[i].dwOfs == DIJOFS_X)
                LX = (LONG)rgdod[i].dwData;
            else
                break;
#ifdef SAVE_THROUGHPUT_POSITIONS_TO_FILE
            fprintf(fLog, "[% 4d] x=% 5d, y= % 5d\n", received_count, (int)LX, (int)LY * -1);
#endif
            received_count++;
        }
    }

#ifdef SAVE_THROUGHPUT_POSITIONS_TO_FILE
    fclose(fLog);
#endif
    return received_count;
}

#endif
