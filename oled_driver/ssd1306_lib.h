#ifndef SSD1306_LIB_H
#define SSD1306_LIB_H

#include <linux/kernel.h>        // Contains basic kernel definitions
#include <linux/module.h>       
#include <linux/device.h>        
#include <linux/cdev.h>          // For managing character devices
#include <linux/slab.h>          // For dynamic memory allocation
#include <linux/fs.h>            // For file system operations
#include <linux/of.h>            // For Device Tree support
#include <linux/of_device.h>     // For managing devices from Device Tree
#include <linux/i2c.h>           // Provides I2C communication functions
#include <linux/uaccess.h>       // For user-space and kernel-space data transfer
#include <linux/errno.h>        
#include <linux/delay.h>         // For introducing delays in kernel

// SSD1306 screen dimensions
#define SSD1306_MAX_SEG 128       // Maximum number of columns on the screen
#define SSD1306_MAX_LINE 7        // Maximum number of lines on the screen
#define SSD1306_DEF_FONT_SIZE 5   // Default font size

// Structure representing the SSD1306 I2C module
struct ssd1306_i2c_module {
    struct i2c_client *client;    // I2C client for communication with SSD1306
    uint8_t line_num;             // Current line number
    uint8_t cursor_position;      // Current cursor position
    uint8_t font_size;            // Font size in use
};

// Function to send data over I2C
int ssd1306_i2c_send(struct ssd1306_i2c_module *module, char *buff, int len);

// Function to write command or data to SSD1306
void ssd1306_write_command(struct ssd1306_i2c_module *module, bool check, char data);

// Function to set the cursor position on the SSD1306 screen
void ssd1306_set_cursor(struct ssd1306_i2c_module *module, uint8_t line_num, uint8_t cursor_position);

// Function to move the cursor to the next line
void ssd1306_goto_next_line(struct ssd1306_i2c_module *module);

// Function to convert a character to its corresponding font index
int convert(char c);

// Function to print a character to the screen
void ssd1306_print_char(struct ssd1306_i2c_module *module, unsigned char c);

// Function to print a string to the screen
void ssd1306_print_string(struct ssd1306_i2c_module *module, unsigned char *str);

// Function to set the brightness of the SSD1306 screen
void ssd1306_set_brightness(struct ssd1306_i2c_module *module, uint8_t brightness);

// Function to clear a specific line on the screen
void ssd1306_clear_page(struct ssd1306_i2c_module *module, uint8_t line);

// Function to clear the entire screen
void ssd1306_clear_full(struct ssd1306_i2c_module *module);

// Function to initialize the SSD1306 display
int ssd1306_display_init(struct ssd1306_i2c_module *module);

#endif
