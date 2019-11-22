/***************************************************************************//**
  @file     8DigitsDisplay.h
  @brief    Display 8 digits driver
  @author   Matias Pierdominici

 ******************************************************************************/

#ifndef _8DIGITSDISPLAY_H_
#define _8DIGITSDISPLAY_H_

#include <stdbool.h>
#include <stdint.h>

#define AMAUNT_DIGITS_8_DISPLAY 8 //cantidad de digitos del display emulado
#define _8DIGIT_BLANK 11
#define _8DIGIT_GUION 10



/**
 * @brief llama al initi del driver de los led 7 segmentos
 */
void _8DigitDisplay_init(void);
/**
 * @brief lleva el display al mismo estado que cuando se llamo a init, se borra el display y se lleva la posicion a cero
 */
void _8DigitDisplay_reset(void);
/**
 * @brief luego de llamar a reset o init se utiliza para agregar caracteres a pantalla.
 * lo primero que se agrga es lo mas significativo.
 * IMPORTANTE agragar informacion antes de utilizar las otras funciones del display
 */
void _8DigitDisplay_append(uint8_t num);
/**
 * @brief prende el cursor y actualiza la pantalla. si el cursor se econtrava dentro de la pantalla se motrara, de lo contrario
 * se debe llamar a _8DigitDisplay_SetCursorPosOnScrenn para que el cursor aparesca dentro de la pantalla
 */
void _8DigitDisplay_cursorOn(void);
/**
 * @brief apaga el cursor de la pantalla pero no se pirrde su posicion
 */
void _8DigitDisplay_cursorOff(void);
/**
 * @brief setea el cursor en la posicion, si esta apago no se muestr. si esta prendido y dentro de la pantalla si se muestra.
 * de lo contrario llamae a SetCursorPosOnScren
 */
bool _8DigitDisplay_SetCursorPos(uint8_t pos);
/**
 * @brief devuelve la popsicion del cursor
 */
uint8_t _8DigitDisplay_GetCursorPos(void);
/**
 * @brief cambia el caracter en la posicion del cursor
 */
void _8DigitDisplay_SetCharInCursorPos(uint8_t num);
/**
 * @brief pone la posicion del cursor al numero menos signigicativo
 */
void _8DigitDisplay_SetCursorPos2End(void);
/**
 * @brief mueve el contenido del display para que el cursor aparesca en pantalla
 */
void _8DigitDisplay_SetCursorPosOnScrenn(void);
/**
 * @brief shiftea el cursor a la izquierda
 */
void _8DigitDisplay_ShiftCursorLeft(void);
/**
 * @brief cuando es true pone guienes en donde no esta el cursor
 */
void _8DigitDisplay_PinMode(bool mode);

/**
 * @brief setea el brillo del display
 */
void _8DigitDisplay_SetBright(uint8_t b);
/**
 * @brief incrementa brillo y si me paso vuelve al principio
 */

void _8DigitDisplay_IncBright(void);






//scroll_on();
//scroll_off();   //pasa a mostrar solo el final de la palabra
//scroll(bool on);



////mode pin(on/off)

////////////////////////////////////////////////
//
//int get_size();
//uint8_t get_next_digit();



#endif
