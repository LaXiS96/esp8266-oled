#include "pictiva.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi.h"

// TODO debugging
#include "esp_log.h"
static inline int32_t asm_ccount(void)
{
    int32_t r;
    asm volatile("rsr %0, ccount"
                 : "=r"(r));
    return r;
}

/// Gets the smallest multiple of stride that is bigger than val
#define ALIGN(val, stride) ((val) % (stride) != 0 ? (val) + 4 - ((val) % (stride)) : (val))

#define CMD_COLUMN_ADDRESS 0x15
#define CMD_ROW_ADDRESS 0x75
#define CMD_CONTRAST_A 0x81
#define CMD_CONTRAST_B 0x82
#define CMD_CONTRAST_C 0x83
#define CMD_MASTER_CURRENT 0x87
#define CMD_REMAP 0xA0
#define CMD_START_LINE 0xA1
#define CMD_ROW_OFFSET 0xA2
#define CMD_MODE_NORMAL 0xA4
#define CMD_MODE_ALL_ON 0xA5
#define CMD_MODE_ALL_OFF 0xA6
#define CMD_MODE_INVERSE 0xA7
#define CMD_MULTIPLEX_RATIO 0xA8
#define CMD_MASTER_CONFIG 0xAD
#define CMD_DISPLAY_OFF 0xAE
#define CMD_DISPLAY_ON 0xAF
#define CMD_NOP 0xE3

/// Number of actual command bytes
#define INIT_CMDS_LEN 9

// clang-format off

/// OLED initialization commands
static const uint8_t init_cmds[ALIGN(INIT_CMDS_LEN, 4)] = {
    CMD_MULTIPLEX_RATIO, 47,
    CMD_REMAP, 0b01110010, // 65K colors, enable COM split odd even, scan COM[MUX]->COM0, column address 95 to SEG0, horizontal address increment
    CMD_MASTER_CONFIG, 0b10001110, // Enable external VCC
    CMD_ROW_ADDRESS, 0, 47,
};
// clang-format on

typedef enum
{
    SEND_COMMAND = 0,
    SEND_DATA = 1,
} send_type_t;

static uint32_t _res_gpio;
static uint32_t _dc_gpio;
static uint8_t framebuf[48][288] = {0};

static esp_err_t spi_send(send_type_t type, uint32_t *data, uint32_t len)
{
    spi_trans_t trans;
    memset(&trans, 0, sizeof(trans));
    trans.bits.mosi = len * 8;
    trans.mosi = data;

    gpio_set_level(_dc_gpio, type);

    return spi_trans(HSPI_HOST, &trans);
}

static esp_err_t spi_send1(send_type_t type, uint8_t data)
{
    return spi_send(type, (uint32_t *)&data, 1);
}

static esp_err_t spi_send2(send_type_t type, uint8_t data1, uint8_t data2)
{
    return spi_send(type, (uint32_t *)&(uint8_t[]){data1, data2}, 2);
}

static esp_err_t spi_send3(send_type_t type, uint8_t data1, uint8_t data2, uint8_t data3)
{
    return spi_send(type, (uint32_t *)&(uint8_t[]){data1, data2, data3}, 3);
}

esp_err_t pictiva_init(uint32_t res_gpio, uint32_t dc_gpio)
{
    _res_gpio = res_gpio;
    _dc_gpio = dc_gpio;

    gpio_config_t io_config;
    io_config.intr_type = GPIO_INTR_DISABLE;
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pin_bit_mask = (1 << _res_gpio) | (1 << _dc_gpio);
    io_config.pull_down_en = 0;
    io_config.pull_up_en = 0;
    gpio_config(&io_config);

    // TODO do we initialize spi here?

    // Reset
    gpio_set_level(_res_gpio, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(_res_gpio, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    return spi_send(SEND_COMMAND, (uint32_t *)&init_cmds, INIT_CMDS_LEN);
}

esp_err_t pictiva_on()
{
    return spi_send1(SEND_COMMAND, CMD_DISPLAY_ON);
}

esp_err_t pictiva_off()
{
    return spi_send1(SEND_COMMAND, CMD_DISPLAY_OFF);
}

esp_err_t pictiva_set_pixel(uint32_t row, uint32_t col, uint8_t value)
{
    framebuf[row][col] = value;

    return ESP_OK; // TODO
}

esp_err_t pictiva_draw()
{
    uint32_t start_count;
    uint32_t end_count;

    start_count = asm_ccount();

    // Reset row and column address pointers
    spi_send3(SEND_COMMAND, CMD_ROW_ADDRESS, 0, 47);
    spi_send3(SEND_COMMAND, CMD_COLUMN_ADDRESS, 0, 95);

    // Map to 565 pixel data and send to display
    uint8_t *framebufptr = (uint8_t *)&framebuf;
    uint8_t *maxptr = framebufptr + 288 * 48;
    while (framebufptr < maxptr)
    {
        uint8_t buf[64] = {0}; // ESP8266 is limited to 64 bytes per SPI transfer
        for (uint32_t i = 0; i < sizeof(buf); i += 2)
        {
            buf[i] = ((*framebufptr & 0b11111) << 3) |
                     ((*(framebufptr + 1) & 0b111000) >> 3);
            buf[i + 1] = ((*(framebufptr + 1) & 0b111) << 5) |
                         (*(framebufptr + 2) & 0b11111);
            framebufptr += 3;
        }
        spi_send(SEND_DATA, (uint32_t *)&buf, 64);
    }

    end_count = asm_ccount();
    ESP_LOGI("pictiva", "start %d end %d diff %d", start_count, end_count, end_count - start_count);

    return ESP_OK; // TODO
}
