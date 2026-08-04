#include "MAX6675.h"
#include "driver/spi_master.h"
#include "esp_log.h"




static spi_device_handle_t max6675_handle = NULL;

esp_err_t max6675_init(void){  //需要返回esp_err_t事件

    spi_bus_config_t bus_config = {  //总线配置
      .mosi_io_num = -1,
      .miso_io_num = MAX6675_MISO_GPIO,
      .sclk_io_num = MAX6675_SCK_GPIO,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 2,
    };

    spi_device_interface_config_t device_config = { //将max6675添加为SPI设备
      .mode = 0,
      .clock_speed_hz = 1000 * 1000,
      .spics_io_num = MAX6675_CS_GPIO,
      .queue_size = 1,
    };

    esp_err_t err = spi_bus_initialize(
      SPI3_HOST,
      &bus_config,
      SPI_DMA_CH_AUTO
  );

  if (err != ESP_OK) {
      ESP_LOGE("MAX6675",
               "SPI总线初始化失败: %s",
               esp_err_to_name(err));
      return err;
  }

  err = spi_bus_add_device(
      SPI3_HOST,
      &device_config,
      &max6675_handle
  );

  if (err != ESP_OK) {
      ESP_LOGE("MAX6675",
               "添加SPI设备失败: %s",
               esp_err_to_name(err));
      return err;
  }

  return ESP_OK;


}

esp_err_t max6675_read(float *temperature_c){

    spi_transaction_t transaction = {
      .flags = SPI_TRANS_USE_RXDATA,
      .length = 16,
      .rxlength = 16,
    };

    esp_err_t err = spi_device_polling_transmit(max6675_handle, &transaction);

    if (err != ESP_OK) {
      return err;
    }

    uint16_t raw = ((uint16_t)transaction.rx_data[0] << 8) | transaction.rx_data[1];

    if (raw & (1U << 2)) {
      ESP_LOGE("MAX6675", "热电偶未连接");
      return ESP_ERR_INVALID_STATE;
    }

    uint16_t code = (raw >> 3) & 0x0FFF;
    *temperature_c = code * 0.25f;

    ESP_LOGI("MAX6675", "raw=0x%04X, code=%u, temperature=%.2f C", raw, code, *temperature_c);
    
    return ESP_OK;
}

