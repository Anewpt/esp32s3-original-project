#include <inttypes.h>

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

constexpr char kTag[] = "drink_dispenser";
constexpr TickType_t kHeartbeatInterval = pdMS_TO_TICKS(5000);

}  // namespace

extern "C" void app_main(void)
{
    esp_chip_info_t chip_info{};
    uint32_t flash_size = 0;

    esp_chip_info(&chip_info);

    if (esp_flash_get_size(nullptr, &flash_size) != ESP_OK) {
        ESP_LOGW(kTag, "无法读取 Flash 容量");
    }

    ESP_LOGI(kTag, "ESP32-S3 饮料机初始工程启动");
    ESP_LOGI(kTag,
             "CPU 核心数: %d, 芯片版本: %d, Flash: %" PRIu32 " MB",
             chip_info.cores,
             chip_info.revision,
             flash_size / (1024U * 1024U));

    uint32_t heartbeat_count = 0;
    while (true) {
        ESP_LOGI(kTag, "系统运行正常，心跳计数: %" PRIu32, ++heartbeat_count);
        vTaskDelay(kHeartbeatInterval);
    }
}
