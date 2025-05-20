// #pragma once
// #include <stdint.h>

// void lcd_init(uint8_t i2c_addr, uint8_t sda_pin, uint8_t scl_pin);
// void lcd_clear();
// void lcd_set_cursor(uint8_t row, uint8_t col);
// void lcd_print_char(char c);
// void lcd_print(const char* str);
// void lcd_load_custom_char(uint8_t location, const uint8_t charmap[8]);
// void lcd_print_custom_char(uint8_t location);


// #pragma once
// #include <stdint.h>

// void lcd_init(uint8_t i2c_addr, uint8_t sda_pin, uint8_t scl_pin);
// void lcd_clear();
// void lcd_set_cursor(uint8_t row, uint8_t col);
// void lcd_print_char(char c);
// void lcd_print(const char* str);
// void lcd_load_custom_char(uint8_t location, const uint8_t charmap[8]);
// void lcd_print_custom_char(uint8_t location);

#pragma once
#include <cstdint>

void lcd_init(uint8_t i2c_addr, uint8_t sda_pin, uint8_t scl_pin);
void lcd_clear();
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print_char(char c);
void lcd_print(const char* str);
void lcd_load_custom_char(uint8_t location, const uint8_t charmap[8]);
void lcd_print_custom_char(uint8_t location);

// wewnętrzne
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);
