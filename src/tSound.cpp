#include <hardware/pwm.h>
#include <pico/stdlib.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "Context.h"
#include "pindefs.h"
#include "tSound.h"

TaskHandle_t tPlayer = NULL;

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
	if (duration > 0) {
		vTaskDelay(duration / portTICK_PERIOD_MS);
	}
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

void tPlay_OK(void *pvParameters) {
	printf("[Sound] Playing OK sound\n");
	// 	"ok:d=4,o=6,b=320:e,e",
	uint32_t bpm = 320;			   // beats per minute, beat is 1/4 note
	uint32_t beat = (60000 / bpm); // ms per beat
	playNote(NOTE_E6, beat);
	playNote(0, beat / 8);
	playNote(NOTE_E6, beat);
	playNote(0, 0);
	tPlayer = NULL;
	vTaskDelete(NULL);
}

void tPlay_Mario(void *pvParameters) {
	// "mariotheme:"
	// "d=4,o=5,b=125:a,8f.,16c,16d,16f,16p,f,16d,16c,16p,16f,16p,16f,16p,8c6,8a.,"
	// "g,16c,a,8f.,16c,16d,16f,16p,f,16d,16c,16p,16f,16p,16a#,16a,16g,2f,16p,8a.,"
	// "8f.,8c,8a.,f,16g#,16f,16c,16p,8g#.,2g,8a.,8f.,8c,8a.,f,16g#,16f,8c,2c6,"
	// "p,p,p,p",
	uint32_t bpm = 500;			   // beats per minute, beat is 1/4 note
	uint32_t beat = (60000 / bpm); // ms per beat
	while (1) {
		playNote(NOTE_A5, beat * 4);
		playNote(NOTE_F5, beat * 3);
		playNote(NOTE_C5, beat);
		playNote(NOTE_D5, beat);
		playNote(NOTE_F5, beat);
		playNote(0, beat);
		playNote(NOTE_F5, beat * 4);
		playNote(NOTE_D5, beat);
		playNote(NOTE_C5, beat);
		playNote(0, beat);
		playNote(NOTE_F5, beat);
		playNote(0, beat);
		playNote(NOTE_F5, beat);
		playNote(0, beat);
		playNote(NOTE_C6, beat * 2);
		playNote(NOTE_A5, beat * 3);
		playNote(NOTE_G5, beat * 4);

		playNote(NOTE_C5, beat * 2);
		playNote(NOTE_A5, beat * 4);
		playNote(NOTE_F5, beat * 3);
		playNote(NOTE_C5, beat);
		playNote(NOTE_D5, beat);
		playNote(NOTE_F5, beat);
		playNote(0, beat);
		playNote(NOTE_F5, beat * 4);
		playNote(NOTE_D5, beat);
		playNote(NOTE_C5, beat);
		playNote(0, beat);
		playNote(NOTE_F5, beat);
		playNote(0, beat);
		playNote(NOTE_AS5, beat);
		playNote(NOTE_A5, beat);
		playNote(NOTE_G5, beat);
		playNote(NOTE_F5, beat * 8);
		playNote(0, beat * 4);

		playNote(NOTE_A5, beat * 3);
		playNote(NOTE_F5, beat * 3);
		playNote(NOTE_C5, beat * 2);
		playNote(NOTE_A5, beat * 3);
		playNote(NOTE_F5, beat * 4);
		playNote(NOTE_GS5, beat);
		playNote(NOTE_F5, beat);
		playNote(NOTE_C5, beat);
		playNote(0, beat);
		playNote(NOTE_GS5, beat * 3);
		playNote(NOTE_G5, beat * 8);
		playNote(NOTE_A5, beat * 3);
		playNote(NOTE_F5, beat * 3);
		playNote(NOTE_C5, beat * 2);
		playNote(NOTE_A5, beat * 3);
		playNote(NOTE_F5, beat * 4);
		playNote(NOTE_GS5, beat);
		playNote(NOTE_F5, beat);
		playNote(NOTE_C5, beat * 2);
		playNote(NOTE_C6, beat * 8);

		playNote(0, beat * 16);
	}
}

void tPlay_Press(void *pvParameters) {
	// "press:d=4,o=6,b=400:c,g",
	uint32_t bpm = 200;			   // beats per minute, beat is 1/4 note
	uint32_t beat = (60000 / bpm); // ms per beat
	playNote(NOTE_C6, beat / 2);
	playNote(NOTE_G6, beat / 2);
	playNote(0, 0);
	tPlayer = NULL;
	vTaskDelete(NULL);
}

void tPlay_Lost(void *pvParameters) {
	// "lost:d=4,o=3,b=120:d,c,g2",
	uint32_t bpm = 120;			   // beats per minute, beat is 1/4 note
	uint32_t beat = (60000 / bpm); // ms per beat
	playNote(NOTE_D3, beat / 4);
	playNote(NOTE_C3, beat / 4);
	playNote(NOTE_G2, beat / 4);
	playNote(0, 0);
	tPlayer = NULL;
	vTaskDelete(NULL);
}

void tPlay_Win(void *pvParameters) {
	// "win:d=4,o=5,b=320:c,c,c,g.",
	uint32_t bpm = 320;			   // beats per minute, beat is 1/4 note
	uint32_t beat = (60000 / bpm); // ms per beat
	playNote(NOTE_C5, beat / 4);
	playNote(NOTE_C5, beat / 4);
	playNote(NOTE_C5, beat / 4);
	playNote(NOTE_G5, beat * 3 / 4);
	playNote(0, 0);
	tPlayer = NULL;
	vTaskDelete(NULL);
}

void playSound(SoundEffect s) {
	printf("[Sound] Playing sound effect: %d\n", (int)s);
	if (tPlayer != NULL) {
		printf("[Sound] tPlayer is not null\n");
		if (eTaskGetState(tPlayer) != eDeleted) {
			printf("[Sound] Deleting previous task\n");
			vTaskDelete(tPlayer);
			playNote(0, 0);
			tPlayer = NULL;
		}
	}
	printf("[Sound] Deleted previous task succesfully\n");

	switch (s) {
	case SoundEffect::STOP:
		playNote(0, 0);
		break;
	case SoundEffect::MARIO_THEME:
		xTaskCreate(
			tPlay_Mario,
			"tPlay_Mario",
			configMINIMAL_STACK_SIZE,
			NULL,
			tskIDLE_PRIORITY + 5,
			&tPlayer
		);
		break;
	case SoundEffect::PORTAL2:
		xTaskCreate(
			tPlay_Portal2,
			"tPlay_Portal2",
			configMINIMAL_STACK_SIZE,
			NULL,
			tskIDLE_PRIORITY + 5,
			&tPlayer
		);
		break;
	case SoundEffect::OK:
		xTaskCreate(
			tPlay_OK,
			"tPlay_OK",
			configMINIMAL_STACK_SIZE,
			NULL,
			tskIDLE_PRIORITY + 5,
			&tPlayer
		);
		break;
	case SoundEffect::PRESS:
		xTaskCreate(
			tPlay_Press,
			"tPlay_Press",
			configMINIMAL_STACK_SIZE,
			NULL,
			tskIDLE_PRIORITY + 5,
			&tPlayer
		);
		break;
	case SoundEffect::LOST:
		xTaskCreate(
			tPlay_Lost,
			"tPlay_Lost",
			configMINIMAL_STACK_SIZE,
			NULL,
			tskIDLE_PRIORITY + 5,
			&tPlayer
		);
		break;
	case SoundEffect::WIN:
		xTaskCreate(
			tPlay_Win,
			"tPlay_Win",
			configMINIMAL_STACK_SIZE,
			NULL,
			tskIDLE_PRIORITY + 5,
			&tPlayer
		);
		break;
	}
}
