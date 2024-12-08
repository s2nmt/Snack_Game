#include "oled_i2c_ssd1306.h"

#define SSD1306_DEV_FILE    "/dev/my_ssd1306_device"  // Device file path for SSD1306 OLED

/*
 * Function: OLED_OpenDevFile
 * --------------------------
 * Opens the device file for the SSD1306 OLED display.
 *
 * returns: the file descriptor (fd) for the opened device file.
 * If the file cannot be opened, the function prints an error message and exits the program.
 */
int OLED_OpenDevFile(){
    int fd = open(SSD1306_DEV_FILE, O_WRONLY);  // Open the device file in write-only mode

    if (fd == -1) {  // If the file cannot be opened
        printf("Open /dev/my_ssd1306 failed. Please check the dev folder\n");
        exit(EXIT_FAILURE);  // Exit the program if the file cannot be opened
    }
    return fd;  // Return the file descriptor if successful
}

/*
 * Function: OLED_SetCursor
 * ------------------------
 * Sets the cursor position on the OLED display.
 *
 * fd: the file descriptor of the opened device file.
 * x: the column position on the OLED display.
 * y: the row (line) position on the OLED display.
 *
 * This function sends the command "cursor x y" to the OLED device to set the cursor at the specified position.
 */
void OLED_SetCursor(int fd, int x, int y)
{
    char str[20];  // Buffer to store the command string
    sprintf(str, "cursor %d %d", x, y);  // Create the command "cursor x y"
    
    // Write the command to the OLED device file
    int w = write(fd, str, strlen(str));   
    if (w == -1)  // If the write operation fails
    {
        printf("Can not set cursor to LCD\n");  // Print an error message
    }
}

/*
 * Function: OLED_Display
 * ----------------------
 * Displays a string on the OLED screen.
 *
 * fd: the file descriptor of the opened device file.
 * str: the string to be displayed on the OLED screen.
 *
 * This function writes the string to the OLED device, and the string will appear on the screen.
 */
void OLED_Display(int fd, char *str){
    int w = write(fd, str, strlen(str));  // Write the string to the OLED device
    if (w == -1)  // If the write operation fails
    {
        printf("Can not write to LCD\n");  // Print an error message
    }
}

/*
 * Function: OLED_Clear
 * --------------------
 * Clears the OLED display screen.
 *
 * fd: the file descriptor of the opened device file.
 *
 * This function sends the "clear" command to the OLED device to erase all content on the screen.
 */
void OLED_Clear(int fd)
{
    write(fd, "clear", 5);  // Send the "clear" command to the OLED device
}
