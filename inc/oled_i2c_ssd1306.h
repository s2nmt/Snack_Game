
#ifndef SSD1306_H
#define SSD1306_H

// Standard include guard for compatibility, ensuring the header file is included only once.
#include "button.h"  // Include the header file for button-related functionality, allowing interaction with button inputs.


// Function declarations for interacting with the OLED display:

// Opens the device file for the OLED display.
int OLED_OpenDevFile();

// Sets the cursor position on the OLED display at the specified coordinates (x, y).
void OLED_SetCursor(int fd, int x, int y);

// Displays a string (`str`) on the OLED screen at the current cursor position.
void OLED_Display(int fd, char *str);

// Clears the OLED display screen.
void OLED_Clear(int fd);

#endif
