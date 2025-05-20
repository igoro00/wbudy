#include "WbudyRFID.h"
#include "hardware/gpio.h"
#include "hardware/regs/spi.h"
#include "hardware/structs/spi.h"
#include "pico/stdlib.h"

RfidReader::RfidReader(uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t cs_pin, uint8_t rst_pin) :
    _sck_pin(sck_pin),
    _mosi_pin(mosi_pin),
    _miso_pin(miso_pin),
    _cs_pin(cs_pin),
    _rst_pin(rst_pin) {
    
    // Inicjalizacja SPI
    spiInit();
    
    // Reset i inicjalizacja czytnika RFID
    reset();
}

void RfidReader::spiInit() {
    // Konfiguracja pinów
    gpio_set_function(_sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(_mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(_miso_pin, GPIO_FUNC_SPI);
    
    gpio_init(_cs_pin);
    gpio_set_dir(_cs_pin, GPIO_OUT);
    gpio_put(_cs_pin, 1); // CS nieaktywny
    
    gpio_init(_rst_pin);
    gpio_set_dir(_rst_pin, GPIO_OUT);
    gpio_put(_rst_pin, 1); // Reset nieaktywny
    
    // Konfiguracja SPI bezpośrednio na rejestrach
    // Zakładam SPI0, można zmienić na SPI1 jeśli potrzeba
    spi = spi0_hw;
    
    // Reset SPI
    spi->cr1 = 0;
    
    // Konfiguracja SPI: 1MHz, tryb 0, 8 bitów
    uint32_t div = 125; // 125MHz (zegar systemowy) / 1MHz = 125
    spi->cpsr = div & 0xFE; // Dzielnik musi być parzysty
    
    // CR0: 8-bit, CPOL=0, CPHA=0
    spi->cr0 = (7 << SPI_SSPCR0_DSS_LSB) | // 8-bit (wartość 7 oznacza 8 bitów)
               (0 << SPI_SSPCR0_FRF_LSB) |  // Format SPI
               (0 << SPI_SSPCR0_SPO_LSB) | // CPOL=0
               (0 << SPI_SSPCR0_SPH_LSB);  // CPHA=0
    
    // Włącz SPI
    spi->cr1 = SPI_SSPCR1_SSE_BITS;
}

uint8_t RfidReader::spiTransfer(uint8_t data) {
    // Czekaj aż SPI będzie gotowe do transmisji
    while (!(spi->sr & SPI_SSPSR_TNF_BITS));
    
    // Wyślij dane
    spi->dr = data;
    
    // Czekaj na zakończenie transmisji
    while (!(spi->sr & SPI_SSPSR_RNE_BITS));
    
    // Odbierz dane
    return spi->dr & 0xFF;
}

void RfidReader::writeRegister(uint8_t reg, uint8_t value) {
    gpio_put(_cs_pin, 0); // CS aktywny
    
    // Adres rejestru (bit 7 = 0 dla zapisu)
    spiTransfer(reg & 0x7F);
    
    // Wartość
    spiTransfer(value);
    
    gpio_put(_cs_pin, 1); // CS nieaktywny
}

uint8_t RfidReader::readRegister(uint8_t reg) {
    gpio_put(_cs_pin, 0); // CS aktywny
    
    // Adres rejestru (bit 7 = 1 dla odczytu)
    spiTransfer(reg | 0x80);
    
    // Odczyt wartości
    uint8_t value = spiTransfer(0x00);
    
    gpio_put(_cs_pin, 1); // CS nieaktywny
    
    return value;
}

void RfidReader::reset() {
    // Reset sprzętowy
    gpio_put(_rst_pin, 0);
    sleep_us(1);
    gpio_put(_rst_pin, 1);
    sleep_us(50);
    
    // Reset programowy
    writeRegister(COMMAND_REG, CMD_SOFT_RESET);
    
    // Czekaj na zakończenie resetu
    sleep_ms(50);
    
    // Inicjalizacja
    writeRegister(CONTROL_REG, 0x10); // Włącz przerwania
}

bool RfidReader::isNewCardPresent() {
    // Wyczyść bufor FIFO
    writeRegister(FIFO_LEVEL_REG, 0x80);
    
    // Przygotuj komendę REQA
    uint8_t buffer[2] = {0x26, 0x00}; // REQA command
    
    // Wyślij komendę
    gpio_put(_cs_pin, 0);
    spiTransfer(FIFO_DATA_REG & 0x7F);
    spiTransfer(buffer[0]);
    gpio_put(_cs_pin, 1);
    
    // Ustaw tryb transceive
    writeRegister(COMMAND_REG, CMD_TRANSCEIVE);
    
    // Rozpocznij transmisję
    uint8_t control = readRegister(CONTROL_REG);
    writeRegister(CONTROL_REG, control | 0x80);
    
    // Czekaj na odpowiedź (z timeoutem)
    uint32_t timeout = 1000;
    uint8_t irq;
    
    do {
        irq = readRegister(COM_IRQ_REG);
        if (--timeout == 0) return false;
    } while (!(irq & BIT_INTERRUPT_REQUEST));
    
    // Sprawdź czy otrzymaliśmy odpowiedź
    uint8_t fifoLevel = readRegister(FIFO_LEVEL_REG);
    return (fifoLevel > 0);
}

bool RfidReader::readCardSerial(uint8_t* uid) {
    // Wyczyść bufor FIFO
    writeRegister(FIFO_LEVEL_REG, 0x80);
    
    // Przygotuj komendę SELECT
    uint8_t buffer[9] = {0x93, 0x20}; // SELECT command
    
    // Wyślij komendę
    gpio_put(_cs_pin, 0);
    spiTransfer(FIFO_DATA_REG & 0x7F);
    for (uint8_t i = 0; i < 2; i++) {
        spiTransfer(buffer[i]);
    }
    gpio_put(_cs_pin, 1);
    
    // Ustaw tryb transceive
    writeRegister(COMMAND_REG, CMD_TRANSCEIVE);
    
    // Rozpocznij transmisję
    uint8_t control = readRegister(CONTROL_REG);
    writeRegister(CONTROL_REG, control | 0x80);
    
    // Czekaj na odpowiedź (z timeoutem)
    uint32_t timeout = 1000;
    uint8_t irq;
    
    do {
        irq = readRegister(COM_IRQ_REG);
        if (--timeout == 0) return false;
    } while (!(irq & BIT_INTERRUPT_REQUEST));
    
    // Odczytaj UID
    uint8_t fifoLevel = readRegister(FIFO_LEVEL_REG);
    if (fifoLevel < 4) return false;
    
    for (uint8_t i = 0; i < 4; i++) {
        gpio_put(_cs_pin, 0);
        spiTransfer(FIFO_DATA_REG | 0x80);
        uid[i] = spiTransfer(0x00);
        gpio_put(_cs_pin, 1);
    }
    
    return true;
}

void RfidReader::haltA() {
    // Komenda HALT
    uint8_t buffer[4] = {0x50, 0x00, 0x00, 0x00};
    
    // Wyślij komendę
    gpio_put(_cs_pin, 0);
    spiTransfer(FIFO_DATA_REG & 0x7F);
    for (uint8_t i = 0; i < 4; i++) {
        spiTransfer(buffer[i]);
    }
    gpio_put(_cs_pin, 1);
    
    // Ustaw tryb transceive
    writeRegister(COMMAND_REG, CMD_TRANSCEIVE);
    
    // Rozpocznij transmisję
    uint8_t control = readRegister(CONTROL_REG);
    writeRegister(CONTROL_REG, control | 0x80);
    
    // Krótkie opóźnienie
    sleep_ms(1);
}

void RfidReader::stopCrypto1() {
    // Wyłącz szyfrowanie
    uint8_t control = readRegister(CONTROL_REG);
    writeRegister(CONTROL_REG, control & (~0x08));
}

uint32_t RfidReader::getUid() {
    // Sprawdź czy karta jest obecna
    if (!isNewCardPresent()) {
        return 0;
    }
    
    // Odczytaj numer seryjny karty
    uint8_t uid[4];
    if (!readCardSerial(uid)) {
        return 0;
    }
    
    // Konwersja 4 bajtów UID na uint32_t
    uint32_t result = 0;
    for (uint8_t i = 0; i < 4; i++) {
        result = (result << 8) | uid[i];
    }
    
    // Zatrzymaj komunikację z kartą
    haltA();
    
    // Zatrzymaj szyfrowanie
    stopCrypto1();
    
    return result;
}