#include "WbudyBUTTON.h"

#include <hardware/gpio.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

WbudyBUTTON::WbudyBUTTON(
	uint32_t pin,
	bool statePressed,
	uint32_t debounce,
	uint32_t longPress
) {
	init(pin, statePressed, debounce, longPress);
}

WbudyBUTTON::WbudyBUTTON() {}

void WbudyBUTTON::init(
	uint32_t pin,
	bool statePressed,
	uint32_t debounce,
	uint32_t longPress
) {
	this->pin = pin;
	this->statePressed = statePressed;
	this->debounce = debounce;
	this->longPress = longPress;

	gpio_set_dir(pin, GPIO_IN);
	gpio_put(pin, 0);
	gpio_set_function(pin, GPIO_FUNC_SIO);
	if (statePressed) {
		gpio_pull_down(pin);
	} else {
		gpio_pull_up(pin);
	}
	this->initDone = true;
	this->debounced = this->isPressed();
	char taskName[16];
	sprintf(taskName, "tButton_%d", pin);
	xTaskCreate(
		tButton,
		taskName,
		configMINIMAL_STACK_SIZE,
		this,
		tskIDLE_PRIORITY,
		NULL
	);
}

void WbudyBUTTON::setOnPressed(void (*cb)(WbudyBUTTON *btn)) {
	this->onPressed = cb;
}

void WbudyBUTTON::setOnLongPressed(void (*cb)(WbudyBUTTON *btn)) {
	this->onLongPressed = cb;
}

void WbudyBUTTON::setOnReleased(void (*cb)(WbudyBUTTON *btn)) {
	this->onReleased = cb;
}

void WbudyBUTTON::setOnChanged(void (*cb)(WbudyBUTTON *btn, bool pressed)) {
	this->onChanged = cb;
}

uint32_t WbudyBUTTON::msSinceChange() {
	return ticksSincePress * portTICK_PERIOD_MS;
}

bool WbudyBUTTON::isPressed() {
	if (this->initDone == false) {
		return false;
	}
	return gpio_get(pin) == statePressed;
}

uint32_t WbudyBUTTON::getPin() const { return this->pin; }

void WbudyBUTTON::tButton(void *pvParameters) {
	WbudyBUTTON *self = static_cast<WbudyBUTTON *>(pvParameters);
	bool lastStableState = self->isPressed();
	bool lastReadState = lastStableState;
	uint32_t lastChangeTime = xTaskGetTickCount();

	while (1) {
		bool currentState = self->isPressed();
		if (currentState != lastReadState) {
			lastReadState = currentState;
			lastChangeTime = xTaskGetTickCount();
		}

		self->ticksSincePress = (xTaskGetTickCount() - lastChangeTime);

		// If state is stable for debounce period
		if (self->msSinceChange() >= self->debounce) {
			if (lastStableState != currentState) {
				lastStableState = currentState;
				self->debounced = currentState;
				if (self->onChanged) {
					self->onChanged(self, currentState);
				}
				if (self->onPressed && currentState) {
					self->onPressed(self);
				}
				if (self->onReleased && !currentState) {
					self->onReleased(self);
				}
			}
		}
		if (currentState && // w tej chwili naciśnięty
			self->onLongPressed &&
			self->msSinceChange() >=
				self->longPress && // naciśnięty od longPress ms
			self->ticksSinceOnLongPress <
				lastChangeTime // nie wywołano jeszcze cb w tym naciśnięciu
		) {
			self->ticksSinceOnLongPress = lastChangeTime;
			self->onLongPressed(self);
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void WbudyBUTTON::clearCallbacks() {
	this->onPressed = nullptr;
	this->onReleased = nullptr;
	this->onLongPressed = nullptr;
	this->onChanged = nullptr;
}