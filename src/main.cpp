#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "WbudyLCD.h"

#include "pindefs.hpp"

int main()
{
    WbudyLCD lcd(i2c0, 0x27, 4, 5);
    lcd.init();
    // lcd.backlightOff();
    lcd.setCursor(0, 0);
    //lcd.print("Witaj RP2040!");
    lcd.printRegister("Adrian");
    lcd.setCursor(1,0);
    lcd.printPolish(L"Urbańczyk");
    

    //const uint8_t ogonek[8] = {
    //    0b00000,
    //    0b00100,
    //    0b00000,
    //    0b01110,
    //    0b10001,
    //    0b11111,
    //    0b10000,
    //    0b00000
    //};

    //lcd.loadCustomChar(0, ogonek);
    //lcd.setCursor(1, 0);
    //lcd.print("Znak: ");
    //lcd.printCustomChar(0);

    while (true) {
        sleep_ms(1000);
    }
}
