#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi.h"

#include "pictiva.h"

#define RES_GPIO 5
#define DC_GPIO 4

void draw_task()
{
    const TickType_t wait_ticks = 100 / portTICK_PERIOD_MS;
    TickType_t last_wake_ticks = xTaskGetTickCount();

    uint32_t count = 0;
    char *str = malloc(64);
    for (;;)
    {
        vTaskDelayUntil(&last_wake_ticks, wait_ticks);

        snprintf(str, 64, "%u", count);
        pictiva_text(str);
        if (pictiva_draw_pending)
            pictiva_draw();

        count++;
    }
}

void app_main()
{
    ESP_LOGI("app", "start");

    spi_config_t spi_config;
    spi_config.clk_div = SPI_10MHz_DIV;
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

    // char *str = malloc(256);
    // if (str == NULL)
    //     ESP_LOGE("main", "str is null");

    // for (uint32_t i = 0; i < 255; i++)
    //     *(str + i) = i + 1;
    // *(str + 255) = '\0';

    // pictiva_text(str);
    // pictiva_draw();

    // char *str = malloc(64);
    // while (1)
    // {
    //     // 4MHz -> 19.94ms
    //     // 5MHz -> 16.2ms
    //     // 8MHz -> 10.55ms
    //     // 10MHz -> 8.67ms
    //     // 16MHz -> 5.89ms
    //     // 20MHz -> 5.24ms WHY?
    //     // 40MHz -> 5.29ms WHY?
    //     // 80MHz -> ...

    //     // uint32_t before = xthal_get_ccount();
    //     pictiva_draw();
    //     // uint32_t after = xthal_get_ccount();
    //     // uint32_t diff = (after > before) ? (after - before) : (UINT32_MAX - before + after);

    //     // snprintf(str, 64, "%u %u %u", before, after, diff);
    //     pictiva_text(str);

    //     vTaskDelay(500 / portTICK_PERIOD_MS);
    // }

    TaskHandle_t draw_handle = NULL;
    xTaskCreate(draw_task, "draw", configMINIMAL_STACK_SIZE, NULL, 14, &draw_handle);

    ESP_LOGI("app", "done");
}
