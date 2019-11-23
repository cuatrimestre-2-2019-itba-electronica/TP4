#include "systick.h"
#include "hardware.h"
#include "board.h"
#include "gpio.h"
#include "PIT.h"
#include <stdlib.h>

static SysTick_funcallback callbacks[NUMBER_OF_CALLBACKS];
static uint8_t lastCallBackPos=0;
void callBack (void);
bool SysTick_Init (void)
{
	PIT_init();
	PIT_set_timer (SYSTICK_ISR_FREQUENCY_HZ, 0);

	for(int i=0;i<NUMBER_OF_CALLBACKS;i++){
		callbacks[i]=NULL;
	}
	PIT_IRQ (callBack, 0);
	return true;
}

void callBack (void)
{

	for(int i=0;i<(lastCallBackPos);i++){
			if(callbacks[i]!=NULL){
				((*callbacks[i])());
			}
		}


}

bool SysTick_append(SysTick_funcallback func){

	if(lastCallBackPos==NUMBER_OF_CALLBACKS){
		return false;
	}
	callbacks[lastCallBackPos]=func;
	lastCallBackPos++;

	return true;
}
