#pragma once

#include "WinSDKVer.h"
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <xinput.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// note: define this to log the controller positions received during the throughput test
//#define SAVE_THROUGHPUT_POSITIONS_TO_FILE

// note: define this to use event notification instead of polling on buffered reads
//#define DI_USE_BUFFERED_WITH_EVENT_NOTIFICATION


// Direct Input modes available for testing
typedef enum DI_mode_e
{
    DI_BUFFERED = 0,
    DI_IMMEDIATE,
    DI_MIXED
} DI_mode_e;
