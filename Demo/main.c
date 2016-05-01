#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"

enum DemoType {
  MorsecodeDemo,
  SosDemo,
  BlinkyDemo
};

// Pick demo type to run
//enum DemoType demoType = MorsecodeDemo;
enum DemoType demoType = SosDemo;

#define UNUSED(v) (void)(v)

#if RPI == 3

static volatile unsigned int *pLed;

void ledInit(void) {
  // TODO: remove static?
  static unsigned int request[7] __attribute__((aligned(16))) = {
    0x1c, 0, 0x00040010, 4, 0, 0, 0
  };

  volatile unsigned int *pBal = (unsigned int *)0x3f00b880;

  while (pBal[6] & 0x80000000) {}
  pBal[8] = (unsigned int)(request + 2);

  while (pBal[6] & 0x40000000) {}
  pLed = (unsigned int *)(request[5] & ~0xc0000000);
}

void ledOn(void) {
  *pLed += 0x00010000;
}

void ledOff(void) {
  *pLed += 0x00000001;
}

#elif RPI == 2 || RPI == 1

// Pi 1 activity LED is GPIO 16, Pi 2 is GPIO 47
#define LED_GPIO (RPI == 1 ? 16 : 47)

void ledInit() {
  SetGpioFunction(LED_GPIO, 1);	 // RDY led
}

void ledOn() {
  SetGpio(LED_GPIO, 0);
}
void ledOff() {
  SetGpio(LED_GPIO, 1);
}
#else
#error Unknown RPI model
#endif

void dot(void) {
  ledOn();
  vTaskDelay(200);
  ledOff();
  vTaskDelay(200);
}

void dash(void) {
  ledOn();
  vTaskDelay(800);
  ledOff();
  vTaskDelay(200);
}

xQueueHandle morsecodeQueue = NULL;

void senderTask(void *pParam) {
  UNUSED(pParam);
  char s = 's';
  char o = 'o';
  for (;;) {
    xQueueSend(morsecodeQueue, &s, 0);
    xQueueSend(morsecodeQueue, &o, 0);
    xQueueSend(morsecodeQueue, &s, 0);
    vTaskDelay(20000);
  }
}

void morsecodeTask(void *pParam) {
  UNUSED(pParam);

  for (;;) {
    char letter = ' ';
    if (xQueueReceive(morsecodeQueue, &letter, 1000)) {
      if (letter == 's') {
	dot();
	dot();
	dot();
      }
      else if (letter == 'o') {
	dash();
	dash();
	dash();
      }
      else {
	// FIXME: add all the other letters and numbers
	dash();
      }
    }
    else {
      dot();
    }
  }
}


void sos(void *pParam) {
  UNUSED(pParam);

  for (;;) {
    dot();
    dot();
    dot();

    dash();
    dash();
    dash();

    dot();
    dot();
    dot();

    vTaskDelay(1600);
  }
}

void task1(void *pParam) {
  UNUSED(pParam);
	int i = 0;
	while(1) {
		i++;
		ledOff();
		vTaskDelay(200);
	}
}

void task2(void *pParam) {
  UNUSED(pParam);

	int i = 0;
	while(1) {
		i++;
		vTaskDelay(100);
		ledOn();
		vTaskDelay(100);
	}
}

volatile unsigned int *pHtr = (unsigned int *)0x3f003004;

void delay(unsigned int ms)
{
  unsigned int t0 = *pHtr;
  while (*pHtr - t0 < ms*1000) {}
}

/**
 *	This is the systems main entry, some call it a boot thread.
 *
 *	-- Absolutely nothing wrong with this being called main(), just it doesn't have
 *	-- the same prototype as you'd see in a linux program.
 **/
int main(void) {
	ledInit();
	ledOn();
#if 0
	for (;;) {
	  delay(500);
	  ledOff();
	  delay(500);
	  ledOn();
	}
#endif
	DisableInterrupts();
	InitInterruptController();

	switch (demoType) {
	case MorsecodeDemo:
	  morsecodeQueue = xQueueCreate(10, sizeof(char));
	  xTaskCreate(senderTask, (signed char*)"Sender", 128, NULL, 0, NULL);
	  xTaskCreate(morsecodeTask, (signed char*)"MorseCode", 128, NULL, 0, NULL);
	  break;
	case SosDemo:
	  xTaskCreate(sos, (signed char*)"SOS", 128, NULL, 0, NULL);
	  break;
	case BlinkyDemo:
	  xTaskCreate(task1, (signed char*)"LED_0", 128, NULL, 0, NULL);
	  xTaskCreate(task2, (signed char*)"LED_1", 128, NULL, 0, NULL);
	  break;
	}

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
