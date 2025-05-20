#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "pico/cyw43_arch.h"
#include "WbudyRFID.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_RST  20

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

    // Initialise the RFID reader
    RfidReader rfid(PIN_SCK, PIN_MOSI, PIN_MISO, PIN_CS, PIN_RST);

    while (true) {
        uint32_t uid = rfid.getUid();
        if (uid != 0) {
            printf("Card UID: %08X\n", uid);
        } else {
            printf("No card detected\n");
        }
        sleep_ms(1000);
    }    

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
