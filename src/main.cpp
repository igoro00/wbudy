#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "WbudyLCD.h"

#include "pindefs.hpp"

int main()
{
    stdio_init_all();
    lcd_init(0x27, LCD_SDA, LCD_SCL);  
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_char('A');
    lcd_print(" Czesc!");

    

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
