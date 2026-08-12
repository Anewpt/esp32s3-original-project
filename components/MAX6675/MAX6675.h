#ifndef MAX6675_H
  #define MAX6675_H

  #include "esp_err.h"
  #include "driver/gpio.h"
  #include "driver/spi_master.h"

  #define MAX6675_MISO_GPIO GPIO_NUM_38
  #define MAX6675_CS_GPIO   GPIO_NUM_35
  #define MAX6675_SCK_GPIO  GPIO_NUM_36

  #ifdef __cplusplus
  extern "C" {
  #endif

  esp_err_t max6675_init(void);
  esp_err_t max6675_read(float *temperature_c);
  esp_err_t temperature_read(float *temperature_c);
  void temperature_write(void);

#ifdef __cplusplus
  }
  #endif

  #endif