#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "WbudyRFID.h"
#include "pindefs.hpp"

#define SPI_PORT spi0

volatile bool card_irq_fired = false;
uint32_t last_uuid = 0;

void onCardDetected(uint32_t uuid) {
    card_irq_fired = true;
    last_uuid = uuid;
    printf("IRQ Callback: Card UUID: 0x%08X\n", uuid);
}

int main() {
    stdio_init_all();
    for (int i = 0; i < 100; i++) {
        if (stdio_usb_connected()) break;
        sleep_ms(50);
    }
    printf("Starting RFID Reader with IRQ...\n");

    spi_init(SPI_PORT, 1000000);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(RFID_MISO, GPIO_FUNC_SPI);
    gpio_set_function(RFID_SCK, GPIO_FUNC_SPI);
    gpio_set_function(RFID_MOSI, GPIO_FUNC_SPI);

    WbudyRFID rfid(SPI_PORT, RFID_CS, RFID_RST, RFID_IRQ);

    if (!rfid.init()) {
        printf("Failed to initialize RFID reader!\n");
        while (true) sleep_ms(1000);
    }

    rfid.attachInterrupt(onCardDetected);
    printf("RFID Reader initialized. Waiting for cards (IRQ)...\n");

    while (true) {
        // Aktywnie skanuj karty - to wyzwoli IRQ
        rfid.startCardDetection();
        
        if (card_irq_fired) {
            printf("Main: Card detected! UUID: 0x%08X\n", last_uuid);
            card_irq_fired = false;
            sleep_ms(1000); // Pauza po wykryciu karty
        }
        sleep_ms(100);
    }
    return 0;
}