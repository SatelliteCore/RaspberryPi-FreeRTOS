#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"
#include "Drivers/led.h"

enum DemoType {
    MorsecodeDemo,
    SosDemo,
    BlinkyDemo,
    ReadWriteDemo
};

// Pick demo type to run
//enum DemoType demoType = MorsecodeDemo;
enum DemoType demoType = ReadWriteDemo;
const int kPinWrite = 16;
const int kPinRead = 20;

#define UNUSED(v) (void)(v)


void dot(void) {
    LedOn();
    vTaskDelay(200);
    LedOff();
    vTaskDelay(200);
}

void dash(void) {
    LedOn();
    vTaskDelay(800);
    LedOff();
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
        LedOff();
        vTaskDelay(200);
    }
}

void task2(void *pParam) {
    UNUSED(pParam);

    int i = 0;
    while(1) {
        i++;
        vTaskDelay(100);
        LedOn();
        vTaskDelay(100);
    }
}

void write(void *pParam) {
    UNUSED(pParam);
    SetGpioFunction(kPinWrite, GPIO_OUT); //Init GPIO output (for specific pin)

    while(1) {
        SetGpio(kPinWrite, 1);
        vTaskDelay(1000);
        SetGpio(kPinWrite, 0);
        vTaskDelay(1000);
    }
}

void read(void *pParam) {
    UNUSED(pParam);
    SetGpioFunction(kPinRead, GPIO_IN);
    int readValue = 0;
    
    while(1) {
        readValue = ReadGpio(kPinRead);
        if(readValue != 0) {
            LedOn();
        }
        else {
            LedOff();
        }
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
    DisableInterrupts();
    InitInterruptController();
    LedInit(); //Init activity LED

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
        case ReadWriteDemo:
            xTaskCreate(write, (signed char*)"Write", 128, NULL, 0, NULL);
            xTaskCreate(read,  (signed char*)"Read",  128, NULL, 0, NULL);
	    break;
    }

    vTaskStartScheduler();

    /*
     *	We should never get here, but just in case something goes wrong,
     *	we'll place the CPU into a safe loop.
     */
    while(2) {
        ;
    }
}
