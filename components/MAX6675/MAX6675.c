#include "MAX6675.h"
#include "driver/spi_master.h"
#include "esp_log.h"

QueueHandle_t temperature_queue;
uint16_t max6675_temperature_read;

static spi_device_handle_t max6675_handle = NULL;

typedef struct{
  float temperatue_c;
  esp_err_t status;
} temperature_sample_t;



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

  temperature_queue = xQueueCreate(1, sizeof(temperature_sample_t));

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
      //ESP_LOGE("MAX6675", "热电偶未连接");
      return ESP_ERR_INVALID_STATE;
    }

    uint16_t code = (raw >> 3) & 0x0FFF;
    *temperature_c = code * 0.25f;

    //ESP_LOGI("MAX6675", "raw=0x%04X, code=%u, temperature=%.2f C", raw, code, *temperature_c);
    
    return ESP_OK;
}

/* void temperature_task(void *pvParameters){
    while(1){

      float temperature = 0.0f;
      TickType_t last_wake_time = xTaskGetTickCount();

      esp_err_t err = max6675_read(&temperature);

      if(err == ESP_OK){
        ESP_LOGI("APP", "teperature=%.2f C", temperature);
      }
      else if(err == ESP_ERR_INVALID_STATE){
        ESP_LOGI("APP", "热电偶断开");
      }
      else{
        ESP_LOGI("APP", "通讯异常");
      }

      //vTaskDelay(pdMS_TO_TICKS(500));
      xTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1000));
    }
} */

void temperature_write(void){
  static float tem_temperatue = 0.0f;
  temperature_sample_t input_sample;
  if(max6675_read(&tem_temperatue) == ESP_OK){
    input_sample.temperatue_c = (tem_temperatue + 0.5f);
    input_sample.status = ESP_OK;

    xQueueOverwrite(temperature_queue, &input_sample);
  }
}

/* uint16_t temperature_read(void){
  temperature_sample_t out_sample;
  if(xQueueReceive(temperature_queue, &out_sample, portMAX_DELAY) == pdTRUE){
    if(out_sample.status == ESP_OK){
      return out_sample.temperatue_c;
    }
  }

  return 0;
} */

esp_err_t temperature_read(float *temperature_c){

  temperature_sample_t out_sample;

  if(temperature_c == NULL){
    return ESP_ERR_INVALID_ARG;
  }

  if(xQueueReceive(temperature_queue, &out_sample, portMAX_DELAY) != pdTRUE){
    return ESP_FAIL;
  }

  if(out_sample.status != ESP_OK){
    return out_sample.status;
  }

  *temperature_c = out_sample.temperatue_c;

  return ESP_OK;
}