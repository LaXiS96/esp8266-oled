# ESP8266 OLED

## Setup

- https://espressif-docs.readthedocs-hosted.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html

1. Extract MSYS2 zip to `C:\`
1. Extract contents of `xtensa-lx106-elf` folder from toolchain zip into `C:\msys32\mingw32\`
1. Launch MinGW32 shell using `C:\msys32\mingw32.exe`
1. Clone ESP8266_RTOS_SDK into `~/esp/ESP8266_RTOS_SDK`

With ESP8266_RTOS_SDK v3 and above it should be possible to use the ESP-IDF framework for building and everything else (CMake, debugging?), but I have yet to try.

PlatformIO would be an obvious choice but it still uses an old and outdated SDK.

## OSRAM Pictiva 3.3-inch 288x48 display

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

Controller datasheet timing diagrams specify a minimum clock period of 250ns, which translates to a maximum clock speed of 4MHz.

|Pin |Name  |To                                  |ESP8266 (NodeMCU DevKit)
|-   |-     |-                                   |-
|1   |VSS   |ground
|2   |-     |-
|3   |VDD   |3.3V
|4   |BS1   |ground
|5   |IREF  |~900KOhm to ground for VCC=12V
|6   |CS#   |SPI chip select                     |D8 (GPIO15 / HSPI CS) or ground if it's the only device
|7   |RES#  |10kOhm pullup to 3.3V, low to reset |D1 (GPIO5)
|8   |D/C#  |data/command                        |D2 (GPIO4)
|9   |R/W#  |ground
|10  |E#    |ground
|11  |CLK   |SPI clock                           |D5 (GPIO14 / HSPI CLK)
|12  |D_IN  |SPI MOSI                            |D7 (GPIO13 / HSPI MOSI)
|13  |D_OUT |-
|14  |-     |-
|15  |VCOMH |100nF ceramic capacitor to ground
|16  |VCL   |ground
|17  |VCC   |12V
|18  |-     |
|19  |-     |
|20  |-     |

### Controller/display details

`COM0` to `COM63`: common (row) drivers, 64 in total but this display only uses 48.  
`SEG0` (`SA0/SB0/SC0`) to `SEG95` (`SA95/SB95/SC95`): segment (column) drivers, 3 subpixels * 96 columns = 288, this display maps each subpixel to a single physical column.

`Multiplex ratio` is set to 47 because we only use `COM0` to `COM47` (48 rows).  
`COM Split Odd Even` is enabled because the row connections are interlaced.

Native display origin (COM0, SEG0) is at lower right; to move the origin to the top left:
- enable `COM Remap` (inverts row scanning)
- enable `Column Address Mapping` (inverts column scanning)

## References

- [Display datasheet](https://static6.arrow.com/aropdfconversion/93b1779f8e16e192f8fbe5a44f298b3b55de40bb/oled_288x48_3_3.pdf)
- [SSD0332 controller datasheet](http://i-lcd.com/PDFs/SSD0332%20R1-0.pdf)
- Similar project: https://github.com/sumotoy/OLED_pictivaWide
