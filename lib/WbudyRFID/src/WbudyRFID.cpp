#include "WbudyRFID.h"
#include "pico/stdlib.h"
#include <FreeRTOS.h>
#include <stdio.h>
#include <task.h>

WbudyRFID::WbudyRFID(
	spi_inst_t *spi,
	uint8_t csPin,
	uint8_t resetPin,
	uint8_t irqPin,
	uint8_t rfidMiso,
	uint8_t rfidSck,
	uint8_t rfidMosi
) {
	init(spi, csPin, resetPin, irqPin, rfidMiso, rfidSck, rfidMosi);
}

WbudyRFID::WbudyRFID() {}

bool WbudyRFID::init(
	spi_inst_t *spi,
	uint8_t csPin,
	uint8_t resetPin,
	uint8_t irqPin,
	uint8_t rfidMiso,
	uint8_t rfidSck,
	uint8_t rfidMosi
) {
	this->_spi = spi;
	this->_cs_pin = csPin;
	this->_reset_pin = resetPin;
	this->_irq_pin = irqPin;
	this->_rfid_miso = rfidMiso;
	this->_rfid_sck = rfidSck;
	this->_rfid_mosi = rfidMosi;

    // Configure CS pin as output and set it high
    gpio_init(_cs_pin);
    gpio_set_dir(_cs_pin, GPIO_OUT);
    gpio_put(_cs_pin, 1);
    
    // Configure reset pin as output and set it high
    gpio_init(_reset_pin);
    gpio_set_dir(_reset_pin, GPIO_OUT);
    gpio_put(_reset_pin, 1);

	spi_init(_spi, 1000000); // 1MHz
    spi_set_format(_spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    // Set up SPI pins
    gpio_set_function(_rfid_miso, GPIO_FUNC_SPI);
    gpio_set_function(_rfid_sck, GPIO_FUNC_SPI);
    gpio_set_function(_rfid_mosi, GPIO_FUNC_SPI);
    
    // Reset the MFRC522
    reset();

    // Configure the MFRC522
    writeRegister(TxASKReg, 0x40);       // 100% ASK modulation
    writeRegister(ModeReg, 0x3D);        // CRC preset value = 0x6363
    
    // Turn on the antenna
    antennaOn();
    
    printf("MFRC522 Initialized\n");
    
    // Check if communication is working by reading version register
    uint8_t version = readRegister(0x37);
    printf("MFRC522 Version: 0x%02X\n", version);
	xTaskCreate(
		WbudyRFID::tRFID,
		"tRFID",
		configMINIMAL_STACK_SIZE,
		this,
		tskIDLE_PRIORITY + 1,
		NULL
	);
    
    return (version != 0 && version != 0xFF);
}

// Get UUID of card (returns 0 if no card present)
uint32_t WbudyRFID::getUUID() {
    // Check if a card is present
    if (!isCardPresent()) {
        return 0;
    }
    
    // Read the card serial number
    if (!readCardSerial()) {
        return 0;
    }
    
    // Convert the 4-byte UID to a 32-bit integer
    uint32_t uuid = 0;
    for (int i = 0; i < 4; i++) {
        uuid = (uuid << 8) | _uid[i];
    }
    
    return uuid;
}

void WbudyRFID::setOnScanned(void (*callback)(uint32_t)) {
	_callback = callback;
}

// Read a register from the MFRC522
uint8_t WbudyRFID::readRegister(uint8_t reg) {
    uint8_t value;
    
    // Select the MFRC522 by setting CS low
    gpio_put(_cs_pin, 0);
    
    // Send the register address with MSB set to 1 to indicate read
    uint8_t address = ((reg << 1) & 0x7E) | 0x80;
    spi_write_blocking(_spi, &address, 1);
    
    // Read the value
    spi_read_blocking(_spi, 0x00, &value, 1);
    
    // Deselect the MFRC522 by setting CS high
    gpio_put(_cs_pin, 1);
    
    return value;
}

// Write to a register in the MFRC522
void WbudyRFID::writeRegister(uint8_t reg, uint8_t value) {
    // Select the MFRC522 by setting CS low
    gpio_put(_cs_pin, 0);
    
    // Send the register address with MSB set to 0 to indicate write
    uint8_t address = (reg << 1) & 0x7E;
    spi_write_blocking(_spi, &address, 1);
    
    // Write the value
    spi_write_blocking(_spi, &value, 1);
    
    // Deselect the MFRC522 by setting CS high
    gpio_put(_cs_pin, 1);
}

// Set bits in a register
void WbudyRFID::setBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = readRegister(reg);
    writeRegister(reg, tmp | mask);
}

// Clear bits in a register
void WbudyRFID::clearBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = readRegister(reg);
    writeRegister(reg, tmp & (~mask));
}

void WbudyRFID::tRFID(void *pvParameters) {
	WbudyRFID *rfid = static_cast<WbudyRFID *>(pvParameters);
	while(1){
		uint32_t uid = rfid->getUUID();
		if (uid != 0) {
			printf("[RFID] Card detected with UID: %08X\n", uid);
			if (rfid->_callback) {
				rfid->_callback(uid);
			}
		}
		vTaskDelay(100 / portTICK_PERIOD_MS); // Check every second
	}
}

// Turn on the antenna by enabling pins TX1 and TX2
void WbudyRFID::antennaOn() {
    uint8_t value = readRegister(TxControlReg);
    if ((value & 0x03) != 0x03) {
        writeRegister(TxControlReg, value | 0x03);
    }
}

// Reset the MFRC522
void WbudyRFID::reset() {
    // Soft reset
    writeRegister(CommandReg, PCD_RESETPHASE);
    
    // Wait for the reset to complete
    sleep_ms(50);
}

// Check if a card is present
bool WbudyRFID::isCardPresent() {
    // Prepare for REQA command
    writeRegister(BitFramingReg, 0x07);  // TxLastBits = 7 means transmit only 7 bits of the last byte
    
    // Send REQA command
    writeRegister(ComIrqReg, 0x7F);      // Clear all interrupt flags
    writeRegister(FIFOLevelReg, 0x80);   // Flush FIFO buffer
    writeRegister(FIFODataReg, PICC_REQIDL); // Write data to transmit to FIFO
    writeRegister(CommandReg, PCD_TRANSCEIVE); // Start transmission
    writeRegister(BitFramingReg, 0x87);  // Start transmission (StartSend = 1)
    
    // Wait for the command to complete
    uint8_t irqFlags;
    uint8_t n = 0;
    do {
        irqFlags = readRegister(ComIrqReg);
        n++;
    } while ((n < 100) && !(irqFlags & 0x31)); // Wait for RxIRq, IdleIRq, or ErrIRq
    
    // Check if a card responded
    if (irqFlags & 0x01) { // Timer overflow
        return false;
    }
    
    uint8_t fifoLevel = readRegister(FIFOLevelReg);
    return (fifoLevel > 0);
}

// Read the card serial number
bool WbudyRFID::readCardSerial() {
    // Prepare for anticollision command
    writeRegister(BitFramingReg, 0x00);  // TxLastBits = 0, RxAlign = 0
    
    // Send anticollision command
    writeRegister(ComIrqReg, 0x7F);      // Clear all interrupt flags
    writeRegister(FIFOLevelReg, 0x80);   // Flush FIFO buffer
    writeRegister(FIFODataReg, PICC_ANTICOLL); // Write data to transmit to FIFO
    writeRegister(FIFODataReg, 0x20);    // NVB = 0x20, meaning no data bytes transmitted yet
    writeRegister(CommandReg, PCD_TRANSCEIVE); // Start transmission
    writeRegister(BitFramingReg, 0x80);  // Start transmission (StartSend = 1)
    
    // Wait for the command to complete
    uint8_t irqFlags;
    uint8_t n = 0;
    do {
        irqFlags = readRegister(ComIrqReg);
        n++;
    } while ((n < 100) && !(irqFlags & 0x31)); // Wait for RxIRq, IdleIRq, or ErrIRq
    
    // Check if a card responded
    if (irqFlags & 0x01) { // Timer overflow
        return false;
    }
    
    // Read the UID bytes
    uint8_t fifoLevel = readRegister(FIFOLevelReg);
    if (fifoLevel < 5) { // Should be 5 bytes (4 UID + 1 BCC)
        return false;
    }
    
    for (uint8_t i = 0; i < 4; i++) {
        _uid[i] = readRegister(FIFODataReg);
    }
    
    // Read and check BCC (Block Check Character)
    uint8_t bcc = readRegister(FIFODataReg);
    uint8_t calculated_bcc = _uid[0] ^ _uid[1] ^ _uid[2] ^ _uid[3];
    
    return (bcc == calculated_bcc);
}