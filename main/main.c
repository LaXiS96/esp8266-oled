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
    pictiva_on();

    // send_command(&(uint32_t){CMD_MODE_ALL_ON}, 1);

    // uint32_t off = 0;
    // for (uint32_t i = 0; i < 96 * 48; i++)
    // {
    //     // if (i == 0)
    //     //     send_data(&(uint32_t){0b11111000 | (0b00011111 << 8)}, 2);
    //     // else if (i == 96 * 48 - 1)
    //     //     send_data(&(uint32_t){0b00000111 | (0b11111111 << 8)}, 2);
    //     // else
    //     //     send_data(&(uint32_t){0}, 2);

    //     if (i == (1 << off) || i == (96 * 48 - 1))
    //     {
    //         send_data(&(uint32_t){0b11111000 | (0b00011111 << 8)}, 2);
    //         off++;
    //     }
    //     else
    //         send_data(&(uint32_t){0}, 2);
    // }

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI("app", "done");
}
