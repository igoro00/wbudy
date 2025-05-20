#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "WbudyRFID.h"

#include "pindefs.hpp"

int main()
{
    stdio_init_all();

    // Initialise the RFID reader
    RfidReader rfid(RFID_SCK, RFID_MOSI, RFID_MISO, RFID_CS, RFID_RST);

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