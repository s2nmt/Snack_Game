
#ifndef BUTTON_H
#define BUTTON_H

/* Standard C libraries */
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

//Linux Libraries
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// Define the path to the button device file for handling button inputs.
#define SSD1306_BUTTON_FILE "/dev/my_button_snake"

// Define constants representing directional buttons and the ENTER button.
#define UP          1
#define LEFT        2
#define RIGHT       3
#define DOWN        4
#define ENTER       5

// Function to open the button device file.
int Button_OpenDevFile();

// Function to read data from the button device file.
int Button_Read(int fdt, char *buff, size_t size);

// Function to check if any button is pressed.
int Button_CheckAnyPress(int fd, char *buff, size_t size);

// Function to process and identify which button was pressed from the buffer.
int Button_Press(char *buff);

// Function to wait until any button is pressed.
int Button_WaitForAnyKey(int fd, char *buff, size_t size);

// Function to convert pressed keys into the corresponding direction (up, down, left, right, or enter).
int Button_KeysPressedToDirection(int fd, char *buff, size_t size, int direction);

#endif
