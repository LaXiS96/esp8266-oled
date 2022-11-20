#pragma once

#include "esp_err.h"

esp_err_t pictiva_init(uint32_t rst_gpio, uint32_t dc_gpio);
esp_err_t pictiva_on();
esp_err_t pictiva_off();
esp_err_t pictiva_draw();
esp_err_t pictiva_set_pixel(uint32_t row, uint32_t col, uint8_t value);
