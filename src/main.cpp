#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "WbudyRFID.h"
#include "pindefs.hpp"
#include <stdio.h>

int main()
{
    stdio_init_all();
    
    // Poczekaj na inicjalizację portu szeregowego
    sleep_ms(2000);
    
    printf("Inicjalizacja czytnika RFID...\n");

    // Inicjalizacja czytnika RFID
    RfidReader rfid(RFID_SCK, RFID_MOSI, RFID_MISO, RFID_CS, RFID_RST);
    
    printf("Czytnik RFID zainicjalizowany. Oczekiwanie na karty...\n");

    while (true) {
        uint32_t uid = rfid.getUid();
        if (uid != 0) {
            printf("Card UID: %08X\n", uid);
        } else {
            printf("No card detected\n");
        }
        sleep_ms(1000);
    }
}