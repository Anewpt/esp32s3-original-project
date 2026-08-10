#include "AlphaNum4.h"
#include "esp_log.h"
#include <cstring>

static const char *Alpha_TAG = "AlphaNum4";

AlphaNum4::AlphaNum4()   //C++类初始值，创建类时自动初始化
    : _i2c_port(I2C_NUM_0),
      _i2c_address(0x70),
      _initialized(false)
{
    clear();
}


void AlphaNum4::clear(){
    memset(_display_buffer, 0, sizeof(_display_buffer));
}

esp_err_t AlphaNum4::i2cWrite(const uint8_t *data, size_t length){

    if(data == nullptr || length == 0){
        return ESP_ERR_INVALID_ARG;
    }

    return i2c_master_write_to_device(_i2c_port, _i2c_address, data, length, pdMS_TO_TICKS(100));
}

esp_err_t AlphaNum4::begin(i2c_port_t i2c_port, uint8_t address){

    if(address > 0x7F){
        ESP_LOGE(Alpha_TAG, "输入地址异常");
        return ESP_ERR_INVALID_ARG;
    }

    _i2c_port = i2c_port;
    _i2c_address = address;
    _initialized = false;

    uint8_t init_cmd = 0x21;
    esp_err_t err = i2c_master_write_to_device(i2c_port, address, &init_cmd, 1,pdMS_TO_TICKS(100));
    if(err != ESP_OK){
        ESP_LOGE(Alpha_TAG, "内部振荡器启动失败");
        return err;
    }

    init_cmd = 0x81;
    err = i2c_master_write_to_device(i2c_port, address, &init_cmd, 1, pdMS_TO_TICKS(100));
    if(err != ESP_OK){
        ESP_LOGE(Alpha_TAG, "显示开启失败");
        return err;
    }

    init_cmd = 0xEF;
    err = i2c_master_write_to_device(i2c_port, address, &init_cmd, 1, pdMS_TO_TICKS(100));
    if(err != ESP_OK){
        ESP_LOGE(Alpha_TAG, "最大亮度设置失败");
        return err;
    }


    _initialized = true;

    return ESP_OK;
}


esp_err_t AlphaNum4::setBrightness(uint8_t brightness){

    if(brightness > 15){
        ESP_LOGE(Alpha_TAG,"亮度数据异常");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t cmd = 0xE0 | brightness;
    return i2cWrite(&cmd, sizeof(cmd));
}

esp_err_t AlphaNum4::setBlinkRate(uint8_t blink_rate){

    if(blink_rate > 3){
        ESP_LOGE(Alpha_TAG, "闪烁率数据异常");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t cmd = 0x80 | 0x01 | (blink_rate << 1);

    return i2cWrite(&cmd, sizeof(cmd));
}

uint16_t AlphaNum4::encodeCharacter(char character){
    switch(character){
        case ' ': return 0x0000;
        case '0': return 0x0C3F;
        case '1': return 0x0006;
        case '2': return 0x00DB;
        case '3': return 0x00CF;
        case '4': return 0x00E6;
        case '5': return 0x00ED;
        case '6': return 0x00FD;
        case '7': return 0x0007;
        case '8': return 0x00FF;
        case '9': return 0x00EF;
        case 'C': return 0x0039;
        case 'E': return 0x00F9;
        case 'O': return 0x003F;
        case 'P': return 0x00F3;
        case 'R': return 0x20F3;
        case 'N': return 0x2136;
        default:  return 0x0000;

    }
}

void AlphaNum4::writeDigitRaw(uint8_t position, uint16_t segment_bits){
    if(position >= 4){
        ESP_LOGE(Alpha_TAG, "位置数据异常");
        return;
    }

    _display_buffer[position] = segment_bits;
}

bool AlphaNum4::writeDigitAscii(uint8_t position, char character, bool decimal_point){
    if(position >= 4){
        ESP_LOGE(Alpha_TAG, "位置数据异常");
        return false;
    }

    uint16_t segment_bits = encodeCharacter(character);

    if(decimal_point){
        segment_bits |= (1U << 14);
    }

    _display_buffer[position] = segment_bits;

    return true;
}

void AlphaNum4::print(const char *text){

    clear();

    if(text == nullptr){
        ESP_LOGE(Alpha_TAG, "输入文本为空");
        return;
    }

    for(uint8_t position = 0; position < 4; position++){

        if(text[position] == '\0'){
            break;
        }
        writeDigitAscii(position, text[position], false);
    }
}

esp_err_t AlphaNum4::writeDisplay(){
    if(!_initialized){
        ESP_LOGE(Alpha_TAG, "设备未初始化");
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t output[17] = {0};
    output[0] = 0x00;

    for(uint8_t i = 0; i < 8; i++){
        output[1 + i * 2] = _display_buffer[i] & 0xFF;
        output[2 + i * 2] = (_display_buffer[i] >> 8) & 0xFF;
    }

    return i2cWrite(output, sizeof(output));
}















































