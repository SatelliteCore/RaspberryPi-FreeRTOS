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
#include <stdint.h>

// This is based on linux kernel, see following:
//
// https://github.com/raspberrypi/linux/blob/02ce9572cc77c65f49086bbc4281233bd3fa48b7/drivers/gpio/gpio-bcm-virt.c

// Activity LED is controlled by GPU.  GPU appears to watch these counters
// which we can write to.
struct LedCtrl {
    uint16_t disableCount;
    uint16_t enableCount;
};

// Address of control for activity LED, set by LedInit()
static volatile struct LedCtrl *pLedCtrl;

// Current state of LED
static bool on = false;


void LedInit(void) {
  // Get access to LED controlled by GPU.
  //
  // Converted from undocumented asm code, documentation below is a rough.
  // Use a mailbox to ask GPU for access to LED, returning an address.

  // Request message (TODO: document what fields mean)
  uint32_t request[7] __attribute__((aligned(16))) = {
    0x1c,
    0,
    0x00040010,   // RPI_FIRMWARE_FRAMEBUFFER_GET_GPIOVIRTBUF
    4,            // size of result (32-bit address)
    0, 0, 0
  };

  // Mailbox address
  volatile uint32_t *pBal = (uint32_t *)0x3f00b880;

  // Wait until can make a request - GPU will set bit when available
  while (pBal[6] & 0x80000000) {
    // twiddle thumbs
  }

  // Put request in mailbox
  pBal[8] = (uint32_t)(request + 2);

  // Wait for reply from GPU
  while (pBal[6] & 0x40000000) {
    // twiddle thumbs
  }

  // got reply - save address of led, masking off the top 2 bits.
  pLedCtrl = (struct LedCtrl *)(request[5] & ~0xc0000000);
}

void LedOn(void) {
  // Converted from undocumented asm code to turn LED on
  if (!on) {
    on = true;
    ++pLedCtrl->enableCount;
  }
}

void LedOff(void) {
  // Converted from undocumented asm code to turn LED off
  if (on) {
    on = false;
    ++pLedCtrl->disableCount;
  }
}


#else
#error Unknown RPI model or RPI not defined (e.g -DRPI=3 option)
#endif
