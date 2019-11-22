/***************************************************************************//**
  @file     display.h
  @brief    Display driver
  @author   Arturo & Matias

 ******************************************************************************/

#ifndef _7SEGDISP_H_
#define _7SEGDISP_H_

#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>

#define DIGIT_AMOUNT 4//cantidad de digitos del display
#define BRIGHT_LEVEL 4 // cantidad de niveles de brillo
#define ALPHABET 12//cantidad de simbolos del alfabeto
#define _7DIGIT_BLANK 11
#define _7DIGIT_GUION 10

/**
 * @brief set up  the pins for 7seg display, as output
 */
bool _7SegDisp_display_init(void); // Init display



/**
 * @brief set 7seg bright
 * @param bright level, between 0 to BRIGHT_LEVEL-1
 */
void _7SegDisp_setBright(uint8_t b);

/**
 * @brief print numbers in display
 * @param pos, between 0 to BRIGHT_LEVEL-1
 * @param digit, between 0 to ALPHABET-1
 */
void _7SegDisp_setDigit(uint8_t pos,uint8_t digit);

/**
 * @brief clear display content
 */
void _7SegDisp_clearDisplay(void);

/**
 * @brief set dot of 7seg lcd to be turn on
 * @param pos, between 0 to BRIGHT_LEVEL-1, if pos is grater, then de pos wil be set to cero
 */
bool _7SegDisp_setCursor(uint8_t pos);

/**
 * @brief turn off the dop of de lcd 7seg
 */
void _7SegDisp_clearCursor(void);

/**
 * @brief turn on de dot of the 7seg disp
 */
void _7SegDisp_updateCursor(void);

/**
 * @brief return cursor position
 */
uint8_t _7SegDisp_getCursorPos(void);

/**
 * @brief incrementa brillo y si me paso vuelve al principio
 */
void _7SegDisp_IncBright(void);
#endif // _DISPLAY_H_
