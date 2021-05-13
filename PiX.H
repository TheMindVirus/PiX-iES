#ifndef _PIX_H_
#define _PIX_H_

#include "ies.h"

STATIC LITERAL PIX_WELCOME_MESSAGE = "[INFO]: Hi, Welcome to PiX-iES!\n";

#define drawPixel_ARM64(x, y, colour) \
    mmio_write((Platform.MONITOR_FRAMEBUFFER + (y * Platform.MONITOR_PITCH_SPACE) + (x * 4)), colour);

#define drawPixel2_ARM64(x, y, colour) \
    mmio_write((Platform.MONITOR_FRAMEBUFFER2 + (y * Platform.MONITOR_PITCH_SPACE2) + (x * 4)), colour);

#endif//_PIX_H_