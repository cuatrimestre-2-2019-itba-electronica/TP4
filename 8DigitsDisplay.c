#include "8DigitsDisplay.h"
#include "7SegDisp.h"
static uint8_t data[AMAUNT_DIGITS_8_DISPLAY+DIGIT_AMOUNT-1]; //arreglo de los numores del display emulado, la posicion 0 es el mas significativo. ej 340 -> 3|4|0|..
uint8_t *_8digits;
static int8_t cursorPosition=0;//posicion del cursor - dot
static int8_t lastDigitPos=-1;//posicion del ultimo digito del display - otambien tamano del display menos uno
static int8_t DispOffset=0;//ofeset para mover cambiar la ventana de lo que se ve en los 7seg- maximo ofset es 4
static bool showCursor=false;//muestro o no el cursor en pantalla (true) - dot
static bool pinMode=false;

void _8DigitDisplay_reset(void){
	_7SegDisp_clearDisplay();
	for(int i=0;i<AMAUNT_DIGITS_8_DISPLAY+DIGIT_AMOUNT-1;i++){
		data[i]=_7DIGIT_BLANK;
	}
	cursorPosition=0;
	lastDigitPos=-1;
	DispOffset=0;
	showCursor=false;
	pinMode=false;
}

void _8DigitDisplay_init(void){
	_7SegDisp_display_init();
	_8DigitDisplay_reset();
	_8digits=data+(DIGIT_AMOUNT-1);

}

static void refresh7SegDisp(void){
	_7SegDisp_clearCursor();
	if(DispOffset>AMAUNT_DIGITS_8_DISPLAY-1){
		DispOffset=AMAUNT_DIGITS_8_DISPLAY-1;
	}
	for(int i=0;i<DIGIT_AMOUNT;i++){
		if(pinMode){//modo pin
			if((DIGIT_AMOUNT-1+DispOffset-i)==(cursorPosition+DIGIT_AMOUNT-1)){
				_7SegDisp_setDigit(i,data[DIGIT_AMOUNT-1+DispOffset-i]);
			}else{
				if(data[DIGIT_AMOUNT-1+DispOffset-i]==_7DIGIT_BLANK){
					_7SegDisp_setDigit(i,_7DIGIT_BLANK);
				}else{
					_7SegDisp_setDigit(i,_7DIGIT_GUION);
				}

			}
		}else{
			_7SegDisp_setDigit(i,data[DIGIT_AMOUNT-1+DispOffset-i]);
		}//fin modo pin
		if((DIGIT_AMOUNT-1+DispOffset-i)==(cursorPosition+DIGIT_AMOUNT-1)){//cursor
			if(showCursor){
				_7SegDisp_setCursor(i);
				_7SegDisp_updateCursor();
			}
		}//fin cursor
	}
}

static bool shiftLeftDisp(void){


	if(DispOffset>AMAUNT_DIGITS_8_DISPLAY-1){
		DispOffset=AMAUNT_DIGITS_8_DISPLAY-1;
		refresh7SegDisp();
		return false;
	}else{

		refresh7SegDisp();
		DispOffset++;
		return true;
	}

}

static bool shiftRightDisp(void){
	DispOffset--;
	if(DispOffset<0){
		DispOffset=0;
		refresh7SegDisp();
		return false;
	}else{
		refresh7SegDisp();
		return true;
	}

}

void _8DigitDisplay_append(uint8_t num){
	lastDigitPos++;
	if(lastDigitPos>AMAUNT_DIGITS_8_DISPLAY-1){
		lastDigitPos=AMAUNT_DIGITS_8_DISPLAY-1;
	}

	_8digits[lastDigitPos]=num;
	cursorPosition=lastDigitPos;

	shiftLeftDisp();

}

void _8DigitDisplay_cursorOn(void){
	showCursor=true;
	refresh7SegDisp();
}

void _8DigitDisplay_cursorOff(void){
	showCursor=false;
	refresh7SegDisp();
}

bool _8DigitDisplay_SetCursorPos(uint8_t pos){
	if(pos>AMAUNT_DIGITS_8_DISPLAY-1){
		return false;
	}
	cursorPosition=pos;
	refresh7SegDisp();
	return true;

}

uint8_t _8DigitDisplay_GetCursorPos(void){
	return cursorPosition;
}

void _8DigitDisplay_SetCharInCursorPos(uint8_t num){
	_8digits[cursorPosition]=num;
	refresh7SegDisp();

}

void _8DigitDisplay_SetCursorPos2End(void){
	cursorPosition=lastDigitPos;
	refresh7SegDisp();
}

void _8DigitDisplay_SetCursorPosOnScrenn(void){
		if(cursorPosition<DIGIT_AMOUNT){
			DispOffset=DIGIT_AMOUNT-1;
		}else{
			DispOffset=AMAUNT_DIGITS_8_DISPLAY-1;
		}
		refresh7SegDisp();
}
void _8DigitDisplay_ShiftCursorLeft(void){
	cursorPosition--;
	if(cursorPosition<0){
		cursorPosition=0;
	}
	_8DigitDisplay_SetCursorPosOnScrenn();

}

void _8DigitDisplay_ShiftCursorRight(void){
	cursorPosition++;
	if(!(cursorPosition<AMAUNT_DIGITS_8_DISPLAY)){
		cursorPosition=AMAUNT_DIGITS_8_DISPLAY-1;
	}
	_8DigitDisplay_SetCursorPosOnScrenn();

}

void _8DigitDisplay_PinMode(bool mode){
	pinMode=mode;
}
void _8DigitDisplay_SetBright(uint8_t b){
	_7SegDisp_setBright(b);
}

void _8DigitDisplay_IncBright(void){
	_7SegDisp_IncBright();
}


//	|	|	|	0|	|	|	3|	4|	|	|	7|

