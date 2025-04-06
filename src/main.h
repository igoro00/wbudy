#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <format>
#include <memory>

#include <Arduino.h>
#include <FatFS.h>
#include <FatFSUSB.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <NonBlockingRtttl.h>
#include <SPI.h>
#include <VFS.h>
#include <WiFi.h>

#include "pindefs.h"
#include "playerData.h"
#include "sound.h"
#include "chars.h"

enum GameState { LOBBY, GAME, END };

struct Context {
	GameState gameState;
	uint32_t cardUID;
	std::unique_ptr<Game> game;
	LiquidCrystal_I2C *lcd;
	MFRC522 *rfid;
};

extern Context ctx;

uint32_t tRfidRead();
void tLobby();
void tGame();
void setLED(byte r, byte g, byte b);
void setLEDByUID(uint32_t uid);
void initFS();
void myPanic();
void bookkeeping();
void initWebserver();
void handleClient();
void wait(uint32_t ms);