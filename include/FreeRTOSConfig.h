#pragma once

// This example uses a common include to avoid repetition
#include "FreeRTOSConfig_examples_common.h"


#undef configCHECK_FOR_STACK_OVERFLOW
#define configCHECK_FOR_STACK_OVERFLOW 2

#undef configUSE_STATS_FORMATTING_FUNCTIONS
#define configUSE_STATS_FORMATTING_FUNCTIONS 1

#undef configGENERATE_RUN_TIME_STATS
#define configGENERATE_RUN_TIME_STATS 1

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()

#ifdef __cplusplus
extern "C" {
#endif
extern uint64_t time_us_64(void);						// "hardware/timer.h"
#ifdef __cplusplus
}
#endif

#define RUN_TIME_STAT_time_us_64Divider 1000			// stat granularity is mS
#define portGET_RUN_TIME_COUNTER_VALUE() (time_us_64()/RUN_TIME_STAT_time_us_64Divider)	// runtime counter in mS