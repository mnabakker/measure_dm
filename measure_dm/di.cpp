#include "di.h"
#include "xi.h"


LONG LX = 0, LY = 0;
#ifdef DI_USE_BUFFERED_WITH_EVENT_NOTIFICATION
HANDLE hEvent = NULL;
#endif

unsigned int deviceCount = 0;
IDirectInputDevice8** devices = NULL;
IDirectInput8* dinput = NULL;

static BOOL CALLBACK DI_EnumDevicesCallback(LPCDIDEVICEINSTANCE instance, LPVOID userData)
{
    DI_mode_e DI_mode = (DI_mode_e)((uintptr_t)userData);
    //wprintf_s(L"%s - %s\n", instance->tszInstanceName, instance->tszProductName);

    // note: Filter x360 vs DS4 when enumerating devices, and use XInput instead of DInput for x360 tests
    if (XI_IsXInputDevice(&instance->guidProduct)) {
        xinputCount++;
        return DIENUM_CONTINUE;
    }

    HRESULT hr;
    IDirectInputDevice8* device;
    dinput->CreateDevice(instance->guidInstance, &device, NULL);
    device->SetCooperativeLevel(GetActiveWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    device->SetDataFormat(&c_dfDIJoystick);

    // note: do we use buffered mode?
    if (DI_mode == DI_BUFFERED || DI_mode == DI_MIXED)
    {
        // note: set to use buffered mode
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0; // device property 
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = DI_BUFFERED_SIZE;
        hr = device->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
        //if (SUCCEEDED(hr))
        //    wprintf_s(L"buffer size = %lu\n", dipdw.dwData);

#ifdef DI_USE_BUFFERED_WITH_EVENT_NOTIFICATION
        // todo: this could/should to be an array, for now its enough to support testing 1 device
        if (!hEvent) {
            hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
            hr = device->SetEventNotification(hEvent);
            // the vigem emulated DS4 does NOT need polling
            //if (hr == DI_POLLEDDEVICE)
            //    ;
        }
#endif
    }

    // note: convert to XInput numbers, so we can more easily compare logs
    //      (with exception for the y-axis that needs to be flipped on reads!, this API doesn't support reversed ranges)
    DIPROPRANGE propRange;
    propRange.lMin = -32767;
    propRange.lMax = 32767;
    propRange.diph.dwSize = sizeof(DIPROPRANGE);
    propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    propRange.diph.dwObj = DIJOFS_X;
    propRange.diph.dwHow = DIPH_BYOFFSET;
    hr = device->SetProperty(DIPROP_RANGE, &propRange.diph);
    //if (SUCCEEDED(hr))
    //    wprintf_s(L"x axis set to -32767 - 32767\n");
    propRange.lMin = -32767;
    propRange.lMax = 32767;
    propRange.diph.dwSize = sizeof(DIPROPRANGE);
    propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    propRange.diph.dwObj = DIJOFS_Y;
    propRange.diph.dwHow = DIPH_BYOFFSET;
    hr = device->SetProperty(DIPROP_RANGE, &propRange.diph);
    //if (SUCCEEDED(hr))
    //    wprintf_s(L"y axis set to -32767 - 32767\n");

    device->Acquire();

    deviceCount++;
    devices = (IDirectInputDevice8**)realloc(devices, deviceCount * sizeof(IDirectInputDevice8*));
    devices[deviceCount - 1] = device;
    return DIENUM_CONTINUE;
}

// note: as per Microsoft recommended for game developers
//      , we enumerate game-pads through DirectInput and filter x360 devices to be used with XInput
BOOL DI_Open(DI_mode_e DI_mode)
{
    DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, 0);
    dinput->EnumDevices(DI8DEVCLASS_GAMECTRL, DI_EnumDevicesCallback, (void*)DI_mode, DIEDFL_ALLDEVICES);

    if (xinputCount > 0) {
        DI_Close();
        wprintf_s(L"XInput (x360).\n");
        return XI_Open(xinputCount);
    }
    if (deviceCount == 0) {
        wprintf_s(L"Error: There are no Controllers connected.\n");
        return FALSE;
    }
    wprintf_s(L"DirectInput (DS4).\n");

    if (deviceCount != 1) {
        wprintf_s(L"Error: There is More than 1 DirectInput Controller connected.\n");
        return FALSE;
    }
    DIJOYSTATE state;
    if (devices[0]->GetDeviceState(sizeof(state), &state) != DI_OK) {
        wprintf_s(L"Error: Cannot determine the DirectInput Controller State.\n");
        return FALSE;
    }
    if (state.lX != LX || state.lY != LY) {
        wprintf_s(L"Error: The DirectInput Controller is NOT centered.\n");
        return FALSE;
    }
    return TRUE;
}

void DI_Close(void)
{
    if (devices) {
        while (deviceCount--) {
            devices[deviceCount]->Unacquire();
#ifdef DI_USE_BUFFERED_WITH_EVENT_NOTIFICATION
            devices[deviceCount]->SetEventNotification(NULL);
#endif
            devices[deviceCount]->Release();
        }
        free(devices);
    }
#ifdef DI_USE_BUFFERED_WITH_EVENT_NOTIFICATION
    if (hEvent)
        CloseHandle(hEvent);
    hEvent = NULL;
#endif
    devices = NULL;
    deviceCount = 0;
    if (dinput)
        dinput->Release();
    dinput = NULL;
    LX = 0, LY = 0;
}

void DI_flush(void)
{
    DWORD dwItems = INFINITE;
    HRESULT hres = devices[0]->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), NULL, &dwItems, 0);
#ifdef DI_USE_BUFFERED_WITH_EVENT_NOTIFICATION
    if (hEvent)
        ResetEvent(hEvent);
#endif
}

BOOL DI_wait_for_single_event(void)
{
    DIJOYSTATE state;
	while (devices[0]->GetDeviceState(sizeof(state), &state) == DI_OK)
	{
		if (state.lX != LX || state.lY != LY)
		{
			LX = state.lX;
			LY = state.lY;
			return TRUE;
		}
	}
	return FALSE;
}

int DI_wait_for_events(int numEvents)
{
#ifdef SAVE_THROUGHPUT_POSITIONS_TO_FILE
    FILE* fLog = NULL;
    fopen_s(&fLog, "di_log.txt", "wt");
#endif
    DIJOYSTATE state;
    int received_count = 0;
    while (received_count < numEvents)
    {
        if (devices[0]->GetDeviceState(sizeof(state), &state) != DI_OK)
            break;
        if (state.lX != LX || state.lY != LY)
        {
            LX = state.lX;
            LY = state.lY;
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
