/***************************************************************************//**
  @file     3LedDisp.h
  @brief    Leds driver
  @author   Matias Pierdominici
 ******************************************************************************/
#ifndef _3LEDDISP_H_
#define _3LEDDISP_H_
#include <stdbool.h>
#include <stdint.h>

#define LED_AMAUNT 3
/**
 * @brief set up pin modes and irq
  */
void _3LedDisp_dispInit(void);

/**
 * @brief return if a led is on(true) or off(false)
 * @param pos, between 0 to LED_AMAUNT-1
 */
bool _3LedDisp_getLedStaus(uint8_t pos);

/**
 * @brief turn on or off each led
 * @param pos, between 0 to LED_AMAUNT-1
 * @param status, true to turn on led, flase to turn off led
 */
void  _3LedDisp_setLed(uint8_t pos,bool status);

#endif
