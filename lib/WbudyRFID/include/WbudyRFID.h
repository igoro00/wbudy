#ifndef RFID_READER_H
#define RFID_READER_H

#include <cstdint>

class RfidReader {
private:
    // Piny SPI
    uint8_t _sck_pin;
    uint8_t _mosi_pin;
    uint8_t _miso_pin;
    uint8_t _cs_pin;
    uint8_t _rst_pin;
    
    // Wskaźnik na strukturę SPI
    void* _spi;
    
    // Rejestry MFRC522 (przykładowe)
    static constexpr uint8_t COMMAND_REG = 0x01;
    static constexpr uint8_t FIFO_DATA_REG = 0x09;
    static constexpr uint8_t FIFO_LEVEL_REG = 0x0A;
    static constexpr uint8_t CONTROL_REG = 0x0C;
    static constexpr uint8_t STATUS_REG = 0x07;
    static constexpr uint8_t COM_IRQ_REG = 0x04;
    
    // Komendy MFRC522
    static constexpr uint8_t CMD_IDLE = 0x00;
    static constexpr uint8_t CMD_MEM = 0x01;
    static constexpr uint8_t CMD_TRANSCEIVE = 0x0C;
    static constexpr uint8_t CMD_SOFT_RESET = 0x0F;
    
    // Flagi
    static constexpr uint8_t BIT_INTERRUPT_REQUEST = 0x10;
    static constexpr uint8_t BIT_TIMER_INTERRUPT = 0x01;
    
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