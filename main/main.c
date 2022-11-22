#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi.h"

#include "pictiva.h"

#define RES_GPIO 5
#define DC_GPIO 4

void app_main()
{
    ESP_LOGI("app", "start");

    spi_config_t spi_config;
    spi_config.clk_div = SPI_4MHz_DIV;
    spi_config.event_cb = NULL;
    spi_config.interface.val = SPI_DEFAULT_INTERFACE;
    spi_config.interface.cs_en = 0;
    spi_config.interface.miso_en = 0;
    spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
    spi_config.mode = SPI_MASTER_MODE;
    spi_init(HSPI_HOST, &spi_config);

    pictiva_init(RES_GPIO, DC_GPIO);
    pictiva_set_brightness(3);
    pictiva_on();
    // pictiva_set_pixel(0, 0, 255);
    // pictiva_set_pixel(0, 143, 255);
    // pictiva_set_pixel(0, 283, 255);
    // pictiva_set_pixel(1, 0, 255);
    // pictiva_set_pixel(47, 287, 255);

    char *str = malloc(256);
    if (str == NULL)
        ESP_LOGE("main", "str is null");

    for (uint32_t i = 0; i < 255; i++)
        *(str + i) = i + 1;
    *(str + 255) = '\0';

    pictiva_text(str);
    pictiva_draw();

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI("app", "done");
}
