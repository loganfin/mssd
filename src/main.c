/* REQUIREMENTS:
 *     1. Create a task that counts down from 42 to 00 and back up to 42 at a rate of 1 count per 1/2 second
 *     2. Create a task that blinks the LED at pin 13 on each count
 *     3. Create a task that outputs the number in the 10's place to the left 7-segment display
 *     4. Create a task that outputs the number in the 1's place to the right 7-segment display
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
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

void vSevSegDisplay(uint16_t display, uint16_t number, TickType_t delay);

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

    //vTaskStartScheduler();

    while(true) {
        gpio_put(LED_PIN, LED_ON);
        for (uint16_t i = 0; i < 11; i++) {
            vSevSegDisplay(1, i, 1000);
        }
        gpio_put(LED_PIN, LED_OFF);
        for (uint16_t i = 0; i < 11; i++) {
            vSevSegDisplay(2, i, 1000);
        }
    }
}

void vSevSegDisplay(uint16_t display, uint16_t number, TickType_t delay)
{
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

    //vTaskDelay(delay);
    sleep_ms(delay);

    gpio_put(SEV_SEG_A, LED_OFF);
    gpio_put(SEV_SEG_B, LED_OFF);
    gpio_put(SEV_SEG_C, LED_OFF);
    gpio_put(SEV_SEG_D, LED_OFF);
    gpio_put(SEV_SEG_E, LED_OFF);
    gpio_put(SEV_SEG_F, LED_OFF);
    gpio_put(SEV_SEG_G, LED_OFF);
    gpio_put(SEV_SEG_DP, LED_OFF);
}
