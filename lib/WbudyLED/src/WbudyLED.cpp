#include "WbudyLED.h"
#include <hardware/clocks.h>
#include <hardware/pwm.h>
#include <pico/stdlib.h>
#include <stdio.h>

WbudyLED::WbudyLED(uint32_t pin, bool inverted = false) { init(pin, inverted); }

WbudyLED::WbudyLED() {}

void WbudyLED::init(uint32_t pin, bool inverted = false) {
	this->inverted = inverted;
	uint32_t freq =
		1000; // 1kHz period (goes from 0 to 255, 1000 times a second)
	uint32_t f_sys = clock_get_hz(clk_sys); // typically 125'000'000
	uint8_t wrap = UINT8_MAX;

	slice_num = pwm_gpio_to_slice_num(pin);
	channel_num = pwm_gpio_to_channel(pin);
	gpio_set_function(pin, GPIO_FUNC_PWM);
	pwm_set_wrap(slice_num, wrap);
	pwm_set_clkdiv(slice_num, (float)f_sys / (freq * (wrap + 1)));
	pwm_set_phase_correct(slice_num, false);

	pwm_set_enabled(slice_num, true);
	set(0);
}

void WbudyLED::set(uint8_t value) {
	uint8_t val;
	if (inverted)
		val = UINT8_MAX - value;
	else
		val = value;
	pwm_set_chan_level(slice_num, channel_num, val);
}

void WbudyLED::operator=(uint8_t value) { set(value); }
