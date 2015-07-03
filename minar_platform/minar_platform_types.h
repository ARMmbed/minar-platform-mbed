// Copyright (C) 2013 ARM Limited. All rights reserved.

#ifndef MINAR_PLATFORM_TYPES_H__
#define MINAR_PLATFORM_TYPES_H__

#include <stdint.h>

namespace minar {
namespace platform {

// TODO 0xc0170: this needs to be placed in mbed HAL as it's target specific. For now, not
// to bring more confusion, paste it here. I'll remove it as soon as possible.
#if defined(TARGET_K64F)
#define MINAR_PLATFORM_TIME_BASE 32768
#define MINAR_PLATFORM_TIME_MASK 0xFFFFFFFFu
#else
#error Not supported yet, don't port, will be placed properly soon.
#endif

enum Constants{
    // ticks per second (maximum resolution). This is what the OS works with.
    Time_Base = MINAR_PLATFORM_TIME_BASE,
    // 32 bits of time
    Time_Mask = MINAR_PLATFORM_TIME_MASK
};

typedef uint32_t irqstate_t;

// Internal time type
typedef uint32_t tick_t;

}; // namespace platform
}; // namespace minar

#endif // ndef MINAR_PLATFORM_TYPES_H__
