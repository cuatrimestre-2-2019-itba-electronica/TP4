/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <gpio.h>


#include "fsm.h"

#include "database.h"




#include "board.h"
#include "gpio.h"
#include "8DigitsDisplay.h"
#include "3LedDsip.h"
#include "SysTick.h"
#include "encoder.h"
#include "magnetic_band.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void delayLoop(uint32_t veces);


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	timerInit();
	//_7SegDisp_display_init();
	_8DigitDisplay_init();
	_3LedDisp_dispInit();
	encoder_init();
	init_mag_card();
	database_populate();
}

/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{
	FSM * machine  = FSM__create();
	while(!FSM_cycle(machine));


}


/*******************************************************************************
 ******************************************************************************/
