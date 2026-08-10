#include "esp_err.h"
#include "esp_log.h"
#include "AlphaNum4.h"
#include "MAX6675.h"
#include "HT16K33.h"

static const char *TAG = "display_temperature";

esp_err_t display_temperature_c(AlphaNum4 &display, float temperature_c){

    if(temperature_c < 0 || temperature_c > 999){
        ESP_LOGE(TAG, "输入数据异常");
        return ESP_ERR_INVALID_ARG;
    }

    int temperature_read_c = (int)(temperature_c + 0.5f);
    char text[5];
    snprintf(text, sizeof(text), "%3dC", temperature_read_c);

    display.print(text);
    return display.writeDisplay();
}

void temperature_task(void *pvParameters){
    AlphaNum4 *display = static_cast<AlphaNum4 *>(pvParameters);
    TickType_t last_wake_time = xTaskGetTickCount();

    while(1){
        
    }
}