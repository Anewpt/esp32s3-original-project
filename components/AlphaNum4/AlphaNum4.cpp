#include "AlphaNum4.h"
#include "HT16K33.h"

static const *Alpha_TAG = "AlphaNum4";

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

    if(data == nullptr || length){
        return ESP_ERR_INVALID_ARG;
    }

    return i2c_master_writer_write_to_device(_i2c_port, _i2c_address, data, length, pdMS_TO_TICKS(100));
}

esp_err_t begin(i2c_port_t i2c_port, uint8_t address){

    if(address > 0x7F){
        ESP_LOGE(Alpha_TAG, "输入地址异常");
        return ESP_ERR_INVALID_ARG;
    }

    _i2c_port = i2c_port;
    _i2c_address = address;
    _initialized = false;

    uint8_t init_cmd = 0x21;
    esp_err_t err = i2c_master_write_to_device(i2c_port, address, &init_cmd, pdMS_TO_TICKS(100));
    if(err != ESP_OK){
        ESP_LOGE(Alpha_TAG, "内部振荡器启动失败");
        returen err;
    }

    init_cmd = 0x81;
    esp_err_t err = i2c_master_write_to_device(i2c_port, address, &init_cmd, pdMS_TO_TICKS(100));
    if(err != ESP_OK){
        ESP_LOGE(Alpha_TAG, "显示开启失败");
        return err;
    }

    init_cmd = 0xEF;
    esp_err_t err = i2c_master_write_to_device(i2c_port, address, &init_cmd, pdMS_TO_TICKS(100));
    if(err != ESP_OK){
        ESP_LOGE(Alpha_TAG, "最大亮度设置失败");
        return err;
    }

    if(err != ESP_OK){
        _initialized = false;
        return err;
    }

    _initialized = true;

    return ESP_OK;
}

