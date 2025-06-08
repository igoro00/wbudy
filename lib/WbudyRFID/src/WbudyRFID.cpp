
#include "WbudyRFID.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

WbudyRFID* WbudyRFID::_instance = nullptr;

WbudyRFID::WbudyRFID(spi_inst_t* spi, uint8_t csPin, uint8_t resetPin, uint8_t irqPin, uint8_t rfidMiso, uint8_t rfidSck, uint8_t rfidMosi) {
    init(spi, csPin, resetPin, irqPin, rfidMiso, rfidSck, rfidMosi);
}

WbudyRFID::WbudyRFID() {}

bool WbudyRFID::init(spi_inst_t* spi, uint8_t csPin, uint8_t resetPin, uint8_t irqPin, uint8_t rfidMiso, uint8_t rfidSck, uint8_t rfidMosi) {
    this->_spi = spi;
    this->_cs_pin = csPin;
    this->_reset_pin = resetPin;
    this->_irq_pin = irqPin;
    this->_rfid_miso = rfidMiso;
    this->_rfid_sck = rfidSck;
    this->_rfid_mosi = rfidMosi;
    this->_callback = nullptr;
    _instance = this;

    // Inicjalizacja mutexa
    this->taskIrqMutex = xSemaphoreCreateBinary();
    xSemaphoreGive(this->taskIrqMutex);

    // Inicjalizacja mutexa - początkowo zablokowany (brak callbacka)
    this->callbackActiveMutex = xSemaphoreCreateBinary();
    // Nie oddajemy semafora - tPing będzie zablokowany dopóki nie zostanie podpięty callback

    // Konfiguracja przerwania
    gpio_set_irq_enabled_with_callback(_irq_pin, GPIO_IRQ_EDGE_FALL, true, &WbudyRFID::irqHandler);

    // Inicjalizacja SPI
    spi_init(_spi, 1000000);
    spi_set_format(_spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(_rfid_miso, GPIO_FUNC_SPI);
    gpio_set_function(_rfid_sck, GPIO_FUNC_SPI);
    gpio_set_function(_rfid_mosi, GPIO_FUNC_SPI);

    // Inicjalizacja pinów
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

    // Włączenie przerwań dla RxIRq i IdleIRq
    writeRegister(ComIEnReg, 0xA0); // IRqInv (0x80) + RxIEn (0x20) = 0xA0
    
    // Wyczyść wszystkie flagi przerwań
    writeRegister(ComIrqReg, 0x7F);
    writeRegister(DivIrqReg, 0x7F);

    uint8_t version = readRegister(0x37);

    xTaskCreate(
        tPing,
        "tPing",
        2048,
        this,
        tskIDLE_PRIORITY + 1,
        NULL
    );

	xTaskCreate(
		tReadCard,
		"tReadCard",
		2048,
		this,
		tskIDLE_PRIORITY + 3,
		NULL
	);

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
    // Oddanie semafora - odblokuj tPing
    xSemaphoreGive(callbackActiveMutex);
}

void WbudyRFID::detachInterrupt() {
    _callback = nullptr;
    // Zabiera semafor - zablokuj tPing
    xSemaphoreTake(callbackActiveMutex, 0); // Nie czekaj, jeśli semafor już jest zabrany
}

void WbudyRFID::irqHandler(uint gpio, uint32_t events) {
    if (_instance && gpio == _instance->_irq_pin && (events & GPIO_IRQ_EDGE_FALL)) {
        BaseType_t xHigherPriorityTaskWoken = pdTRUE;
        // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(_instance->taskIrqMutex, &xHigherPriorityTaskWoken);
    }
}

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

void WbudyRFID::tPing(void *pvParameters) {
    WbudyRFID *self = static_cast<WbudyRFID*>(pvParameters);

    while (1) {
        // Czeka na semafor - blokuj jeśli nie ma callbacka
        if (xSemaphoreTake(self->callbackActiveMutex, portMAX_DELAY) == pdTRUE) {
            self->startCardDetection();
            // Oddanie semafora z powrotem - pozwól na kolejne pingowanie
            xSemaphoreGive(self->callbackActiveMutex);
            vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void WbudyRFID::tReadCard(void *pvParameters) {
    WbudyRFID *self = static_cast<WbudyRFID*>(pvParameters);
    while (1) {
        if (xSemaphoreTake(self->taskIrqMutex, portMAX_DELAY) == pdTRUE) {
            uint8_t irqFlags = self->readRegister(ComIrqReg);

            if (irqFlags & 0x20) { // RxIRq - odebrano dane
                uint8_t fifoLevel = self->readRegister(FIFOLevelReg);

                if (fifoLevel > 0 && self->_callback) {
                    uint32_t uuid = self->getUUID();
                    if (self->_callback != NULL) {
                        self->_callback(uuid);
                    }
                }
            }
            // Wyczyść flagi przerwań
            self->writeRegister(ComIrqReg, 0x7F);
        }
    }
}