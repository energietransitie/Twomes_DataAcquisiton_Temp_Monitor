#ifndef _SENSOR_IO_H
#define _SENSOR_IO_H

#define BUTTON_P1 0         //Button P1 on GPIO0
#define BUTTON_P2 15        //Button P2 on GPIO15
#define LED_ERROR 14        //Error LED on GPIO 14
#define LED_STATUS 12       //Status LED on GPIO12

#include <stdlib.h>
#include <FreeRTOS.h>

/**
 * @brief Blink LEDs
 * Pass two arguments in uint8_t array:
 * @param argument[0]  amount of blinks
 * @param argument[1]  pin to blink on (LED_STATUS or LED_ERROR)
 */
void blink(void *args) {
    uint8_t *arguments = (uint8_t *)args;
    uint8_t amount = arguments[0];
    uint8_t pin = arguments[1];
    uint8_t i;
    for (i = 0; i < amount; i++) {
        gpio_set_level((gpio_num_t)pin, 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        gpio_set_level((gpio_num_t)pin, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    } //for(i<amount)
    //Delete the blink task after completion:
    vTaskDelete(NULL);
} //void blink;

#endif//_SENSOR_IO_H