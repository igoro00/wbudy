#include "WbudyRFID.h"
#include "pico/stdlib.h"
#include <stdio.h>

WbudyRFID* WbudyRFID::_instance = nullptr;

WbudyRFID::WbudyRFID(spi_inst_t* spi, uint8_t csPin, uint8_t resetPin, uint8_t irqPin)
    : _spi(spi), _cs_pin(csPin), _reset_pin(resetPin), _irq_pin(irqPin), _irq_fired(false), _callback(nullptr)
{
    _instance = this;
}

bool WbudyRFID::init() {
    gpio_init(_cs_pin);
    gpio_set_dir(_cs_pin, GPIO_OUT);
    gpio_put(_cs_pin, 1);

    gpio_init(_reset_pin);
    gpio_set_dir(_reset_pin, GPIO_OUT);
    gpio_put(_reset_pin, 1);

    gpio_init(_irq_pin);
    gpio_set_dir(_irq_pin, GPIO_IN);
    gpio_pull_up(_irq_pin);

    reset();
    writeRegister(TxASKReg, 0x40);
    writeRegister(ModeReg, 0x3D);
    antennaOn();

    // KLUCZOWA ZMIANA: Włącz przerwania dla RxIRq i IdleIRq
    writeRegister(ComIEnReg, 0xA0); // IRqInv (0x80) + RxIEn (0x20) = 0xA0
    
    // Wyczyść wszystkie flagi przerwań
    writeRegister(ComIrqReg, 0x7F);
    writeRegister(DivIrqReg, 0x7F);

    printf("MFRC522 Initialized with IRQ\n");
    uint8_t version = readRegister(0x37);
    printf("MFRC522 Version: 0x%02X\n", version);

    return (version != 0 && version != 0xFF);
}

void WbudyRFID::startCardDetection() {
    // Wyczyść flagi przerwań
    writeRegister(ComIrqReg, 0x7F);
    
    // Przygotuj FIFO
    writeRegister(FIFOLevelReg, 0x80);  // Flush FIFO
    writeRegister(CommandReg, PCD_IDLE);
    
    // Wyślij REQA
    writeRegister(FIFODataReg, PICC_REQIDL);
    writeRegister(BitFramingReg, 0x07);
    writeRegister(CommandReg, PCD_TRANSCEIVE);
    writeRegister(BitFramingReg, 0x87); // Start transmission
    
    // Teraz MFRC522 wyśle sygnał i może wygenerować IRQ
}

uint32_t WbudyRFID::getUUID() {
    if (!readCardSerial()) return 0;
    uint32_t uuid = 0;
    for (int i = 0; i < 4; i++) uuid = (uuid << 8) | _uid[i];
    return uuid;
}

void WbudyRFID::attachInterrupt(CardCallback cb) {
    _callback = cb;
    _irq_fired = false;
    gpio_set_irq_enabled_with_callback(_irq_pin, GPIO_IRQ_EDGE_FALL, true, &WbudyRFID::irqHandler);
    printf("IRQ attached to pin %d\n", _irq_pin);
}

void WbudyRFID::detachInterrupt() {
    gpio_set_irq_enabled(_irq_pin, GPIO_IRQ_EDGE_FALL, false);
    _callback = nullptr;
}

void WbudyRFID::irqHandler(uint gpio, uint32_t events) {
    if (_instance && gpio == _instance->_irq_pin && (events & GPIO_IRQ_EDGE_FALL)) {
        printf("IRQ triggered on pin %d\n", gpio);
        _instance->handleIRQ();
    }
}

void WbudyRFID::handleIRQ() {
    uint8_t irqFlags = readRegister(ComIrqReg);
    printf("IRQ Flags: 0x%02X\n", irqFlags);
    
    if (irqFlags & 0x20) { // RxIRq - odebrano dane
        printf("RxIRq detected - card response received\n");
        uint8_t fifoLevel = readRegister(FIFOLevelReg);
        printf("FIFO Level: %d\n", fifoLevel);
        
        if (fifoLevel > 0 && _callback) {
            uint32_t uuid = getUUID();
            if (uuid != 0) {
                _callback(uuid);
            }
        }
    }
    
    if (irqFlags & 0x10) { // IdleIRq
        printf("IdleIRq detected - command finished\n");
    }
    
    // Wyczyść flagi przerwań
    writeRegister(ComIrqReg, 0x7F);
}

// Pozostałe metody bez zmian...
uint8_t WbudyRFID::readRegister(uint8_t reg) {
    uint8_t value;
    gpio_put(_cs_pin, 0);
    uint8_t address = ((reg << 1) & 0x7E) | 0x80;
    spi_write_blocking(_spi, &address, 1);
    spi_read_blocking(_spi, 0x00, &value, 1);
    gpio_put(_cs_pin, 1);
    return value;
}

void WbudyRFID::writeRegister(uint8_t reg, uint8_t value) {
    gpio_put(_cs_pin, 0);
    uint8_t address = (reg << 1) & 0x7E;
    spi_write_blocking(_spi, &address, 1);
    spi_write_blocking(_spi, &value, 1);
    gpio_put(_cs_pin, 1);
}

void WbudyRFID::setBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = readRegister(reg);
    writeRegister(reg, tmp | mask);
}

void WbudyRFID::clearBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = readRegister(reg);
    writeRegister(reg, tmp & (~mask));
}

void WbudyRFID::antennaOn() {
    uint8_t value = readRegister(TxControlReg);
    if ((value & 0x03) != 0x03) {
        writeRegister(TxControlReg, value | 0x03);
    }
}

void WbudyRFID::reset() {
    writeRegister(CommandReg, PCD_RESETPHASE);
    sleep_ms(50);
}

bool WbudyRFID::isCardPresent() {
    writeRegister(BitFramingReg, 0x07);
    writeRegister(ComIrqReg, 0x7F);
    writeRegister(FIFOLevelReg, 0x80);
    writeRegister(FIFODataReg, PICC_REQIDL);
    writeRegister(CommandReg, PCD_TRANSCEIVE);
    writeRegister(BitFramingReg, 0x87);

    uint8_t irqFlags;
    uint8_t n = 0;
    do {
        irqFlags = readRegister(ComIrqReg);
        n++;
    } while ((n < 100) && !(irqFlags & 0x31));

    if (irqFlags & 0x01) return false;
    uint8_t fifoLevel = readRegister(FIFOLevelReg);
    return (fifoLevel > 0);
}

bool WbudyRFID::readCardSerial() {
    writeRegister(BitFramingReg, 0x00);
    writeRegister(ComIrqReg, 0x7F);
    writeRegister(FIFOLevelReg, 0x80);
    writeRegister(FIFODataReg, PICC_ANTICOLL);
    writeRegister(FIFODataReg, 0x20);
    writeRegister(CommandReg, PCD_TRANSCEIVE);
    writeRegister(BitFramingReg, 0x80);

    uint8_t irqFlags;
    uint8_t n = 0;
    do {
        irqFlags = readRegister(ComIrqReg);
        n++;
    } while ((n < 100) && !(irqFlags & 0x31));

    if (irqFlags & 0x01) return false;

    uint8_t fifoLevel = readRegister(FIFOLevelReg);
    if (fifoLevel < 5) return false;

    for (uint8_t i = 0; i < 4; i++) {
        _uid[i] = readRegister(FIFODataReg);
    }
    uint8_t bcc = readRegister(FIFODataReg);
    uint8_t calculated_bcc = _uid[0] ^ _uid[1] ^ _uid[2] ^ _uid[3];
    return (bcc == calculated_bcc);
}