#pragma once

#include <cstdint>
#include "hardware/spi.h"
#include "hardware/gpio.h"

class WbudyRFID {
public:
    using CardCallback = void(*)(uint32_t);

    WbudyRFID(spi_inst_t* spi, uint8_t csPin, uint8_t resetPin, uint8_t irqPin);

    bool init();
    uint32_t getUUID();
    void startCardDetection(); // Zmieniona nazwa z pollForCard
    bool isCardPresent();

    // Obsługa przerwań
    void attachInterrupt(CardCallback cb);
    void detachInterrupt();

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
    uint8_t _uid[4];
    volatile bool _irq_fired;
    CardCallback _callback;

    // MFRC522 Register addresses
    static constexpr uint8_t CommandReg = 0x01;
    static constexpr uint8_t ComIEnReg = 0x02;
    static constexpr uint8_t DivIEnReg = 0x03;
    static constexpr uint8_t ComIrqReg = 0x04;
    static constexpr uint8_t DivIrqReg = 0x05;
    static constexpr uint8_t ErrorReg = 0x06;
    static constexpr uint8_t Status1Reg = 0x07;
    static constexpr uint8_t Status2Reg = 0x08;
    static constexpr uint8_t FIFODataReg = 0x09;
    static constexpr uint8_t FIFOLevelReg = 0x0A;
    static constexpr uint8_t ControlReg = 0x0C;
    static constexpr uint8_t BitFramingReg = 0x0D;
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