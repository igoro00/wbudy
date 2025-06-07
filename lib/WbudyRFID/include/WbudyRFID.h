#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <cstdint>
#include "hardware/spi.h"
#include "hardware/gpio.h"

class WbudyRFID {
public:
    using CardCallback = void(*)(uint32_t);

    WbudyRFID(spi_inst_t* spi, uint8_t csPin, uint8_t resetPin, uint8_t irqPin, uint8_t rfidMiso, uint8_t rfidSck, uint8_t rfidMosi);

    WbudyRFID();
    bool init(spi_inst_t* spi, uint8_t csPin, uint8_t resetPin, uint8_t irqPin, uint8_t rfidMiso, uint8_t rfidSck, uint8_t rfidMosi);

    bool init();
    uint32_t getUUID();
    void startCardDetection(); // Zmieniona nazwa z pollForCard
    bool isCardPresent();

    // Obsługa przerwań
    void attachInterrupt(CardCallback cb);
    void detachInterrupt();

    static void tPing(void *pvParameters);
    static void tReadCard(void *pvParameters);

private:
    // SPI communication methods
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    void setBitMask(uint8_t reg, uint8_t mask);
    void clearBitMask(uint8_t reg, uint8_t mask);

    bool readCardSerial();
    void antennaOn();
    void reset();

    // IRQ obsługa
    static void irqHandler(uint gpio, uint32_t events);
    void handleIRQ();

    // Member variables
    spi_inst_t* _spi;
    uint8_t _cs_pin;
    uint8_t _reset_pin;
    uint8_t _irq_pin;
    uint8_t _rfid_miso;
    uint8_t _rfid_sck;
    uint8_t _rfid_mosi;
    uint8_t _uid[4];
    volatile bool _irq_fired;
    CardCallback _callback;
    SemaphoreHandle_t taskIrqMutex;

    // MFRC522 Register addresses
    static constexpr uint8_t CommandReg = 0x01; // Sterowanie komendami
    static constexpr uint8_t ComIEnReg = 0x02; // Włączanie przerwań komunikacyjnych
    static constexpr uint8_t DivIEnReg = 0x03; // Włączanie przerwań dzielnika
    static constexpr uint8_t ComIrqReg = 0x04; // Flagi przerwań komunikacyjnych
    static constexpr uint8_t DivIrqReg = 0x05; // Flagi przerwań dzielnika
    static constexpr uint8_t ErrorReg = 0x06; // Flagi błędów
    static constexpr uint8_t Status1Reg = 0x07; // Status komunikacji
    static constexpr uint8_t Status2Reg = 0x08; // Status odbiornika i transmitera
    static constexpr uint8_t FIFODataReg = 0x09; // Dane wejściowe/wyjściowe FIFO
    static constexpr uint8_t FIFOLevelReg = 0x0A; // Poziom zapełnienia FIFO
    static constexpr uint8_t ControlReg = 0x0C; // Różne ustawienia sterujące
    static constexpr uint8_t BitFramingReg = 0x0D; // Orientacja bitów dla transmisji i odbioru
    static constexpr uint8_t ModeReg = 0x11;
    static constexpr uint8_t TxControlReg = 0x14;
    static constexpr uint8_t TxASKReg = 0x15;

    // MFRC522 Commands
    static constexpr uint8_t PCD_IDLE = 0x00;
    static constexpr uint8_t PCD_TRANSCEIVE = 0x0C;
    static constexpr uint8_t PCD_RESETPHASE = 0x0F;

    // PICC Commands
    static constexpr uint8_t PICC_REQIDL = 0x26;
    static constexpr uint8_t PICC_ANTICOLL = 0x93;

    // Static pointer for IRQ handler
    static WbudyRFID* _instance;

};