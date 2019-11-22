
/***************************************************************************//**
timer (HAL) basado en el ejemplo "timer2" visto en clase
Autor del ejemplo original: Nicolas Magliola
 ******************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "SysTick.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TIMER_TICK_US       100
#define TIMER_US2TICKS(us)  ((us)/TIMER_TICK_US)


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef int32_t ticks_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialice timer and corresponding peripheral
 */
void timerInit(void);


// Non-Blocking services ////////////////////////////////////////////////

/**
 * @brief Begin to run a new timer
 * @param ticks time until timer expires, in ticks
 * @return Timeout value
 */
ticks_t timerStart(ticks_t ticks);

/**
 * @brief Verify if a timer has run timeout
 * @param timeout timeout to check for expiration
 * @return 1 = timer expired
 */
bool timerExpired(ticks_t timeout);



/*******************************************************************************
 ******************************************************************************/



#endif /* TIMER_H_ */
