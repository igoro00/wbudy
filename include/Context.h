#pragma once

#include <cstdint>
#include <memory>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "Game.hpp"
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
	WbudyRGB rgb;
	WbudyLCD lcd;

	uint32_t cardUID;
	std::unique_ptr<Game> game;
	// MFRC522 *rfid;
	
	// Lazy loaded players list
	// Other parts of the program will first try to 
	std::unique_ptr<std::vector<Player>> players;
};

extern Context ctx;