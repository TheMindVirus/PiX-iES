#ifndef PIX_H
#define PIX_H

#include "iES.H"

STATIC LITERAL PIX_WELCOME_MESSAGE = "Hello, World! Welcome to PiX.";

#define drawPixel_ARM64(x, y, colour) \
    mmio_write((Platform.MONITOR_FRAMEBUFFER + (y * Platform.MONITOR_PITCH_SPACE) + (x * 4)), colour);

#define drawPixel2_ARM64(x, y, colour) \
    mmio_write((Platform.MONITOR_FRAMEBUFFER2 + (y * Platform.MONITOR_PITCH_SPACE2) + (x * 4)), colour);

#define ONE_SECOND   0x100000 // 1048576 ticks
#define ONE_TENTH    0x1999A // 104858 ticks
//WARNING: SYSTEM TIMER IS PRONE TO 64-BIT OVERFLOW AND PRECISION ERROR!

#endif//PIX_H