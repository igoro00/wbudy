#include "Arduino.h"

#include "webserver.hpp"

void myPanic() {
	// disable_interrupts();
	pinMode(LED_BUILTIN, OUTPUT);
	while (1) {
		digitalWrite(LED_BUILTIN, LOW);
		delay(100);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(100);
		digitalWrite(LED_BUILTIN, LOW);
		delay(100);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(500);
	}
}

// normally it would be ran in the main loop
// but since we have multiple loops
// we need to run it in each of them
void bookkeeping() {
	handleClient();
}

void wait(uint32_t ms) {
	uint32_t start = millis();
	while (ms < millis() - start) {
		bookkeeping();
		delay(1);
	}
}