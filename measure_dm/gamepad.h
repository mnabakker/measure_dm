#pragma once

#include "main.h"

BOOL GamePad_Open(DI_mode_e DI_mode);
void GamePad_Close(void);
BOOL MeasureLatency(DI_mode_e DI_mode, int numRounds);
BOOL MeasureThroughput(DI_mode_e DI_mode, int numEvents);
