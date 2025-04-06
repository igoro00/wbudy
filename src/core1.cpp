#include <Arduino.h>
#include <NonBlockingRtttl.h>
#include "pindefs.h"
// second core only for sound
const char *lastSound = nullptr;
bool repeatSound = false;

void setup1() {
	pinMode(SPEAKER, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(SPEAKER, 0);
}
void loop1() {
	if (rp2040.fifo.available()) {
		uint32_t value = rp2040.fifo.pop();
		// if -1 then play the last sound effect in a loop
		if (value == 0) {            
            rtttl::stop();
            repeatSound = false;
            lastSound = nullptr;
        } else if (value == -1) {
            if (lastSound != nullptr) {
                repeatSound = true;
            }
        } else {
            lastSound = (const char *)value;
            repeatSound = false;
            // pop returns uint32_t
            // but since were using 32bit cpu
            // we can cast it to pointer
            const char *soundEffect = (const char *)value;
            // the c string should contain the RTTTL song
            // it will overwrite the currently playing song
            rtttl::begin(SPEAKER, soundEffect);
        }
	}
	if (rtttl::isPlaying()) {
		rtttl::play(); // next tick
	} else if (repeatSound) {
        rtttl::begin(SPEAKER, lastSound);
    }
}