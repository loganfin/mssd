#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <queue.h>
#include <semphr.h>
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

void vCounterTask();
void vBlinkTask();
void vSemGiverTask();
void vLeftDisplayTask();
void vRightDisplayTask();
void vSevSegDisplay(uint16_t display, uint16_t number);

QueueHandle_t qLeftDisplay;
QueueHandle_t qRightDisplay;
SemaphoreHandle_t xSemaphore;

int main()
{
    stdio_init_all();

    /* initialize the required pins */
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

    /* set the gpio direction of the pins */
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

    /* create a queue to communicate between vCounterTask and vBlinkTask */
    qLeftDisplay= xQueueCreate(1, sizeof(uint16_t));
    qRightDisplay= xQueueCreate(1, sizeof(uint16_t));
    xSemaphore = xSemaphoreCreateBinary();

    if (qLeftDisplay != NULL && qRightDisplay && xSemaphore != NULL) {
        xTaskCreate(vCounterTask, "CounterTask", 256, NULL, 1, NULL);
        xTaskCreate(vBlinkTask, "BlinkTask", 256, NULL, 1, NULL);
        xTaskCreate(vSemGiverTask, "SemGiverTask", 256, NULL, 2, NULL);
        xTaskCreate(vLeftDisplayTask, "LeftDisplayTask", 256, NULL, 1, NULL);
        xTaskCreate(vRightDisplayTask, "RightDisplayTask", 256, NULL, 1, NULL);
        vTaskStartScheduler();
    }

    /* the queue or semaphore could not be created */

    while(true);
}

void vCounterTask()
{
    uint16_t counter = 0;
    uint16_t addend = 1;
    BaseType_t xStatus;


    while (true) {
        if (counter == 0) {
            addend = 1;
        }
        else if (counter == 42) {
            addend = -1;
        }
        xStatus = xQueueSendToBack(qLeftDisplay, &counter, 0);
        xStatus = xQueueSendToBack(qRightDisplay, &counter, 0);
        counter += addend;
        vTaskDelay(0.5 * configTICK_RATE_HZ);
    }
}

void vBlinkTask()
{
    uint16_t receivedCounter = 0;
    BaseType_t xStatus;

    while(true) {
        xStatus = xQueuePeek(qLeftDisplay, &receivedCounter, 0);
        if (xStatus == pdPASS) {
            gpio_put(LED_PIN, LED_ON);
            vTaskDelay(0.25 * configTICK_RATE_HZ);
            gpio_put(LED_PIN, LED_OFF);
        }
        taskYIELD();
    }
}

void vSemGiverTask()
{
    while (true) {
        xSemaphoreGive(xSemaphore);
        vTaskDelay(1);
    }
}

void vLeftDisplayTask()
{
    uint16_t receivedCounter = 0;

    while (true) {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            xQueueReceive(qLeftDisplay, &receivedCounter, 0);
            vSevSegDisplay(1, receivedCounter / 10);
        }
    }
}

void vRightDisplayTask()
{
    uint16_t receivedCounter = 0;

    while (true) {
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            xQueueReceive(qRightDisplay, &receivedCounter, 0);
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
