#pragma once

#include <stdbool.h>

#include "esp_err.h"

/// Whether the framebuffer contents have been modified since last draw
static bool pictiva_draw_pending = true;

/// Draw internal framebuffer to the display
esp_err_t pictiva_draw();

/// Initialize display
esp_err_t pictiva_init(uint32_t rst_gpio, uint32_t dc_gpio);

/// Turn display OFF
esp_err_t pictiva_off();

/// Turn display ON
esp_err_t pictiva_on();

/// Set display brightness between 0 and 15, where 15 is full brightness
esp_err_t pictiva_set_brightness(uint32_t value);

esp_err_t pictiva_set_pixel(uint32_t row, uint32_t col, uint8_t value);

void pictiva_text(const char *str);
