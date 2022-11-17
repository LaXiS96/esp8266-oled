#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi.h"

#define RES_GPIO 5
#define DC_GPIO 4

#define CMD_COLUMN_ADDRESS 0x15U
#define CMD_ROW_ADDRESS 0x75U
#define CMD_REMAP 0xA0U
#define CMD_ROW_OFFSET 0xA2U
#define CMD_MODE_NORMAL 0xA4U
#define CMD_MODE_ALL_ON 0xA5U
#define CMD_MODE_ALL_OFF 0xA6U
#define CMD_MODE_INVERSE 0xA7U
#define CMD_MULTIPLEX_RATIO 0xA8U
#define CMD_MASTER_CONFIG 0xADU
#define CMD_DISPLAY_OFF 0xAEU
#define CMD_DISPLAY_ON 0xAFU

esp_err_t spi_transmit(uint32_t *data, uint32_t len)
{
    spi_trans_t trans;
    memset(&trans, 0, sizeof(trans));
    trans.bits.mosi = len * 8;
    trans.mosi = data;

    return spi_trans(HSPI_HOST, &trans);
}

void send_command(uint32_t *cmd, uint32_t len)
{
    gpio_set_level(DC_GPIO, 0);
    spi_transmit(cmd, len);
    // TODO err
}

void send_data(uint32_t *cmd, uint32_t len)
{
    gpio_set_level(DC_GPIO, 1);
    spi_transmit(cmd, len);
    // TODO err
}

void app_main()
{
    ESP_LOGI("app", "start");

    gpio_config_t ioConfig;
    ioConfig.intr_type = GPIO_INTR_DISABLE;
    ioConfig.mode = GPIO_MODE_OUTPUT;
    ioConfig.pin_bit_mask = (1 << RES_GPIO) | (1 << DC_GPIO);
    ioConfig.pull_down_en = 0;
    ioConfig.pull_up_en = 0;
    gpio_config(&ioConfig);

    spi_config_t spiConfig;
    spiConfig.clk_div = SPI_2MHz_DIV;
    spiConfig.event_cb = NULL;
    spiConfig.interface.val = SPI_DEFAULT_INTERFACE;
    spiConfig.interface.cs_en = 0;
    spiConfig.interface.miso_en = 0;
    spiConfig.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
    spiConfig.mode = SPI_MASTER_MODE;
    spi_init(HSPI_HOST, &spiConfig);

    // Reset
    gpio_set_level(RES_GPIO, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(RES_GPIO, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // TODO convert to byte array and send at once

    // Set multiplex ratio (row count - 1)
    send_command(&(uint32_t){CMD_MULTIPLEX_RATIO | (47 << 8)}, 2);

    // Set: 65K colors, enable COM split odd even, scan COM[MUX]->COM0, column address 95 to SEG0, horizontal address increment
    send_command(&(uint32_t){CMD_REMAP | (0b01110010 << 8)}, 2);

    // Enable external VCC
    send_command(&(uint32_t){CMD_MASTER_CONFIG | (0b10001110 << 8)}, 2);

    // Set row addresses to start at 0 and end at 47
    send_command(&(uint32_t){CMD_ROW_ADDRESS | (0 << 8) | (47 << 16)}, 3);

    // Turn display ON
    send_command(&(uint32_t){CMD_DISPLAY_ON}, 1);

    // send_command(&(uint32_t){CMD_MODE_ALL_ON}, 1);

    uint32_t off = 0;
    for (uint32_t i = 0; i < 96 * 48; i++)
    {
        // if (i == 0)
        //     send_data(&(uint32_t){0b11111000 | (0b00011111 << 8)}, 2);
        // else if (i == 96 * 48 - 1)
        //     send_data(&(uint32_t){0b00000111 | (0b11111111 << 8)}, 2);
        // else
        //     send_data(&(uint32_t){0}, 2);

        if (i == (1 << off) || i == (96 * 48 - 1))
        {
            send_data(&(uint32_t){0b11111000 | (0b00011111 << 8)}, 2);
            off++;
        }
        else
            send_data(&(uint32_t){0}, 2);
    }

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI("app", "done");
}
