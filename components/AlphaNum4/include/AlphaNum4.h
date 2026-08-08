#pragma once
 
#include <stddef.h>
#include <stdint.h>
 
#include "esp_err.h"
#include "driver/i2c.h"
 
class AlphaNum4
{
public:
    AlphaNum4();
 
    esp_err_t begin(i2c_port_t i2c_port, uint8_t address);
 
    esp_err_t setBrightness(uint8_t brightness);
    esp_err_t setBlinkRate(uint8_t blink_rate);
 
    void clear();
 
    void writeDigitRaw(uint8_t position, uint16_t segment_bits);
 
    bool writeDigitAscii(
        uint8_t position,
        char character,
        bool decimal_point = false
    );
 
    void print(const char *text);
    esp_err_t writeDisplay();
 
private:
    esp_err_t i2cWrite(
        const uint8_t *data,
        size_t length
    );
 
    static uint16_t encodeCharacter(char character);
 
    i2c_port_t _i2c_port;
    uint8_t _i2c_address;
    bool _initialized;
    uint16_t _display_buffer[8];
};