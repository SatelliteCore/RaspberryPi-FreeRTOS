#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"

#define UNUSED(v) (void)(v)

void dot(void) {
  SetGpio(16, 0);
  vTaskDelay(200);
  SetGpio(16, 1);
  vTaskDelay(200);
}

void dash(void) {
  SetGpio(16, 0);
  vTaskDelay(800);
  SetGpio(16, 1);
  vTaskDelay(200);
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
		SetGpio(16, 1);
		vTaskDelay(200);
	}
}

void task2(void *pParam) {
  UNUSED(pParam);

	int i = 0;
	while(1) {
		i++;
		vTaskDelay(100);
		SetGpio(16, 0);
		vTaskDelay(100);
	}
}


/**
 *	This is the systems main entry, some call it a boot thread.
 *
 *	-- Absolutely nothing wrong with this being called main(), just it doesn't have
 *	-- the same prototype as you'd see in a linux program.
 **/
int main(void) {

	DisableInterrupts();
	InitInterruptController();

	SetGpioFunction(16, 1);			// RDY led

	xTaskCreate(sos, (signed char*)"SOS", 128, NULL, 0, NULL);
	//xTaskCreate(task1, (signed char*)"LED_0", 128, NULL, 0, NULL);
	//xTaskCreate(task2, (signed char*)"LED_1", 128, NULL, 0, NULL);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
