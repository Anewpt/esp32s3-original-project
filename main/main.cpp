#include <inttypes.h>

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MAX6675.h"
#include "HT16K33.h"
#include "AlphaNum4.h"
#include "temperature_display.h"



extern "C" void app_main(void)
{
  ESP_ERROR_CHECK(max6675_init());
  ESP_ERROR_CHECK(i2c_bus_init());

  
  /* display.print("180C");
  ESP_ERROR_CHECK(display.writeDisplay()); */
  BaseType_t tem = xTaskCreate(temperature_task, "temperature_task", 2048, NULL, 2, NULL);
  if(tem != pdPASS){
    ESP_LOGI("APP", "温度任务创建失败");
  }
}
