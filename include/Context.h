#pragma once

#include <cstdint>
#include <memory>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "Game.h"
#include "WbudyBUTTON.h"
#include "WbudyRGB.h"
#include "WbudyLCD.h"

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

	uint32_t cardUID;
	
	NVMem nvmem;
};

extern Context ctx;