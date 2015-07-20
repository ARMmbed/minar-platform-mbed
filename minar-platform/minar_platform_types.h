// Copyright (C) 2013 ARM Limited. All rights reserved.

#ifndef MINAR_PLATFORM_TYPES_H
#define MINAR_PLATFORM_TYPES_H

#include <stdint.h>
#include "objects.h" //TODO: Replace by the proper target config file

namespace minar {
namespace platform {

enum Constants{
    // ticks per second (maximum resolution). This is what the OS works with.
    Time_Base = MINAR_PLATFORM_TIME_BASE,
    // 32 bits of time for mbed platforms
    Time_Mask = 0xFFFFFFFFu
};

typedef uint32_t irqstate_t;

// Internal time type
typedef uint32_t tick_t;

}; // namespace platform
}; // namespace minar

#endif // ndef MINAR_PLATFORM_TYPES_H
