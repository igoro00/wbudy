#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "WbudyRFID.h"
#include "pindefs.hpp"

// SPI Configuration
#define SPI_PORT spi0

// callback do obsługi wykrytej karty
void cardDetectedCallback(uint32_t uuid) {
    printf(">> Karta ZBLIŻONA! UUID: 0x%08X <<\n", uuid);
}

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
    gpio_set_function(RFID_MISO, GPIO_FUNC_SPI);
    gpio_set_function(RFID_SCK, GPIO_FUNC_SPI);
    gpio_set_function(RFID_MOSI, GPIO_FUNC_SPI);
    
    // Create RFID reader instance
    WbudyRFID rfid(SPI_PORT, RFID_CS, RFID_RST);
    
    // Initialize the RFID reader
    if (!rfid.init()) {
        printf("Failed to initialize RFID reader!\n");
        while (true) {
            sleep_ms(1000);
        }
    }
    
    printf("RFID Reader initialized. Waiting for cards...\n");

    // Attach the interrupt (callback function)
    rfid.attachInterrupt(cardDetectedCallback);
    
    while (true) {
        // Call the poll method to check for cards and trigger the callback
        rfid.poll();
        sleep_ms(50); // Check every 50ms
    }
    
    return 0;
}

// zablokowanie pobierania UUID po 30 sekundach

// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/spi.h"
// #include "WbudyRFID.h"
// #include "pindefs.hpp"
// #include "pico/time.h" // Dodajemy nagłówek czasu

// // SPI Configuration
// #define SPI_PORT spi0

// // Funkcja callback do obsługi wykrytej karty
// void cardDetectedCallback(uint32_t uuid) {
//     printf(">> Karta ZBLIŻONA! UUID: 0x%08X <<\n", uuid);
// }

// int main() {
//     // Initialize stdio
//     stdio_init_all();

//     // Wait for USB serial to connect
//     for (int i = 0; i < 100; i++) {
//         if (stdio_usb_connected()) break;
//         sleep_ms(50);
//     }

//     printf("Starting RFID Reader...\n");

//     // Initialize SPI
//     spi_init(SPI_PORT, 1000000); // 1MHz
//     spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

//     // Set up SPI pins
//     gpio_set_function(RFID_MISO, GPIO_FUNC_SPI);
//     gpio_set_function(RFID_SCK, GPIO_FUNC_SPI);
//     gpio_set_function(RFID_MOSI, GPIO_FUNC_SPI);

//     // Create RFID reader instance
//     WbudyRFID rfid(SPI_PORT, RFID_CS, RFID_RST);

//     // Initialize the RFID reader
//     if (!rfid.init()) {
//         printf("Failed to initialize RFID reader!\n");
//         while (true) {
//             sleep_ms(1000);
//         }
//     }

//     printf("RFID Reader initialized. Waiting for cards...\n");

//     // Attach the interrupt (callback function)
//     rfid.attachInterrupt(cardDetectedCallback);

//     // Get the current time
//     absolute_time_t startTime = get_absolute_time();
//     const int DETACH_DELAY_MS = 30000; // 30 seconds

//     while (true) {
//         // Call the poll method to check for cards and trigger the callback
//         rfid.poll();
//         sleep_ms(50); // Check every 50ms

//         // Check if 30 seconds have passed
//         if (absolute_time_diff_us(startTime, get_absolute_time()) > DETACH_DELAY_MS * 1000) {
//             // Detach the interrupt
//             rfid.detachInterrupt();
//             printf(">> MOŻLIWOŚĆ ODCZYTU KART ZABLOKOWANA PO 30 SEKUNDACH <<\n");
//             // Break out of the loop to prevent further polling
//             break;
//         }
//     }

//     // Keep the program running, but without RFID functionality
//     while(true) {
//         sleep_ms(1000);
//     }

//     return 0;
// }