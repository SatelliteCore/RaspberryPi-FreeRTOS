/*
  LED control for Pi

  This module provides functions to control the Pi on-board activity LED.
*/

#ifndef DRIVERS_LED_H
#define DRIVERS_LED_H

/**
 * Call at least once before any other Ledxxx() functions to set things up.
 */
void LedInit(void);

/**
 * Turn the on-board activity LED on
 */
void LedOn(void);

/**
 * Turn the on-board activity LED off
 */
void LedOff(void);

#endif //DRIVERS_LED_H
