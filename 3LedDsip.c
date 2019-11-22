#include "3LedDsip.h"
#include "gpio.h"
#include "board.h"
#include"SysTick.h"

#define LED_1_POS 0
#define LED_1_MASK (1UL << LED_1_POS)
#define LED_2_POS 1
#define LED_2_MASK (1UL << LED_2_POS)
#define LED_3_POS 2
#define LED_3_MASK (1UL << LED_3_POS)


#define MULTIPLEX_LED_FREC 1000
#define MULTIPLEX_LED_PRESCALER (SYSTICK_ISR_FREQUENCY_HZ/MULTIPLEX_LED_FREC)
#define LEDS_OFF 3

static uint8_t ledMask[3]={LED_1_MASK,LED_2_MASK,LED_3_MASK};
static uint8_t leds=0; //almaceno el estado de los leds


static void setMux(uint8_t disp){
	if(disp>3){
		disp=3;
	}
	bool s0 [4]={HIGH,HIGH,LOW,LOW};
	bool s1 [4]={HIGH,LOW,HIGH,LOW};
	gpioWrite(PIN_3LED_SE10,s0[disp]);
	gpioWrite(PIN_3LED_SE11,s1[disp]);
}

bool _3LedDisp_getLedStaus(uint8_t pos){
	pos%=LED_AMAUNT;
	return (leds & ledMask[pos]);
}

static void IrqDispShow(void){
	static uint32_t prescalerCounter=0;
	static uint8_t currentLed=0;
	if((prescalerCounter%=MULTIPLEX_LED_PRESCALER)==0){
		if(leds){ //chequeo si hay algun led prendido
			if(_3LedDisp_getLedStaus(currentLed)){
				setMux(currentLed);
			}
		}else{
			setMux(LEDS_OFF);
		}
		currentLed++;
		currentLed%=LED_AMAUNT;
	}
	prescalerCounter++;
}

void _3LedDisp_dispInit(void){
	gpioMode(PIN_3LED_SE10 ,OUTPUT);
	gpioMode(PIN_3LED_SE11 ,OUTPUT);
	leds=0;//inicializo todos los leds prendidos
	SysTick_append(IrqDispShow);
}

void  _3LedDisp_setLed(uint8_t pos,bool status){
	pos%LED_AMAUNT;
	leds&=~ledMask[pos];
	if(status){
		leds|=ledMask[pos];
	}

}
