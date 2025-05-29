#ifndef WBUDY_RFID_H
#define WBUDY_RFID_H

#include <cstdint>
#include "hardware/spi.h"
#include "hardware/gpio.h"

class WbudyRFID {
public:
    typedef void (*InterruptCallback)(uint32_t);

    // Constructor - sets up pins and initializes SPI
    WbudyRFID(spi_inst_t* spi, uint8_t csPin, uint8_t resetPin);
    
    // Initialize the RFID reader
    bool init();
    
    // Get UUID of card (returns 0 if no card present)
    uint32_t getUUID();

    void attachInterrupt(InterruptCallback callback);
    void detachInterrupt();

    // Metoda do cyklicznego sprawdzania obecności karty i wywołania callbacka
    void poll();
    
private:
    // SPI communication methods
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    void setBitMask(uint8_t reg, uint8_t mask);
    void clearBitMask(uint8_t reg, uint8_t mask);
    
    // Card communication methods
    bool isCardPresent();
    bool readCardSerial();
    
    // MFRC522 commands
    void antennaOn();
    void reset();
    
    // Member variables
    spi_inst_t* _spi;
    uint8_t _cs_pin;
    uint8_t _reset_pin;
    uint8_t _uid[4]; // Store the UID bytes
    
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

    InterruptCallback _callback = nullptr;
    bool _cardPresentLast = false; // do wykrywania zbocza (przyłożenie karty)
};

#endif // WBUDY_RFID_H