#include <inttypes.h>

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MAX6675.h"
#include "HT16K33.h"



extern "C" void app_main(void)
{
  ESP_ERROR_CHECK(max6675_init());

      /* while (true) {
          float temperature;

          esp_err_t err = max6675_read(&temperature);
          if (err == ESP_OK) {
              ESP_LOGI("APP", "温度：%.2f°C", temperature);
          }

          vTaskDelay(pdMS_TO_TICKS(500));
        } */
  BaseType_t tem = xTaskCreate(temperature_task, "temperature_task", 2048, NULL, 2, NULL);
  if(tem != pdPASS){
    ESP_LOGI("APP", "温度任务创建失败");
  }

  ESP_ERROR_CHECK(HT16K33_init());
}
