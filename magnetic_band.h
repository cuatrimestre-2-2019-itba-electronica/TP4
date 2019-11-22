/*
 * magnetic_band.h
 *
 *  Created on: Sep 7, 2019
 *      Author: gonzalosilva
 */

#ifndef MAGNETIC_BAND_H_
#define MAGNETIC_BAND_H_




  ////////////////////////characters of the magnectic card//////////////////////////////
 ///********************************************************************************///
//////////////////////////////////////////////////////////////////////////////////////
#define SS				11
#define FS				13
#define ES				15
  ////////////////////////////////////////////////////////////////////////////////////
 ///******************************************************************************///
////////////////////////////////////////////////////////////////////////////////////
#define PIN_SW3			PORTNUM2PIN(PA,4)//prueba
#define PIN_SW2			PORTNUM2PIN(PC,6)//prueba
#define DATA_LENGTH		8 //la cantidad de caracteres que se quieren recibir con get_data
#define WORD_LENGTH		5 //tamano de la palabra con el bit de paridad
#define PAIRITY_MASK 	0xEF
  ////////////////////////////////////////////////////////////////////////////////////
 ///******************************************************************************///
////////////////////////////////////////////////////////////////////////////////////
void init_mag_card(void);//inicializa los pines que usara la tarjeta magetica
uint8_t get_data(void); //devuelve caracter a caracter la informacion
bool is_loaded(void); //indica si hay informacion que no se uso, devuelve un 1 si esta cargado el buffer



#endif /* MAGNETIC_BAND_H_ */
