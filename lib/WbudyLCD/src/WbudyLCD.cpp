// #include "WbudyLCD.h"
// #include "pico/stdlib.h"
// #include "hardware/gpio.h"
// #include "hardware/i2c.h"

// #define LCD_BACKLIGHT 0x08
// #define LCD_ENABLE 0x04
// #define LCD_COMMAND 0
// #define LCD_DATA 1

// static uint8_t lcd_addr;
// static i2c_inst_t* lcd_i2c = i2c0;

// static void lcd_write_byte(uint8_t data) {
//     i2c_write_blocking(lcd_i2c, lcd_addr, &data, 1, false);
// }

// static void lcd_toggle_enable(uint8_t data) {
//     sleep_us(500);
//     lcd_write_byte(data | LCD_ENABLE);
//     sleep_us(500);
//     lcd_write_byte(data & ~LCD_ENABLE);
//     sleep_us(500);
// }

// static void lcd_send_byte(uint8_t data, uint8_t mode) {
//     uint8_t high_nibble = (data & 0xF0) | LCD_BACKLIGHT | mode;
//     uint8_t low_nibble  = ((data << 4) & 0xF0) | LCD_BACKLIGHT | mode;

//     lcd_write_byte(high_nibble);
//     lcd_toggle_enable(high_nibble);

//     lcd_write_byte(low_nibble);
//     lcd_toggle_enable(low_nibble);
// }

// void lcd_init(uint8_t i2c_addr, uint8_t sda_pin, uint8_t scl_pin) {
//     lcd_addr = i2c_addr;

//     gpio_set_function(sda_pin, GPIO_FUNC_I2C);
//     gpio_set_function(scl_pin, GPIO_FUNC_I2C);
//     gpio_pull_up(sda_pin);
//     gpio_pull_up(scl_pin);

//     i2c_init(lcd_i2c, 100 * 1000);

//     sleep_ms(50);
//     lcd_send_byte(0x30, LCD_COMMAND);
//     sleep_ms(5);
//     lcd_send_byte(0x30, LCD_COMMAND);
//     sleep_us(200);
//     lcd_send_byte(0x30, LCD_COMMAND);
//     lcd_send_byte(0x20, LCD_COMMAND);

//     lcd_send_byte(0x28, LCD_COMMAND); // 4-bit, 2-line
//     lcd_send_byte(0x0C, LCD_COMMAND); // Display ON
//     lcd_send_byte(0x06, LCD_COMMAND); // Entry mode
//     lcd_send_byte(0x01, LCD_COMMAND); // Clear display
//     sleep_ms(2);
// }

// void lcd_clear() {
//     lcd_send_byte(0x01, LCD_COMMAND);
//     sleep_ms(2);
// }

// void lcd_set_cursor(uint8_t row, uint8_t col) {
//     uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
//     lcd_send_byte(0x80 | (col + row_offsets[row]), LCD_COMMAND);
// }

// void lcd_print_char(char c) {
//     lcd_send_byte(c, LCD_DATA);
// }

// void lcd_print(const char* str) {
//     while (*str) {
//         lcd_print_char(*str++);
//     }
// }

// void lcd_load_custom_char(uint8_t location, const uint8_t charmap[8]) {
//     location &= 0x7;
//     lcd_send_byte(0x40 | (location << 3), LCD_COMMAND);
//     for (int i = 0; i < 8; ++i) {
//         lcd_print_char(charmap[i]);
//     }
// }

// void lcd_print_custom_char(uint8_t location) {
//     lcd_print_char(location);
// }

// #include "WbudyLCD.h"
// #include "pico/stdlib.h"
// #include "hardware/i2c.h" 
// #include "hardware/regs/i2c.h"
// #include "hardware/structs/i2c.h"

// #define LCD_BACKLIGHT 0x08
// #define LCD_ENABLE 0x04
// #define LCD_CMD 0x00
// #define LCD_DATA 0x01

// static uint8_t lcd_addr;
// static uint8_t lcd_sda, lcd_scl;

// static void i2c_init_manual() {
//     gpio_set_function(lcd_sda, GPIO_FUNC_I2C);
//     gpio_set_function(lcd_scl, GPIO_FUNC_I2C);
//     gpio_pull_up(lcd_sda);
//     gpio_pull_up(lcd_scl);
//     i2c_init(i2c0, 100 * 1000);
// }

// static void i2c_write_byte(uint8_t val) {
//     while (!(i2c0->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS));
//     i2c0->hw->data_cmd = val;
//     while (i2c0->hw->status & I2C_IC_STATUS_ACTIVITY_BITS);
// }

// static void lcd_write_byte(uint8_t data) {
//     uint8_t buf[1] = { data };
//     i2c_write_blocking(i2c0, lcd_addr, buf, 1, false); // opcjonalnie rejestry zamiast tej linii
// }

// static void lcd_toggle_enable(uint8_t data) {
//     sleep_us(1);
//     lcd_write_byte(data | LCD_ENABLE);
//     sleep_us(1);
//     lcd_write_byte(data & ~LCD_ENABLE);
//     sleep_us(50);
// }

// static void lcd_send_byte(uint8_t data, uint8_t mode) {
//     uint8_t high = (data & 0xF0) | LCD_BACKLIGHT | mode;
//     uint8_t low  = ((data << 4) & 0xF0) | LCD_BACKLIGHT | mode;

//     lcd_write_byte(high);
//     lcd_toggle_enable(high);

//     lcd_write_byte(low);
//     lcd_toggle_enable(low);
// }

// void lcd_init(uint8_t i2c_addr_param, uint8_t sda_pin, uint8_t scl_pin) {
//     lcd_addr = i2c_addr_param;
//     lcd_sda = sda_pin;
//     lcd_scl = scl_pin;

//     i2c_init_manual();
//     sleep_ms(50);

//     lcd_send_byte(0x33, LCD_CMD); // init
//     lcd_send_byte(0x32, LCD_CMD); // 4-bit
//     lcd_send_byte(0x28, LCD_CMD); // 2-line
//     lcd_send_byte(0x0C, LCD_CMD); // display on
//     lcd_send_byte(0x06, LCD_CMD); // cursor move dir
//     lcd_send_byte(0x01, LCD_CMD); // clear display
//     sleep_ms(5);
// }

// void lcd_clear() {
//     lcd_send_byte(0x01, LCD_CMD);
//     sleep_ms(2);
// }

// void lcd_set_cursor(uint8_t row, uint8_t col) {
//     static const uint8_t row_offsets[] = {0x00, 0x40};
//     lcd_send_byte(0x80 | (col + row_offsets[row]), LCD_CMD);
// }

// void lcd_print_char(char c) {
//     lcd_send_byte(c, LCD_DATA);
// }

// void lcd_print(const char* str) {
//     while (*str) {
//         lcd_print_char(*str++);
//     }
// }

// void lcd_load_custom_char(uint8_t location, const uint8_t charmap[8]) {
//     location &= 0x07;
//     lcd_send_byte(0x40 | (location << 3), LCD_CMD);
//     for (uint8_t i = 0; i < 8; ++i) {
//         lcd_print_char(charmap[i]);
//     }
// }

// void lcd_print_custom_char(uint8_t location) {
//     lcd_print_char(location);
// }


#include "WbudyLCD.h"
#include "hardware/regs/i2c.h"
#include "hardware/structs/i2c.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <cstring>

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_COMMAND   0
#define LCD_DATA      1

static i2c_hw_t* i2c_hw = i2c0_hw;
static uint8_t lcd_addr = 0x27;

void lcd_init(uint8_t addr, uint8_t sda_pin, uint8_t scl_pin) {
    lcd_addr = addr;

    // Konfiguracja pinów
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    // Reset kontrolera
    i2c_hw->enable = 0;

    // IC_CON - tryb master, standard speed, disable slave
    i2c_hw->con = (1 << 0)  |   // Master Mode
                  (1 << 5)  |   // Restart enable
                  (1 << 6);     // Slave disable

    // Adres urządzenia
    i2c_hw->tar = lcd_addr;

    // Prędkość (standard 100kHz)
    i2c_hw->fs_scl_hcnt = 60;
    i2c_hw->fs_scl_lcnt = 130;

    i2c_hw->enable = 1;

    sleep_ms(50);

    // Inicjalizacja LCD w 4-bit
    lcd_send_cmd(0x33);
    lcd_send_cmd(0x32);
    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x06);
    lcd_clear();
}

static void i2c_write_byte(uint8_t data) {
    // Czekaj aż FIFO puste
    while (!(i2c_hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS));
    i2c_hw->data_cmd = data;
    while (!(i2c_hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS));
}

static void lcd_write_byte(uint8_t data) {
    i2c_write_byte(data);
}

static void lcd_toggle_enable(uint8_t data) {
    sleep_us(1);
    lcd_write_byte(data | LCD_ENABLE);
    sleep_us(1);
    lcd_write_byte(data & ~LCD_ENABLE);
    sleep_us(100);
}

static void lcd_send(uint8_t data, uint8_t mode) {
    uint8_t high = (data & 0xF0) | LCD_BACKLIGHT | mode;
    uint8_t low  = ((data << 4) & 0xF0) | LCD_BACKLIGHT | mode;

    lcd_write_byte(high);
    lcd_toggle_enable(high);

    lcd_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_send_cmd(uint8_t cmd) {
    lcd_send(cmd, LCD_COMMAND);
}

void lcd_send_data(uint8_t data) {
    lcd_send(data, LCD_DATA);
}

void lcd_clear() {
    lcd_send_cmd(0x01);
    sleep_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_send_cmd(0x80 | (col + row_offsets[row]));
}

void lcd_print_char(char c) {
    lcd_send_data(static_cast<uint8_t>(c));
}

void lcd_print(const char* str) {
    while (*str) {
        lcd_print_char(*str++);
    }
}

void lcd_load_custom_char(uint8_t location, const uint8_t charmap[8]) {
    location &= 0x7;
    lcd_send_cmd(0x40 | (location << 3));
    for (int i = 0; i < 8; i++) {
        lcd_send_data(charmap[i]);
    }
}

void lcd_print_custom_char(uint8_t location) {
    lcd_send_data(location);
}
