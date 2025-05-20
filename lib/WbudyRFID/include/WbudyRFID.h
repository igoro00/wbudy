#ifndef RFID_READER_H
#define RFID_READER_H

#include <cstdint>
#include "hardware/structs/spi.h"

class RfidReader {
private:
    // Piny SPI
    uint8_t _sck_pin;
    uint8_t _mosi_pin;
    uint8_t _miso_pin;
    uint8_t _cs_pin;
    uint8_t _rst_pin;
    
    // Wskaźnik na strukturę SPI
    spi_hw_t* spi;
    
    // Rejestry MFRC522
    static constexpr uint8_t COMMAND_REG = 0x01;
    static constexpr uint8_t FIFO_DATA_REG = 0x09;
    static constexpr uint8_t FIFO_LEVEL_REG = 0x0A;
    static constexpr uint8_t CONTROL_REG = 0x0C;
    static constexpr uint8_t STATUS_REG = 0x07;
    static constexpr uint8_t COM_IRQ_REG = 0x04;
    static constexpr uint8_t DIV_IRQ_REG = 0x05;
    static constexpr uint8_t ERROR_REG = 0x06;
    static constexpr uint8_t TModeReg = 0x2A;
    static constexpr uint8_t TPrescalerReg = 0x2B;
    static constexpr uint8_t TReloadRegL = 0x2C;
    static constexpr uint8_t TReloadRegH = 0x2D;
    static constexpr uint8_t TxASKReg = 0x15;
    static constexpr uint8_t ModeReg = 0x11;
    static constexpr uint8_t TxControlReg = 0x14;
    static constexpr uint8_t BitFramingReg = 0x0D;
    
    // Komendy MFRC522
    static constexpr uint8_t CMD_IDLE = 0x00;
    static constexpr uint8_t CMD_MEM = 0x01;
    static constexpr uint8_t CMD_GENERATE_RANDOM_ID = 0x02;
    static constexpr uint8_t CMD_CALC_CRC = 0x03;
    static constexpr uint8_t CMD_TRANSMIT = 0x04;
    static constexpr uint8_t CMD_NO_CMD_CHANGE = 0x07;
    static constexpr uint8_t CMD_RECEIVE = 0x08;
    static constexpr uint8_t CMD_TRANSCEIVE = 0x0C;
    static constexpr uint8_t CMD_MF_AUTHENT = 0x0E;
    static constexpr uint8_t CMD_SOFT_RESET = 0x0F;
    
    // Flagi
    static constexpr uint8_t BIT_INTERRUPT_REQUEST = 0x10;
    static constexpr uint8_t BIT_TIMER_INTERRUPT = 0x01;
    static constexpr uint8_t BIT_RX_DONE = 0x20;
    static constexpr uint8_t BIT_IDLE = 0x10;
    
    // Metody prywatne
    void spiInit();
    uint8_t spiTransfer(uint8_t data);
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void reset();
    bool isNewCardPresent();
    bool readCardSerial(uint8_t* uid);
    void haltA();
    void stopCrypto1();

public:
    // Konstruktor z konfiguracją pinów
    RfidReader(uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t cs_pin, uint8_t rst_pin);
    
    // Pobieranie UUID w formie 4B
    uint32_t getUid();
};

#endif // RFID_READER_H