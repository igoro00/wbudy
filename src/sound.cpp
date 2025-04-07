#include "Arduino.h"
#include "NonBlockingRtttl.h"

#include "sound.hpp"


const char *SOUND_EFFECTS[] = {
	// special values interpreted in core1 loop
	(const char *)-1,
	(const char *)0,
	// RTTTL sound effects
    "setup done:d=4,o=6,b=400:a4,b4",
    "mariotheme:"
	"d=4,o=5,b=125:a,8f.,16c,16d,16f,16p,f,16d,16c,16p,16f,16p,16f,16p,8c6,8a.,"
	"g,16c,a,8f.,16c,16d,16f,16p,f,16d,16c,16p,16f,16p,16a#,16a,16g,2f,16p,8a.,"
	"8f.,8c,8a.,f,16g#,16f,16c,16p,8g#.,2g,8a.,8f.,8c,8a.,f,16g#,16f,8c,2c6,"
	"p,p,p,p",
	"portal2:"
	"d=4,o=4,b=180:a3,e4,a3,e4,a3,d#4,p,a3,d4,a3,d4,a3,f4,p",
	"ok:d=4,o=6,b=320:e,e",
	"press:d=4,o=6,b=400:c,g",
	"lost:d=4,o=3,b=120:d,c,g2",
	"win:d=4,o=5,b=320:c,c,c,g.",
};

void waitForSoundEffect() {
	// wait for sound effect to finish
	do{
		delay(1);
	}while (rtttl::isPlaying() || rp2040.fifo.available());
}