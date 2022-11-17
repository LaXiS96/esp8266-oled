# ESP8266 OLED

## Setup

- https://espressif-docs.readthedocs-hosted.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html

1. Extract MSYS2 zip to `C:\`
1. Extract contents of `xtensa-lx106-elf` folder from toolchain zip into `C:\msys32\mingw32\`
1. Launch MinGW32 shell using `C:\msys32\mingw32.exe`
1. Clone ESP8266_RTOS_SDK into `~/esp/ESP8266_RTOS_SDK`

## OSRAM Pictiva 3.3-inch 288x48 display

[Display datasheet](https://static6.arrow.com/aropdfconversion/93b1779f8e16e192f8fbe5a44f298b3b55de40bb/oled_288x48_3_3.pdf)

[SSD0332 controller datasheet](http://i-lcd.com/PDFs/SSD0332%20R1-0.pdf)

### Controller pin functions

|Pin   |I2C      |SPI
|-     |-        |-
|BS1   |1        |0
|SA0   |addr LSB |-
|D/C#  |-        |0=command, 1=data
|CS#   |-        |0=enabled
|D0    |SCL      |SCLK
|D1    |SDAin    |SDIN
|D2    |SDAout   |open
|D3-D7 |-        |ground
|E     |-        |ground
|R/W   |-        |ground

### Connections for SPI mode

SPI up to 4MHz

|Pin |Name  |To                             |ESP8266 (NodeMCU)
|-   |-     |-                              |-
|1   |VSS   |ground
|2   |-     |-
|3   |VDD   |3.3V
|4   |BS1   |ground
|5   |IREF  |~900kOhm to ground for VCC=12V
|6   |CS#   |SPI chip select                |HSPI CS GPIO15 (D8) or ground
|7   |RES#  |pullup 3.3V, ground to reset   |GPIO5 (D1) ?
|8   |D/C#  |data/command                   |GPIO4 (D2) ?
|9   |R/W#  |ground
|10  |E#    |ground
|11  |CLK   |SPI clock                      |HSPI CLK GPIO14 (D5)
|12  |D_IN  |SPI MOSI                       |HSPI MOSI GPIO13 (D7)
|13  |D_OUT |-
|14  |-     |-
|15  |VCOMH |capacitor to ground
|16  |VCL   |ground
|17  |VCC   |12V
|18  |-     |
|19  |-     |
|20  |-     |

### Controller details

`COM0` to `COM63`: common (row) drivers  
`SA0/SB0/SC0` to `SA95/SB95/SC95`: segment (column) drivers, 96x3=288

`Multiplex ratio` is set to 47 because we only use `COM0` to `COM47` (48 rows).
`COM Split Odd Even` is enabled because the row connections are interlaced.

Native display origin (COM0, SEG0) is at lower right; to move the origin to the top left:
- enable `COM Remap` (inverts row scanning)
- enable `Column Address Mapping` (inverts column scanning)

# References

- https://github.com/sumotoy/OLED_pictivaWide
