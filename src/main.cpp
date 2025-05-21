/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fade an LED between low and high brightness. An interrupt handler updates
// the PWM slice's output level each time the counter wraps.

#include "pico/stdlib.h"
#include "WbudyLED.h"

int main() {
	stdio_init_all();
    
	WbudyLED led(1);

	uint8_t i = 0;
	while(1){
		led=128;
	}
    // while (1) {
	// 	i++;
	// 	led = 255/2;
	// 	printf("i: %d\n", i);
	// 	sleep_ms(10);
	// }
	// pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, 0);
	// pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, 255);
	// sleep_ms(1000);
}