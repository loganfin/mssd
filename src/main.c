#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();

    vTaskStartScheduler();

    while(1);
}
