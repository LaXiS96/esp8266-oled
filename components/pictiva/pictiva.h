#pragma once

#include "esp_err.h"

esp_err_t pictiva_init(uint32_t rst_gpio, uint32_t dc_gpio);
esp_err_t pictiva_on();
esp_err_t pictiva_off();
