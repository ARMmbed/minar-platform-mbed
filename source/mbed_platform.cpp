// Copyright (C) 2015 ARM Limited. All rights reserved.

#include "minar/minar_platform.h"

#include "mbed-hal/lp_ticker_api.h"
#include "mbed-hal/sleep_api.h"

#define __CMSIS_GENERIC
#if defined(TARGET_LIKE_CORTEX_M3)
  #include "cmsis-core/core_cm3.h"
#elif defined(TARGET_LIKE_CORTEX_M4)
  #include "cmsis-core/core_cm4.h"
#else
  #error MINAR is only supported on Cortex-M3 and Cortex-M4 MCUs at the moment
#endif

/// @name Local Constants
const static minar::platform::tick_t Minimum_Sleep = 10; // in Platform_Time_Base units

/// @name Local function declarations
static minar::platform::tick_t maskTime(minar::platform::tick_t t);
static bool timeIsInPeriod(minar::platform::tick_t start, minar::platform::tick_t time, minar::platform::tick_t end);

///////////////////////////////////////////////////////////////////////////////

static void mbed_sleep()
{
    sleep();
}

namespace minar {
namespace platform {

irqstate_t pushDisableIRQState() {
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

void sleepFromUntil(tick_t now, tick_t until){
    /* be defensive against non-masked times being passed*/
    now   = maskTime(now);
    until = maskTime(until);
    // use real-now for front-most end of do-not-sleep range check
    // !!! FIXME: looks like there's actually a race condition here that could
    // cause wakeup not to work properly
    const tick_t real_now = getTime();
    if(timeIsInPeriod(now, until, maskTime(real_now + Minimum_Sleep))){
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

uint32_t getTimeOverflows(){
    return lp_ticker_get_overflows();
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

static minar::platform::tick_t maskTime(minar::platform::tick_t t){
    return t & minar::platform::Time_Mask;
}
