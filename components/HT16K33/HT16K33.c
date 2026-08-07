#include "HT16K33.h"
#include "driver/i2c.h"
#include "esp_log.h"

static const char *i2c_scan_TAG = "I2C_SCAN";

static void i2c_scan_f(i2c_port_t i2c_port);

esp_err_t HT16K33_init(void){

    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = HT16K33_SDA_GPIO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = HT16K33_SCL_GPIO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;

    esp_err_t err = i2c_param_config(I2C_NUM_0, &conf);
    if(err != ESP_OK){
        ESP_LOGI("HT16K33", "I2C总线初始化失败");
        return err;
    }

    err = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    if(err != ESP_OK){
        ESP_LOGI("HT16K33", "I2C驱动安装失败");
        return err;
    }

    i2c_scan_f(I2C_NUM_0);

    return ESP_OK;
}

static void i2c_scan_f(i2c_port_t i2c_port){

    int device_count = 0;


    ESP_LOGI(i2c_scan_TAG, "开始扫描");

    for (uint8_t address = 0x08; address <= 0x77; address++){

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();

        if(cmd == NULL){
            ESP_LOGI(i2c_scan_TAG, "创建I2C命令失败");
            return;
        }

        i2c_master_start(cmd);

        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);

        i2c_master_stop(cmd);

        esp_err_t err = i2c_master_cmd_begin(i2c_port, cmd, pdMS_TO_TICKS(20));

        i2c_cmd_link_delete(cmd);

        if(err == ESP_OK){
            ESP_LOGI(i2c_scan_TAG, "发现I2C设备：0x%02x", (unsigned int)address);
            device_count++;
        }
    }

    if(device_count == 0){
        ESP_LOGW(i2c_scan_TAG, "没有发现任何I2C设备");
    }else{
        ESP_LOGI(i2c_scan_TAG, "扫描完成，共发%d个设备", device_count);
    }
}