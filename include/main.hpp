#pragma once

#include <cstdint>
// #include <string>
// #include <vector>
// #include <format>
#include <memory>

// #include <Arduino.h>
// #include <FatFS.h>
// #include <FatFSUSB.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
// #include <NonBlockingRtttl.h>
// #include <SPI.h>
// #include <WiFi.h>

// #include "pindefs.hpp"
#include "Player.hpp"
#include "Game.hpp"
// #include "sound.hpp"
// #include "chars.hpp"

enum GameState { LOBBY, GAME, END };

struct Context {
	GameState gameState;
	uint32_t cardUID;
	std::unique_ptr<Game> game;
	LiquidCrystal_I2C *lcd;
	MFRC522 *rfid;
	
	// Lazy loaded players list
	// Other parts of the program will first try to 
	std::unique_ptr<std::vector<Player>> players;
};

extern Context ctx;

uint32_t tRfidRead();
void tLobby();
void tGame();

// blocking delay function
// runs bookkeeping() in a loop
void wait(uint32_t ms);