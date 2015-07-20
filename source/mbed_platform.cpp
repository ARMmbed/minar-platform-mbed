// Copyright (C) 2015 ARM Limited. All rights reserved.

#include "minar_platform/minar_platform.h"

#include "mbed-hal/lp_ticker_api.h"
#include "mbed-hal/sleep_api.h"
#include "cmsis-core/core_generic.h"

/// @name Local Constants
const static minar::platform::tick_t Minimum_Sleep = MINAR_PLATFORM_MINIMUM_SLEEP; // in Platform_Time_Base units

/// @name Local function declarations
static bool timeIsInPeriod(minar::platform::tick_t start, minar::platform::tick_t time, minar::platform::tick_t end);

///////////////////////////////////////////////////////////////////////////////

static void mbed_sleep(){
    sleep();
}

namespace minar {
namespace platform {

irqstate_t pushDisableIRQState(){
    uint32_t ret = __get_PRIMASK();
    __disable_irq();
    return (irqstate_t)ret;
}

void popDisableIRQState(irqstate_t restore){
    __set_PRIMASK((uint32_t)restore);
}

void init(){
    lp_ticker_init();
}

void sleep(){
    mbed_sleep();
}

tick_t getTime() {
    return lp_ticker_read();
}

uint32_t getTimeOverflows(){
    return lp_ticker_get_overflows_counter();
}

void sleepFromUntil(tick_t now, tick_t until){
    // use real-now for front-most end of do-not-sleep range check
    const tick_t real_now = getTime();
    if(timeIsInPeriod(now, until, real_now + Minimum_Sleep)){
        // in this case too soon to go to sleep, just return
        return;
    } else {
        const uint32_t next_int = lp_ticker_get_compare_match();

        if(timeIsInPeriod(now, until, next_int)){
            lp_ticker_set_interrupt(now, until);
        } else {
            // existing compare match is sooner, do nothing
        }
        sleep();
    }
}

}; // namespace platform
}; // namespace minar

// return true if `time' is in the range [`start', `end'), where
// all the range may wrap around to zero (`end' may be less than
// `start')
static bool timeIsInPeriod(minar::platform::tick_t start, minar::platform::tick_t time, minar::platform::tick_t end){
    // Taking care to handle wrapping: (M = now + Minumum_Sleep)
    //   Case (A.1)
    //                       S    T   E
    //      0 ---------------|----|---|-- 0xf
    //
    //   Case (A.2): this case also allows S==T==E
    //         E                 S    T
    //      0 -|-----------------|----|-- 0xf
    //
    //   Case (B)
    //         T   E                 S
    //      0 -|---|-----------------|--- 0xf
    //
    if((time >= start && ( time < end ||    // (A.1)
                          start >= end)) || // (A.2)
        (time < start && end < start && end > time)){  // (B)
        return true;
    }
    return false;
}
