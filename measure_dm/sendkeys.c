#include "sendkeys.h"

void SI_SendKeyStrokes(int count);

void SI_SendKeyDown(UINT uCode)
{
    INPUT inputs[1];
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.dwFlags = KEYEVENTF_SCANCODE;
    inputs[0].ki.wScan = MapVirtualKey(uCode, MAPVK_VK_TO_VSC_EX);
    
    const UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
        wprintf_s(L"SI_SendKeyDown failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
}

void SI_SendKeyUp(UINT uCode)
{
    INPUT inputs[1];
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    inputs[0].ki.wScan = MapVirtualKey(uCode, MAPVK_VK_TO_VSC_EX);

    const UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
        wprintf_s(L"SI_SendKeyUp failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
}


// note: this will send (count * 8) + 2 key-events
// note: this will send events like we're rolling a DPAD through all positions
void SI_StartSending(int count)
{
    INPUT inputs[1];
    ZeroMemory(inputs, sizeof(inputs));

    // note: always have a key pressed!(WASD)
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.dwFlags = KEYEVENTF_SCANCODE;
    inputs[0].ki.wScan = MapVirtualKey('W', MAPVK_VK_TO_VSC_EX);
    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
        wprintf_s(L"SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));

    SI_SendKeyStrokes(count);


    inputs[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
        wprintf_s(L"SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
}

void SI_SendKeyStrokes(int count)
{
    INPUT inputs[8];
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wScan = MapVirtualKey('D', MAPVK_VK_TO_VSC_EX);
    inputs[0].ki.dwFlags = KEYEVENTF_SCANCODE;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wScan = MapVirtualKey('W', MAPVK_VK_TO_VSC_EX);
    inputs[1].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wScan = MapVirtualKey('S', MAPVK_VK_TO_VSC_EX);
    inputs[2].ki.dwFlags = KEYEVENTF_SCANCODE;

    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wScan = MapVirtualKey('D', MAPVK_VK_TO_VSC_EX);
    inputs[3].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

    inputs[4].type = INPUT_KEYBOARD;
    inputs[4].ki.wScan = MapVirtualKey('A', MAPVK_VK_TO_VSC_EX);
    inputs[4].ki.dwFlags = KEYEVENTF_SCANCODE;

    inputs[5].type = INPUT_KEYBOARD;
    inputs[5].ki.wScan = MapVirtualKey('S', MAPVK_VK_TO_VSC_EX);
    inputs[5].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

    inputs[6].type = INPUT_KEYBOARD;
    inputs[6].ki.wScan = MapVirtualKey('W', MAPVK_VK_TO_VSC_EX);
    inputs[6].ki.dwFlags = KEYEVENTF_SCANCODE;

    inputs[7].type = INPUT_KEYBOARD;
    inputs[7].ki.wScan = MapVirtualKey('A', MAPVK_VK_TO_VSC_EX);
    inputs[7].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

    while (count--) {
        const UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
        if (uSent != ARRAYSIZE(inputs))
            wprintf_s(L"SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }
}
