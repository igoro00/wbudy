#include <hardware/pwm.h>
#include <pico/stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include "pindefs.hpp"
#include "tSound.h"

void initSound() {
	gpio_set_function(SPEAKER, GPIO_FUNC_PWM);
	pwm_set_clkdiv(pwm_gpio_to_slice_num(SPEAKER), 125);
	pwm_set_wrap(pwm_gpio_to_slice_num(SPEAKER), 1000);
	pwm_set_chan_level(
		pwm_gpio_to_slice_num(SPEAKER),
		pwm_gpio_to_channel(SPEAKER),
		0
	);
	pwm_set_enabled(pwm_gpio_to_slice_num(SPEAKER), true);
}

void setSpeakerFreq(uint16_t freq) {
	if (freq == 0) {
		pwm_set_chan_level(
			pwm_gpio_to_slice_num(SPEAKER),
			pwm_gpio_to_channel(SPEAKER),
			0
		);
		return;
	}
	pwm_set_wrap(pwm_gpio_to_slice_num(SPEAKER), 1000000 / freq);
	pwm_set_chan_level(
		pwm_gpio_to_slice_num(SPEAKER),
		pwm_gpio_to_channel(SPEAKER),
		1000000 / freq / 2
	);
}

void playNote(uint16_t freq, uint32_t duration) {
	setSpeakerFreq(freq);
	vTaskDelay(duration / portTICK_PERIOD_MS);
}

void tPlay_Portal2(void *pvParameters) {
	//"d=4,o=4,b=180:a3,e4,a3,e4,a3,d#4,p,a3,d4,a3,d4,a3,f4,p",
	uint32_t bpm = 180; // beats per minute, beat is 1/4 note

	uint32_t beat = (60000 / bpm); // ms per beat
	while (1) {
		playNote(NOTE_A3, beat);
		playNote(NOTE_E4, beat);
		playNote(NOTE_A3, beat);
		playNote(NOTE_E4, beat);
		playNote(NOTE_A3, beat);
		playNote(NOTE_DS4, beat);
		playNote(0, beat);
		playNote(NOTE_A3, beat);
		playNote(NOTE_D4, beat);
		playNote(NOTE_A3, beat);
		playNote(NOTE_D4, beat);
		playNote(NOTE_A3, beat);
		playNote(NOTE_F4, beat);
		playNote(0, beat);
	}
}