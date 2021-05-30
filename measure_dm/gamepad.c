#include "gamepad.h"
#include "sendkeys.h"
#include "di.h"
#include "di_buffered.h"
#include "xi.h"

BOOL GamePad_Open(DI_mode_e DI_mode)
{
    return DI_Open(DI_mode);
}

void GamePad_Close(void)
{
    XI_Close();
    DI_Close();
}

//
// measures latency by Pressing and Releasing the key 'W', and waiting for a Controller change.
// 
BOOL MeasureLatency(DI_mode_e DI_mode, int numRounds)
{
    double pressed_total = 0., released_total = 0.;
    LARGE_INTEGER frequency, time_start, time_end;
    QueryPerformanceFrequency(&frequency);
    BOOL(*wait_for_single_event)(void) = NULL;

    // did we detect a XInput or DInput game-pad
    if (xinputCount > 0) {
        wait_for_single_event = XI_wait_for_single_event;
    }
    else {
        DI_flush();
        if (DI_mode == DI_BUFFERED)
            wait_for_single_event = DI_wait_for_single_event_buffered;
        else if (DI_mode == DI_IMMEDIATE)
            wait_for_single_event = DI_wait_for_single_event;
        else // DI_MIXED
            wait_for_single_event = DI_wait_for_single_event;
    }

    int cnt = 0;
    for (; cnt < numRounds; cnt++)
    {
        QueryPerformanceCounter(&time_start);
        SI_SendKeyDown('W');
        if (!wait_for_single_event())
            break;
        QueryPerformanceCounter(&time_end);
        pressed_total += (time_end.QuadPart - time_start.QuadPart) * 1000.0 / frequency.QuadPart;

        QueryPerformanceCounter(&time_start);
        SI_SendKeyUp('W');
        if (!wait_for_single_event())
            break;
        QueryPerformanceCounter(&time_end);
        released_total += (time_end.QuadPart - time_start.QuadPart) * 1000.0 / frequency.QuadPart;
    }
    if (cnt != numRounds)
    {
        wprintf_s(L"Error: The Controller got Disconnected after (%d out of %d rounds).\n", cnt, numRounds);
        return FALSE;
    }

    pressed_total /= numRounds;
    released_total /= numRounds;
    wprintf_s(L"Average Latency from Pressing  'W' to actual Controller Movement = %.6f ms.\n", pressed_total);
    wprintf_s(L"Average Latency from Releasing 'W' to actual Controller Movement = %.6f ms.\n", released_total);
    return TRUE;
}

LARGE_INTEGER thread_time_start, thread_time_end;
int thread_numEvents;

DWORD WINAPI thread_send_keys(LPVOID lpThreadParameter)
{
    QueryPerformanceCounter(&thread_time_start);
    SI_StartSending(thread_numEvents / 8);
    QueryPerformanceCounter(&thread_time_end);
    return 0;
}

//
// measures throughput by Pressing and Releasing the keys 'WASD', and counting the Controller changes..
// 
BOOL MeasureThroughput(DI_mode_e DI_mode, int numEvents)
{
    int received_count;
    LARGE_INTEGER frequency, time_start, time_end;
    QueryPerformanceFrequency(&frequency);
    int(*wait_for_events)(int) = NULL;
    int expected_controller_events;
    
    // did we detect a XInput or DInput game-pad
    if (xinputCount > 0) {
        expected_controller_events = numEvents + 2; // 2002 max
        wait_for_events = XI_wait_for_events;
    }
    else {
        expected_controller_events = numEvents * 2 + 2; // 4002 max
        DI_flush();
        if (DI_mode == DI_BUFFERED)
            wait_for_events = DI_wait_for_events_buffered;
        else if (DI_mode == DI_IMMEDIATE)
            wait_for_events = DI_wait_for_events;
        else // DI_MIXED
            wait_for_events = DI_wait_for_events_buffered;
    }

    // note: Spawn a thread to send keyboard-events
    thread_numEvents = numEvents;
    HANDLE hThread = CreateThread(NULL, 0, thread_send_keys, NULL, /*CREATE_SUSPENDED*/0, NULL);
    if (hThread == NULL)
    {
        wprintf_s(L"Error: CreateThread().\n");
        return FALSE;
    }
    // todo: should we force this- and the send-keys-thread on different CPU-cores
    //ResumeThread(hThread);

    QueryPerformanceCounter(&time_start);
    received_count = wait_for_events(expected_controller_events);
    QueryPerformanceCounter(&time_end);

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    // note: see: SI_StartSending(), it sends 2 more keys-events than its given argument * 8
    numEvents += 2;

    const double keys_sent_time = (thread_time_end.QuadPart - thread_time_start.QuadPart) * 1000.0 / frequency.QuadPart;
    wprintf_s(L"Sent %lu key-events in %.6f ms.(%.2f hz)\n", numEvents, keys_sent_time, (1000. / (keys_sent_time / (double)numEvents)));
    if (received_count != expected_controller_events)
    {
        wprintf_s(L"Error: The Controller got Disconnected after (%d out of %d movements received).\n", received_count, expected_controller_events);
        return FALSE;
    }
    const double keys_recv_time = (time_end.QuadPart - time_start.QuadPart) * 1000.0 / frequency.QuadPart;
    // note: hz are displayed relative to the key-events sent!
    wprintf_s(L"Received %d controller-movements in %.6f ms.(%.2f hz)\n", expected_controller_events, keys_recv_time, (1000. / (keys_recv_time / (double)numEvents)));

    return TRUE;
}
