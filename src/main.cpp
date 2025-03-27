#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include "colors.h"

#define YELLOW_BTN 15
#define RED_BTN 22
#define LED_R 21
#define LED_G 20
#define LED_B 10
#define RFID_RST 6
#define RFID_CS 17
#define RFID_IRQ 9

#define PEOPLES 4

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(RFID_CS, RFID_RST);
u_int32_t cardUID = 0;


u_int32_t uids[PEOPLES] = {
  0,
  0xEAAA764F,
  0x3A4AE9CE,
  0x5A7EFF4B
};

String names[PEOPLES] = {
  "Nie znaleziono",
  "Igor Ordecha",
  "Adrian Urbanczyk",
  "Jakub Bukowski"
};

volatile bool yellowBtn = false;
volatile bool redBtn = false;
volatile bool redLED = false;

void handleYellow() {
  yellowBtn = !digitalRead(YELLOW_BTN);
  // digitalWrite(LED_B, yellowBtn);
}
void handleRed() {
  redBtn = !digitalRead(RED_BTN);
}

void updateScreen() {
  if(!cardUID){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID Reader");
    lcd.setCursor(0, 1);
    lcd.print("Waiting for card");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    int index = 0;
    for(byte i = 0; i < PEOPLES; i++) {
      if(uids[i] == cardUID) {
        index = i;
        break;
      }
    }
    lcd.print(names[index]);
    lcd.setCursor(0, 1);
    lcd.print("UID: ");
    lcd.print(cardUID, HEX);
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  Wire.begin();
  lcd.init();
  lcd.backlight();

  rfid.PCD_Init();
  rfid.PCD_WriteRegister(rfid.ComIEnReg, 0xA0);
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  analogWrite(LED_R, 255);
  analogWrite(LED_G, 255);
  analogWrite(LED_B, 255);

  pinMode(YELLOW_BTN, INPUT_PULLUP);
  pinMode(RED_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(YELLOW_BTN), handleYellow, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RED_BTN), handleRed, CHANGE);
  updateScreen();
}

void loop() {
  delay(10);
  uint8_t hue = simpleHash(cardUID);

  u_int32_t color = HSL2RGB(hue, 0xff, 0x7f);
  byte r = color>>16;
  byte g = color>>8;
  byte b = color>>0;

  analogWrite(LED_R, 255-r);
  analogWrite(LED_G, 255-g);
  analogWrite(LED_B, 255-b);

  if ( ! rfid.PICC_IsNewCardPresent()){
    return;
  }

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial()) {
    return;
  }
  redLED = !redLED;

  // MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
  //   piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
  //   piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
  //   return;
  // }

  for (byte i = 0; i < 4; i++) {
    cardUID = (cardUID << 8) + rfid.uid.uidByte[i];
  }
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  updateScreen();
}