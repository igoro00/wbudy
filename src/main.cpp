#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "pico/cyw43_arch.h"
#include "WbudyRFID.h"
#include "WbudyLCD.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}




int main()
{


    stdio_init_all();
    lcd_init(0x27, I2C_SDA, I2C_SCL);  
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_char('A');
    lcd_print(" Cześć!");

    

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
