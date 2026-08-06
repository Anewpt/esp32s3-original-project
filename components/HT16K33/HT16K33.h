#ifndef HT16K33_H
#define HT16K33_H

#include "esp_err.h"
#include "driver/gpio.h"

#define HT16K33_SDA_GPIO GPIO_NUM_4
#define HT16K33_SCL_GPIO GPIO_NUM_5


 #ifdef __cplusplus
  extern "C" {
  #endif

      esp_err_t HT16K33_init(void);
      void i2c_scan_f(i2c_port_t i2c_port);

#ifdef __cplusplus
  }
#endif





#endif