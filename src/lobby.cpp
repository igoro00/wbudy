#include "main.h"

void updateLobbyLCD(Context &ctx){
    LiquidCrystal_I2C *lcd = ctx.lcd;
    lcd->clear();
    lcd->setCursor(0, 0);
    if (ctx.players[0] == 0) {
        lcd->print("Player 1");
    } else {
        lcd->print(ctx.players[0], HEX);
    }
    lcd->setCursor(0, 1);
    if (ctx.players[1] == 0) {
        lcd->print("Player 2");
    } else {
        lcd->print(ctx.players[1], HEX);
    }
}

void tLobby(Context &ctx) {
    updateLobbyLCD(ctx);
    uint32_t lastRST = 0;
    while(1){
        Serial.println("Lobby loop");
        bool yellowBtn = !digitalRead(YELLOW_BTN);
        bool redBtn = !digitalRead(RED_BTN);
        bool rstBtn = !digitalRead(GAME_RST_BTN);
        if (rstBtn && ctx.players[0] && ctx.players[1]) {
            if(lastRST == 0) {
                lastRST = millis();
            } else if (millis() - lastRST > 3000) {
                ctx.gameState = GameState::GAME;
                Serial.println("Game started");
                setLED(0, 0, 0);
                for (int i = 0; i < 5; i++) {
                    setLED(255, 0, 0);
                    delay(50);
                    setLED(0, 0, 0);
                    delay(50);
                }
                break;
            } else {
                byte r = map(millis() - lastRST, 0, 3000, 0, 200);
                setLED(r, 0, 0);
            }
        } else {
            lastRST = 0;
            setLED(0, 0, 0);
        }

        if (yellowBtn && redBtn) {
            Serial.println("Both buttons pressed");
            // not allowed
            continue;
        }

        // cancel RFID task if both buttons are not pressed
        if (!yellowBtn && !redBtn) {
            Serial.println("No buttons pressed");
            continue;
        }
        Serial.println("One button pressed");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

        // At this point exactly one button is pressed

        uint32_t uid = tRfidRead(ctx);
        if (uid) {
            if(redBtn) {
                ctx.players[0] = uid;
            } else {
                ctx.players[1] = uid;
            }
            updateLobbyLCD(ctx);
            delay(100);
        }
    }

}