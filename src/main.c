/* REQUIREMENTS:
 *     1. Create a task that counts down from 42 to 00 and back up to 42 at a rate of 1 count per 1/2 second
 *     2. Create a task that blinks the LED at pin 13 on each count
 *     3. Create a task that outputs the number in the 10's place to the left 7-segment display
 *     4. Create a task that outputs the number in the 1's place to the right 7-segment display
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <queue.h>
#include "pico/stdlib.h"

#define SEV_SEG_CC1 11    // common cathode corresponding to the left digit (active low)
#define SEV_SEG_CC2 10    // common cathode corresponding to the right digit (active low)

#define SEV_SEG_CC_ON  0
#define SEV_SEG_CC_OFF 1

#define SEV_SEG_A 26      // controls the top horizontal bar LED (active high)
#define SEV_SEG_B 27      // controls the top-right vertical bar LED
#define SEV_SEG_C 29      // controls the bottom-right vertical bar LED
#define SEV_SEG_D 18      // controls the bottom horizontal LED
#define SEV_SEG_E 25      // controls the bottom-left vertical bar LED
#define SEV_SEG_F 7       // controls the top-right vertical bar LED
#define SEV_SEG_G 28      // controls the middle horizontal bar LED
#define SEV_SEG_DP 24     // controls the period LED

#define LED_PIN 13

#define LED_ON 1
#define LED_OFF 0

//void vCounterTask(TickType_t xDelay);
void vCounterTask();
void vBlinkTask();
void vRightDisplayTask();
void vSevSegDisplay(uint16_t display, uint16_t number);

QueueHandle_t xQueue;

int main()
{
    stdio_init_all();

    // initialize the required pins
    gpio_init(SEV_SEG_CC1);
    gpio_init(SEV_SEG_CC2);
    gpio_init(SEV_SEG_A);
    gpio_init(SEV_SEG_B);
    gpio_init(SEV_SEG_C);
    gpio_init(SEV_SEG_D);
    gpio_init(SEV_SEG_E);
    gpio_init(SEV_SEG_F);
    gpio_init(SEV_SEG_G);
    gpio_init(SEV_SEG_DP);
    gpio_init(LED_PIN);

    // set the gpio direction of the required pins
    gpio_set_dir(SEV_SEG_CC1, GPIO_OUT);
    gpio_set_dir(SEV_SEG_CC2, GPIO_OUT);
    gpio_set_dir(SEV_SEG_A, GPIO_OUT);
    gpio_set_dir(SEV_SEG_B, GPIO_OUT);
    gpio_set_dir(SEV_SEG_C, GPIO_OUT);
    gpio_set_dir(SEV_SEG_D, GPIO_OUT);
    gpio_set_dir(SEV_SEG_E, GPIO_OUT);
    gpio_set_dir(SEV_SEG_F, GPIO_OUT);
    gpio_set_dir(SEV_SEG_G, GPIO_OUT);
    gpio_set_dir(SEV_SEG_DP, GPIO_OUT);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // create a queue to communicate between vCounterTask and vBlinkTask
    xQueue = xQueueCreate(5, sizeof(uint16_t));
    if (xQueue != NULL) {
        xTaskCreate(vCounterTask, "CounterTask", 256, NULL, 1, NULL);
        //xTaskCreate(vBlinkTask, "BlinkTask", 256, NULL, 2, NULL);
        xTaskCreate(vRightDisplayTask, "RightDisplayTask", 256, NULL, 2, NULL);
        vTaskStartScheduler();
    }
    else {
        /* the queue could not be created */
    }

    while(true);
}

void vCounterTask()
{
    uint16_t counter = 0;
    uint16_t operand = 1;
    BaseType_t xStatus;

    while (true) {
        if (counter == 0) {
            operand = 1;
        }
        else if (counter == 42) {
            operand = -1;
        }
        for (int i = 0; i < 42; i++) {
            printf("counter: %d\n", counter);
            xStatus = xQueueSendToBack(xQueue, &counter, 0);
            counter += operand;
            vTaskDelay(0.5 * configTICK_RATE_HZ);
        }
    }
}

void vBlinkTask()
{
    uint16_t receivedCounter = 0;
    BaseType_t xStatus;

    while(true) {
        xStatus = xQueueReceive(xQueue, &receivedCounter, 100);
        if (xStatus == pdPASS) {
            gpio_put(LED_PIN, LED_ON);
            vTaskDelay(10);
            gpio_put(LED_PIN, LED_OFF);
        }
    }
}

void vRightDisplayTask()
{
    uint16_t receivedCounter = 0;
    BaseType_t xStatus;

    while (true) {
        xStatus = xQueueReceive(xQueue, &receivedCounter, 100);
        if (xStatus == pdPASS) {
            vSevSegDisplay(2, receivedCounter % 10);
        }
    }
}

void vSevSegDisplay(uint16_t display, uint16_t number)
{
    gpio_put(SEV_SEG_A, LED_OFF);
    gpio_put(SEV_SEG_B, LED_OFF);
    gpio_put(SEV_SEG_C, LED_OFF);
    gpio_put(SEV_SEG_D, LED_OFF);
    gpio_put(SEV_SEG_E, LED_OFF);
    gpio_put(SEV_SEG_F, LED_OFF);
    gpio_put(SEV_SEG_G, LED_OFF);
    gpio_put(SEV_SEG_DP, LED_OFF);

    switch (display) {
        case 1:
            gpio_put(SEV_SEG_CC2, SEV_SEG_CC_OFF);
            gpio_put(SEV_SEG_CC1, SEV_SEG_CC_ON);
            break;
        case 2:
            gpio_put(SEV_SEG_CC1, SEV_SEG_CC_OFF);
            gpio_put(SEV_SEG_CC2, SEV_SEG_CC_ON);
            break;
        default:
            gpio_put(SEV_SEG_CC1, SEV_SEG_CC_OFF);
            gpio_put(SEV_SEG_CC2, SEV_SEG_CC_OFF);
            break;
    }

    switch (number) {
        case 0:
            gpio_put(SEV_SEG_A, LED_ON);
            gpio_put(SEV_SEG_B, LED_ON);
            gpio_put(SEV_SEG_C, LED_ON);
            gpio_put(SEV_SEG_D, LED_ON);
            gpio_put(SEV_SEG_E, LED_ON);
            gpio_put(SEV_SEG_F, LED_ON);
            break;
        case 1:
            gpio_put(SEV_SEG_B, LED_ON);
            gpio_put(SEV_SEG_C, LED_ON);
            break;
        case 2:
            gpio_put(SEV_SEG_A, LED_ON); //  -
            gpio_put(SEV_SEG_B, LED_ON); //   |
            gpio_put(SEV_SEG_G, LED_ON); //  -
            gpio_put(SEV_SEG_E, LED_ON); // |
            gpio_put(SEV_SEG_D, LED_ON); //  -
            break;
        case 3:
            gpio_put(SEV_SEG_A, LED_ON); //  -
            gpio_put(SEV_SEG_B, LED_ON); //   |
            gpio_put(SEV_SEG_G, LED_ON); //  -
            gpio_put(SEV_SEG_C, LED_ON); //   |
            gpio_put(SEV_SEG_D, LED_ON); //  -
            break;
        case 4:
            gpio_put(SEV_SEG_F, LED_ON);
            gpio_put(SEV_SEG_G, LED_ON);
            gpio_put(SEV_SEG_B, LED_ON);
            gpio_put(SEV_SEG_C, LED_ON);
            break;
        case 5:
            gpio_put(SEV_SEG_A, LED_ON); //  -
            gpio_put(SEV_SEG_F, LED_ON); // |
            gpio_put(SEV_SEG_G, LED_ON); //  -
            gpio_put(SEV_SEG_C, LED_ON); //   |
            gpio_put(SEV_SEG_D, LED_ON); //  -
            break;
        case 6:
            gpio_put(SEV_SEG_A, LED_ON);
            gpio_put(SEV_SEG_F, LED_ON);
            gpio_put(SEV_SEG_E, LED_ON);
            gpio_put(SEV_SEG_D, LED_ON);
            gpio_put(SEV_SEG_C, LED_ON);
            gpio_put(SEV_SEG_G, LED_ON);
            break;
        case 7:
            gpio_put(SEV_SEG_F, LED_ON);
            gpio_put(SEV_SEG_A, LED_ON);
            gpio_put(SEV_SEG_B, LED_ON);
            gpio_put(SEV_SEG_C, LED_ON);
            break;
        case 8:
            gpio_put(SEV_SEG_A, LED_ON);
            gpio_put(SEV_SEG_B, LED_ON);
            gpio_put(SEV_SEG_C, LED_ON);
            gpio_put(SEV_SEG_D, LED_ON);
            gpio_put(SEV_SEG_E, LED_ON);
            gpio_put(SEV_SEG_F, LED_ON);
            gpio_put(SEV_SEG_G, LED_ON);
            break;
        case 9:
            gpio_put(SEV_SEG_A, LED_ON);
            gpio_put(SEV_SEG_F, LED_ON);
            gpio_put(SEV_SEG_G, LED_ON);
            gpio_put(SEV_SEG_B, LED_ON);
            gpio_put(SEV_SEG_C, LED_ON);
            break;
        default:
            gpio_put(SEV_SEG_DP, LED_ON);
            break;
    }
}
