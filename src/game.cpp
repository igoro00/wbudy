#include "main.h"

void tGame(Context &ctx) {
    LiquidCrystal_I2C *lcd = ctx.lcd;
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("Game is starting in...");
    lcd->setCursor(0, 1);
    lcd->print("3 ");
    delay(1000);
    lcd->print("2 ");
    delay(1000);
    lcd->print("1 ");
    delay(1000);
    lcd->print("GO!");
    delay(1000);
    lcd->clear();
    randomSeed(millis());

    for(int i = 0; i < 5; i++) {
        long player = random(2);
        delay(random(500, 5000));
        setLEDByUID(ctx.players[player]);
        uint32_t ledStarted = millis();
        bool clicked = false;
        while (millis() - ledStarted < 5000) {
            bool p1Btn = !digitalRead(RED_BTN);
            bool p2Btn = !digitalRead(YELLOW_BTN);
            if (!p1Btn && !p2Btn) {
                continue;
            }
            if (p1Btn && p2Btn) {
                lcd->clear();
                lcd->setCursor(0, 0);
                lcd->print("Both players");
                lcd->setCursor(0, 1);
                lcd->print("pressed!");
                clicked = true;
                break;
            }
            if ((player==0 && p1Btn) || (player==1 && p2Btn)) {
                lcd->clear();
                lcd->setCursor(0, 0);
                lcd->print("Player ");
                lcd->print(player + 1);
                lcd->print(" wins!");
                lcd->setCursor(0, 1);
                lcd->print(millis() - ledStarted);
                lcd->print("ms");
                clicked = true;
                break;
            } else {
                lcd->clear();
                lcd->setCursor(0, 0);
                lcd->print("Player ");
                lcd->print(player + 1);
                lcd->print(" lost!");
                clicked = true;
                break;
            }
        }
        if(!clicked) {
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("Timeout!");
        }
        setLED(0, 0, 0);
        delay(2000);
        lcd->clear();
    }
    ctx.gameState = GameState::END;
}