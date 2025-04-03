#pragma once

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <cstdint>
#include <string>
#include <SPI.h>
#include <FatFS.h>
#include <FatFSUSB.h>

#include "pindefs.h"

enum GameState {
  LOBBY,
  GAME,
  END
};

struct Context {
  GameState gameState;
  uint32_t cardUID;
  uint32_t players[2];
  LiquidCrystal_I2C *lcd;
  MFRC522 *rfid;
};

uint32_t tRfidRead(Context &ctx);
void tLobby(Context &ctx);
void tGame(Context &ctx);
void setLED(byte r, byte g, byte b);
void setLEDByUID(u_int32_t uid);