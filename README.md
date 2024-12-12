# Snake Game
## Introduction
This is Snake Game project to learn Linux driver. I use beagle bone black, oled 128x64.

## Hardware 
| ID  | OLED              |   PIN  | 
| --- | ----------------- | ------ | 
| 1   | SSD1306_SCL       | P9_19  | 
| 2   | SSD1306_SDA       | P9_20  | 
| 3   | SSD1306_GND       | P9_01  | 
| 4   | SSD1306_VCC       | P9_07  | 
| 5   | Up Button         | P8_13  | 
| 6   | Down Button       | P8_10  | 
| 7   | Left Button       | P8_12  | 
| 8   | Right Button      | P8_11  | 
| 9   | Right Button      | P8_09  | 

## How to use

On ubuntu:
- cd button_driver and execute make to contruct the button_driver.ko
- cd oled_driver and execute make to contruct the oled_driver.ko

- make sta_all to build the file execute in folder bin 

On beaglebone black:

- sudo insmod button_driver and oled_driver
- sudo chmod 0666 my_button_driver and my_ssd1306
- ./main_static to execute program.