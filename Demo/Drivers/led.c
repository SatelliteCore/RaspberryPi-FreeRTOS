/*
  Implementation for Pi LED control.  Has conditional code for Pi
  models 1, 2, and 3.
*/

#include "led.h"

#if RPI == 1 || RPI == 2
// Pi 1 and Pi 2 are similar except activity LED is wired to different GPIO
#include "gpio.h"

// Pi 1 activity LED is GPIO 16, Pi 2 is GPIO 47
#define LED_GPIO (RPI == 1 ? 16 : 47)

void LedInit() {
  SetGpioFunction(LED_GPIO, 1);	 // RDY led
}

void LedOn() {
  SetGpio(LED_GPIO, 0);
}
void LedOff() {
  SetGpio(LED_GPIO, 1);
}

#elif RPI == 3

#include <stdbool.h>

// Will hold address of activity LED, set by LedInit()
static volatile unsigned int *pLed;

// current state of led
static bool on = false;


// TODO: look in linux kernel here for why this works
//
// https://github.com/raspberrypi/linux/blob/02ce9572cc77c65f49086bbc4281233bd3fa48b7/drivers/gpio/gpio-bcm-virt.c

void LedInit(void) {
  // Get access to LED.
  //
  // Converted from undocumented asm code, documentation below is a
  // guess.  Use a mailbox to ask GPU for access to LED, returning an
  // address.

  // Request message (TODO: find out what fields mean)
  unsigned int request[7] __attribute__((aligned(16))) = {
    0x1c, 0, 0x00040010, 4, 0, 0, 0
  };

  // Mailbox address?
  volatile unsigned int *pBal = (unsigned int *)0x3f00b880;

  // Wait until can make a request - GPU will set bit when available
  while (pBal[6] & 0x80000000) {
    // twiddle thumbs
  }

  // Put request in mailbox
  pBal[8] = (unsigned int)(request + 2);

  // Wait for reply from GPU
  while (pBal[6] & 0x40000000) {
    // twiddle thumbs
  }

  // got reply - save address of led, masking off the top 2 bits.
  pLed = (unsigned int *)(request[5] & ~0xc0000000);
}

void LedOn(void) {
  // Converted from undocumented asm code to turn LED on
  if (!on) {
    on = true;
    *pLed += 0x00010000;
  }
}

void LedOff(void) {
  // Converted from undocumented asm code to turn LED off
  if (on) {
    on = false;
    *pLed += 0x00000001;
  }
}


#else
#error Unknown RPI model or RPI not defined (e.g -DRPI=3 option)
#endif
