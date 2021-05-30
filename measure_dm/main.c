#include "main.h"
#include "gamepad.h"

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
#pragma comment(lib, "xinput.lib")
#else
#pragma comment(lib, "xinput9_1_0.lib")
#endif
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define NUM_SINGLE_ROUNDS       (100)
#define NUM_DIRECTION_CHANGES   (250 * 8) // note: must be a multiple of 8!


int run_test(DI_mode_e DI_mode)
{
    if (!GamePad_Open(DI_mode))
        return EXIT_FAILURE;

    wprintf_s(L"\nMeasuring latency for %d Keyboard events.\n", NUM_SINGLE_ROUNDS * 2);
    Sleep(2000);
    if (!MeasureLatency(DI_mode, NUM_SINGLE_ROUNDS))
        return EXIT_FAILURE;

    wprintf_s(L"\n\nMeasuring throughput for %d Keyboard events.\n", NUM_DIRECTION_CHANGES + 2);
    wprintf_s(L"Note: If results are not shown after 5 seconds, Close your Double-Movement solution, to show some results.\n");
    Sleep(2000);
    if (!MeasureThroughput(DI_mode, NUM_DIRECTION_CHANGES))
        return EXIT_FAILURE;
    
    GamePad_Close();
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    // optional cmd-line option for Direct Input could be: 0=buffered, 1=immediate, 2=mixed
    DI_mode_e DI_mode = (argc <= 1 ? DI_MIXED : (DI_mode_e)(strtoul(argv[1], NULL, 0) & 0x3));
    
    const int ret = run_test(DI_mode);
    
    wprintf_s(L"\nPress Enter to Exit.\n");
    (void)getchar();
    return ret;
}
