#pragma once

#include <cstdint>
#include <memory>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "Game.h"
#include "pindefs.h"
#include "WbudyBUTTON.h"
#include "WbudyRGB.h"
#include "WbudyLCD.h"
#include "WbudyRFID.h"

enum class GameState {
	MAIN,
	LOBBY,
	GAME,
	END
};
struct Context {
	SemaphoreHandle_t taskMutex;
	TaskHandle_t currentTask;
	GameState gameState;

	WbudyBUTTON redButton;
	WbudyBUTTON yellowButton;
	WbudyBUTTON resetButton;
	WbudyRGB rgb;
	WbudyLCD lcd;
	WbudyRFID rfid;
	uint16_t fotoValue;
	
	NVMem nvmem;
};

uint8_t FotoToL(uint16_t fotoValue);

extern Context ctx;