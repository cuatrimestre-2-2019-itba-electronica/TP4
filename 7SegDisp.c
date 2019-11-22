#include "7SegDisp.h"
#include <stdbool.h>
#include<stdint.h>
#include"SysTick.h"
#include"board.h"

static uint8_t cseg[4];//pin values(HIGH O LOW) for each 7seg
static uint8_t current7Seg=0;//7 segmento que se esta senalando en el momento

static uint8_t brightLevel=2;
static uint8_t cursorPos=0;

/* Masks */

#define display_CSEGA_Pos          	0U
#define display_CSEGA_Msk 			(1UL << display_CSEGA_Pos)

#define display_CSEGB_Pos          	1U
#define display_CSEGB_Msk 			(1UL << display_CSEGB_Pos)

#define display_CSEGC_Pos          	2U
#define display_CSEGC_Msk 			(1UL << display_CSEGC_Pos)

#define display_CSEGD_Pos          	3U
#define display_CSEGD_Msk 			(1UL << display_CSEGD_Pos)

#define display_CSEGE_Pos          	4U
#define display_CSEGE_Msk 			(1UL << display_CSEGE_Pos)

#define display_CSEGF_Pos          	5U
#define display_CSEGF_Msk 			(1UL << display_CSEGF_Pos)

#define display_CSEGG_Pos          	6U
#define display_CSEGG_Msk 			(1UL << display_CSEGG_Pos)

#define display_CSEGDP_Pos          7U
#define display_CSEGDP_Msk 			(1UL << display_CSEGDP_Pos)


#define FREC_MULTIPLEXOR	200U //frecuencia con las que se cambia de digito
#define FREC_BRIGHTNESS	5000U //frecuencia de las simulaion del PWM


#define CLEAR_DISPLAY 0

#define PRESCALER_MULTIPLEXOR (FREC_BRIGHTNESS/FREC_MULTIPLEXOR)

#define PRESCALER_PWM (SYSTICK_ISR_FREQUENCY_HZ/FREC_BRIGHTNESS)

static void IrqAllInclusive(void);


/**
 * @brief turn an int into a 7seg pin state
 * @param num numbre to turn into a 7seg
 */
uint8_t display_set_cseg(unsigned int num){
	uint8_t alfabeto[ALPHABET]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,display_CSEGG_Msk,0};
	if(num<ALPHABET){
		return alfabeto[num];
	}
	return 0;

//	switch(num){
//		case 0:
//			return(0x3F);
//			break;
//		case 1:
//			return(0x06);
//			break;
//		case 2:
//			return(0x5B);
//			break;
//		case 3:
//			return(0x4F);
//			break;
//		case 4:
//			return(0x66);
//			break;
//		case 5:
//			return(0x6D);
//			break;
//		case 6:
//			return(0x7D);
//			break;
//		case 7:
//			return(0x07);
//			break;
//		case 8:
//			return(0x7F);
//			break;
//		case 9:
//			return(0x6F);
//			break;
//		case 10:
//			return(display_CSEGG_Msk);
//			break;
//		default:
//			return(0x00);
//			break;
//	}
}

/**
 * @brief turn on off each gpio
 */
static void display_CSEG_show(int PIN_CSEG, int PIN_LEVEL){
	if(PIN_LEVEL!=0){
		gpioWrite(PIN_CSEG, HIGH);
	}
	else{
		gpioWrite(PIN_CSEG, LOW);
	}
}

/**
 * @brief set HIGH or LOW the 7 seg pins
 */
static void setPinsState(uint8_t cSeg){
	display_CSEG_show(PIN_CSEGA, cSeg & display_CSEGA_Msk);
	display_CSEG_show(PIN_CSEGB, cSeg & display_CSEGB_Msk);
	display_CSEG_show(PIN_CSEGC, cSeg & display_CSEGC_Msk);
	display_CSEG_show(PIN_CSEGD, cSeg & display_CSEGD_Msk);
	display_CSEG_show(PIN_CSEGE, cSeg & display_CSEGE_Msk);
	display_CSEG_show(PIN_CSEGF, cSeg & display_CSEGF_Msk);
	display_CSEG_show(PIN_CSEGG, cSeg & display_CSEGG_Msk);
	display_CSEG_show(PIN_CSEGDP, cSeg & display_CSEGDP_Msk);
}

/**
 * @brief set up gpio of the mux
 * @param disp, wich 7seg i want to turn on, between 0 to 3
 */
static void setMux(uint8_t disp){
	if(disp>3){
		disp=3;
	}
	bool s0 [4]={HIGH,HIGH,LOW,LOW};
	bool s1 [4]={HIGH,LOW,HIGH,LOW};
	gpioWrite(PIN_SE10,s0[disp]);
	gpioWrite(PIN_SE11,s1[disp]);
}

bool _7SegDisp_display_init(void){
	gpioMode(PIN_CSEGA, OUTPUT);
	gpioMode(PIN_CSEGB, OUTPUT);
	gpioMode(PIN_CSEGC, OUTPUT);
	gpioMode(PIN_CSEGD, OUTPUT);
	gpioMode(PIN_CSEGE, OUTPUT);
	gpioMode(PIN_CSEGF, OUTPUT);
	gpioMode(PIN_CSEGG, OUTPUT);
	gpioMode(PIN_CSEGDP, OUTPUT);
	gpioMode(PIN_SE10, OUTPUT);
	gpioMode(PIN_SE11, OUTPUT);

	//gpioMode(PIN_MEAS, OUTPUT);//todo sacar

	setMux(0);//inicializo en rimer display el mux

	SysTick_append(IrqAllInclusive);

	for(int i=0;i<4;i++){//SACAR para pruebas nomas, seteo los display en 0 1 2 3
		cseg[i]=display_set_cseg(8);

	}

	return true;
}


/**
 * @brief turn on off each 7seg, and change multiplexor(print lcd and set bright)
 */
static void IrqAllInclusive(void){
	//gpioWrite(PIN_MEAS,true);//todo sacar
	static uint32_t CounterPrescalerPwm=0;
	static uint32_t CounterPrescalerMux=0;
	static uint32_t currentPeriod=0;
	static uint8_t turnOnOnce=0;
	static uint8_t turnOffOnce=0;
	static uint8_t lastDigit=0;

	if((CounterPrescalerPwm%=PRESCALER_PWM)==0){ //divicion del clock para el PWM
		if((currentPeriod<brightLevel)){
			if(!turnOnOnce){
				setPinsState(cseg[lastDigit]);
				turnOnOnce=1;
			}

		}else if(!turnOffOnce){
			setPinsState(0);
			turnOffOnce=1;
		}


		if((CounterPrescalerMux%=PRESCALER_MULTIPLEXOR)==0){ // divicion del clock para el multiplexeo de los 7 segmentos

			setMux(current7Seg);
			if(!turnOffOnce){
				setPinsState(cseg[current7Seg]);
			}

			lastDigit=current7Seg;
			current7Seg++;
			current7Seg=current7Seg%4;

		}
		CounterPrescalerMux++;
		currentPeriod++;
		if((currentPeriod%=BRIGHT_LEVEL)==0){
					turnOnOnce=0;
					turnOffOnce=0;
				}
	}
	CounterPrescalerPwm++;
	//gpioWrite(PIN_MEAS,false);//todo sacar
}


void _7SegDisp_setBright(uint8_t bright){
	bright%=BRIGHT_LEVEL;// aseguro que el nivel de brillo no sea mallor al esperado
	brightLevel=bright;
}


void _7SegDisp_setDigit(uint8_t pos,uint8_t digit){
	pos%=DIGIT_AMOUNT;//overflow protection
	digit%=ALPHABET;
	cseg[pos]=display_set_cseg(digit);
}

void _7SegDisp_clearDisplay(void){
	for(int i=0;i<DIGIT_AMOUNT;i++){
		cseg[i]=CLEAR_DISPLAY;
	}
}

bool _7SegDisp_setCursor(uint8_t pos){
	if(pos>DIGIT_AMOUNT)
		return false;
	cursorPos=pos;
	return true;
}

void _7SegDisp_clearCursor(void){
	for(int i=0;i<DIGIT_AMOUNT;i++){
		cseg[i]&=~display_CSEGDP_Msk;
	}
}

void _7SegDisp_updateCursor(void){
	_7SegDisp_clearCursor();
	cseg[cursorPos]|=display_CSEGDP_Msk;
}

uint8_t _7SegDisp_getCursorPos(void){
	return cursorPos;
}

void _7SegDisp_IncBright(void){
	static uint8_t currrBright=BRIGHT_LEVEL-1;
	if(!(currrBright<BRIGHT_LEVEL)){
		currrBright=0;
	}
	_7SegDisp_setBright(currrBright);
	currrBright++;
}
