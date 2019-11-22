/*
 * magnectic_band.c
 *
 *  Created on: Sep 6, 2019
 *      Author: gonzalosilva
 */

#include <gpio.h>
#include "board.h"
#include "magnetic_band.h"
#include "gpio.h"


static void mag_data (void);//se obtiene bit a bit la data de la tarjeta
static void reset_mag(void);//resetea todos los valores luego que la informacion haya sido enviada
static void error_reset(void);

static unsigned int counter = 0; //contador general
static unsigned int count = 0; //contador de cada caracter
static unsigned int data_send_counter = 0;//contador del envio de informacion, esta se envia caracter a caracter
static unsigned int parity = 0; //para verificar el bit de paridad
static bool after_SS = false;
static bool loaded = false; //si ya tengo informacion en el arreglo y no me la pidieron, este queda en 1
static uint8_t code[40]; //buffer donde se guarda la informacion sin el SS
static uint8_t LCR_control[] = {1,0,0,1};
static uint8_t card_character = 0;
static bool error = false;


void init_mag_card(void) {
    gpioMode(CARD_ENABLE, INPUT);
    gpioIRQ(CARD_ENABLE, GPIO_IRQ_MODE_RISING_EDGE, error_reset);

    gpioMode(CARD_CLOCK, INPUT);
    gpioIRQ(CARD_CLOCK, GPIO_IRQ_MODE_FALLING_EDGE, mag_data);

    gpioMode(CARD_DATA, INPUT);
}


void mag_data (void){
	if (!gpioRead(CARD_ENABLE) && loaded == false && error == false){ //si se esta pasando la tarjeta
		if(!after_SS){ //si todavia no se encontro el inicio de los caracteres de la tarjeta
			card_character = card_character >> 1;
			card_character |= ((!gpioRead(CARD_DATA)) << (WORD_LENGTH-1));
			if(card_character == SS){
				after_SS = true;
				card_character = 0;
			}
		}
		else{ //una vez que se ecnontro el primer caracter (SS) empiezan a buscarse todos los restantes
			if (count < WORD_LENGTH){
				card_character |= (((!gpioRead(CARD_DATA))) << count);
				count++;
				parity += (!gpioRead(CARD_DATA));
				LCR_control[count] += (!gpioRead(CARD_DATA));
			}
			if(count == WORD_LENGTH){
				if(!(parity%2) && code[counter-1] != ES){ //si tiene una cantidad pares de 1s, se leyo mal algun dato
					error = true;
					reset_mag();
				}
				else{
					code[counter] = (card_character & PAIRITY_MASK);
					counter++;
					count = 0;
					card_character = 0;
					parity = 0;
				}
			}

			if (code[counter-2] == ES && error == false){ //si el es final de la informacion se pone como cargado y ya no se vuelve a modificar hasta que se pida esta informacion
				loaded = true;
				for(int i = 0; i < WORD_LENGTH-1; i++){
					if (!(LCR_control[i]%2)){
						error = true;
						reset_mag();
					}
				}
			}
		}
	}
}
uint8_t get_data(void){
	if(loaded == true){
		if (data_send_counter < DATA_LENGTH){
			return code[data_send_counter++];
		}
		else{
			reset_mag();
			return 255;
		}
	}
}

bool is_loaded(void){
	return loaded;
}

void reset_mag(void){
	data_send_counter = 0;
	counter = 0;
	count = 0;
	loaded = false;
	card_character = 0;
	after_SS = false;
	parity = 0;
	LCR_control[0] = 1;
	LCR_control[1] = 0;
	LCR_control[2] = 0;
	LCR_control[3] = 1;
	for (int i = 0; i < 40; i++)
		code[i] = 0;

}

void error_reset(void){
	if(code[counter-2] != ES)
		reset_mag();
	error = false;
}
