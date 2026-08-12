#ifndef temperature_display_H
#define temperature_display_H

#include "esp_err.h"

esp_err_t display_temperature_c(AlphaNum4 &display, float temperature_c);
void temperature_task(void *pvParameters);

#endif