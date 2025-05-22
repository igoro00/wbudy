#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "WbudyRFID.h"

// SPI Configuration
#define SPI_PORT spi0
#define PIN_MISO 0
#define PIN_CS   1
#define PIN_SCK  2
#define PIN_MOSI 3
#define PIN_RST  6

int main() {
    // Initialize stdio
    stdio_init_all();
    
    // Wait for USB serial to connect
    for (int i = 0; i < 100; i++) {
        if (stdio_usb_connected()) break;
        sleep_ms(50);
    }
    
    printf("Starting RFID Reader...\n");
    
    // Initialize SPI
    spi_init(SPI_PORT, 1000000); // 1MHz
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    // Set up SPI pins
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Create RFID reader instance
    WbudyRFID rfid(SPI_PORT, PIN_CS, PIN_RST);
    
    // Initialize the RFID reader
    if (!rfid.init()) {
        printf("Failed to initialize RFID reader!\n");
        while (true) {
            sleep_ms(1000);
        }
    }
    
    printf("RFID Reader initialized. Waiting for cards...\n");
    
    while (true) {
        // Try to read a card
        uint32_t uuid = rfid.getUUID();
        
        if (uuid != 0) {
            printf("Card detected! UUID: 0x%08X\n", uuid);
            sleep_ms(1000); // Wait a second before reading again
        }
        
        sleep_ms(100); // Small delay between reads
    }
    
    return 0;
}