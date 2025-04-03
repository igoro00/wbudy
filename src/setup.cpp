#include "main.h"

#include "colors.h"
#include "chars.h"
#include "playerData.h"

Context ctx;

const char *demoText = "Hello World!";
void setup() {
  ctx.gameState = GameState::LOBBY;
  ctx.cardUID = 0;
  ctx.players[0] = 0;
  ctx.players[1] = 0;
  LiquidCrystal_I2C *lcd = new LiquidCrystal_I2C(0x27, 16, 2);
  ctx.lcd = lcd;
  MFRC522 *rfid = new MFRC522(RFID_CS, RFID_RST);
  ctx.rfid = rfid;
  Serial.begin(115200);
  SPI.setRX(RFID_MISO);
  SPI.setTX(RFID_MOSI);
  SPI.setSCK(RFID_SCK);
  SPI.setCS(RFID_CS);
  SPI.begin();
  Wire.begin();
  lcd->init();
  loadChars(lcd);
  lcd->backlight();
  
  rfid->PCD_Init();
  rfid->PCD_WriteRegister(rfid->ComIEnReg, 0xA0);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  digitalWrite(LED_BUILTIN, 1);
  analogWrite(LED_R, 255);
  analogWrite(LED_G, 255);
  analogWrite(LED_B, 255);

  pinMode(YELLOW_BTN, INPUT_PULLUP);
  pinMode(RED_BTN, INPUT_PULLUP);
  pinMode(GAME_RST_BTN, INPUT_PULLUP);
}
void loop() {
  if (ctx.gameState == GameState::LOBBY) {
    tLobby(ctx);
  } else if (ctx.gameState == GameState::GAME) {
    tGame(ctx);
  } else {
    u_int32_t color = HSL2RGB((millis()/4)%256, 0xff, 0x7f);
    byte r = color>>16;
    byte g = color>>8;
    byte b = color>>0;
  
    setLED(r, g, b);
  }
}

void setLEDByUID(u_int32_t uid){
  uint8_t hue = simpleHash(uid);

  u_int32_t color = HSL2RGB(hue, 0xff, 0x7f);
  byte r = color>>16;
  byte g = color>>8;
  byte b = color>>0;

  setLED(r, g, b);
}

void setLED(byte r, byte g, byte b){
  analogWrite(LED_R, 255-r);
  analogWrite(LED_G, 255-g);
  analogWrite(LED_B, 255-b);
}

void tLCDUpdate (Context &ctx) {
  LiquidCrystal_I2C lcd = *ctx.lcd;
  while(1){
    if(!ctx.cardUID){
      lcd.clear();
      lcd.setCursor(0, 0);
      printPolishMsg(lcd, L"Jebać czarnuchów");
      lcd.setCursor(0, 1);
      lcd.print("Waiting for card");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      int index = 0;
      for(byte i = 0; i < 4; i++) {
        if(uids[i] == ctx.cardUID) {
          index = i;
          break;
        }
      }
      printPolishMsg(lcd, names[index]);
      lcd.setCursor(0, 1);
      lcd.print("UID: ");
      lcd.print(ctx.cardUID, HEX);
    }
    delay(100);
  }
}