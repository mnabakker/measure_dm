
# Double Movement performance utility

This software measures latencies from the HIQ(Hardware Input Queue) up until the actual controller movement as it would be detected by games.
- SendInput() is used to add Keyboard events to the HIQ
- For x360 controllers XInput is used to detect movement
- For DS4 controllers DirectInput is used to detect movement

## Warning
- For measuring Keys2XInput v2.2.2 and up, you have to deactivate their Fortnite bug 'Bus jump workaround'

## Notes
- DirectInput will use 'immediate' mode for latency testing, and 'buffered' mode for throughput testing, you can change this behavior via cmd-line
- XInput only has an 'immediate' mode
- DirectInput will generate 2 game-pad events if both axis have changed (1 event per axis change), XInput does not.

## Setup a consistent test environment:
    - disable ALL boosting/throttling in your BIOS
    - use a NEW windows installation
    - set Windows power-settings on performance mode
    - disable all drivers/services you do not need to perform this test
    - only install the double-movement software you wish to test

## current take-aways:
    - Numbers suggest XInput has a slightly lower latency then DirectInput
        Thus, if Fortnite uses the XInput API for x360 devices, you should use x360 emulation.
    - DirectInput will generate 2 game-pad events if both axis have changed (1 event per axis change), XInput does not.
        Thus, a game has more state changes to deal with. [and 1 of them is an intermediate state you do not desire]
    - Do not stare blindly at the lower latency from the 'LowLevel Input Capture' option in Keys2XInput
        Before that, you need to 100% understand the relation of the times displayed in the throughput tests
