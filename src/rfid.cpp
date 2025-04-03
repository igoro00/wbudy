#include "main.h"

// Task który czyta UID z karty
// Jak znajdzie to zapisuje do ctx.cardUID oraz kończy się
uint32_t tRfidRead(Context &ctx){
    uint32_t uid = 0;
    bool result = ctx.rfid->PICC_IsNewCardPresent();
    if (!result){
        return 0;
    }

    // Verify if the NUID has been readed
    if (!ctx.rfid->PICC_ReadCardSerial()){
        return 0;
    }

    for (byte i = 0; i < 4; i++){
        uid = (uid << 8) + ctx.rfid->uid.uidByte[i];
    }
    ctx.rfid->PICC_HaltA();

    // Stop encryption on PCD
    ctx.rfid->PCD_StopCrypto1();
    return uid;
}