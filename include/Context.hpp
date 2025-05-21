#pragma once

#include <cstdint>
#include <memory>

#include "Game.hpp"

struct Context {
	uint32_t cardUID;
	std::unique_ptr<Game> game;
	// LiquidCrystal_I2C *lcd;
	// MFRC522 *rfid;
	
	// Lazy loaded players list
	// Other parts of the program will first try to 
	std::unique_ptr<std::vector<Player>> players;
};

extern Context ctx;