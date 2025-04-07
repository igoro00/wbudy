#include "FatFS.h"
#include "FatFSUSB.h"

#include "main.hpp"
#include "sysutils.hpp"

volatile bool updated = false;
volatile bool driveConnected = false;
volatile bool inPrinting = false;

// Called by FatFSUSB when the drive is released.  We note this, restart FatFS,
// and tell the main loop to rescan.
void unplug(uint32_t i) {
	(void)i;
	driveConnected = false;
	updated = true;
	FatFS.begin();
}

// Called by FatFSUSB when the drive is mounted by the PC.  Have to stop FatFS,
// since the drive data can change, note it, and continue.
void plug(uint32_t i) {
	(void)i;
	driveConnected = true;
	FatFS.end();
}

// Called by FatFSUSB to determine if it is safe to let the PC mount the USB
// drive.  If we're accessing the FS in any way, have any Files open, etc. then
// it's not safe to let the PC mount the drive.
bool mountable(uint32_t i) {
	(void)i;
	return !inPrinting;
}

void initFS() {
	if (!FatFS.begin()) {
		Serial.println("FatFS initialization failed!");
		myPanic();
	}
	FatFSUSB.onUnplug(unplug);
	FatFSUSB.onPlug(plug);
	FatFSUSB.driveReady(mountable);
	// Start FatFS USB drive mode
	FatFSUSB.begin();
	if(!FatFS.exists("/games")) {
		FatFS.mkdir("/games");
	}
	if(!FatFS.exists("/players")) {
		FatFS.mkdir("/players");
	}
}