#include "button.h"

// Open the button device file for reading
int Button_OpenDevFile() {
    int fd = open(SSD1306_BUTTON_FILE, O_RDONLY | O_NONBLOCK);  // Open the button device file in read-only and non-blocking mode
    if (fd == -1) {
        perror("Failed to open device file");  // Print error if opening fails
        exit(EXIT_FAILURE);  // Exit if opening the file fails
    }

    return fd;  // Return the file descriptor
}

// Read button input from the device
int Button_Read(int fdt, char *buff, size_t size) {
    fd_set fds;
    struct timeval timeout;
    int re;

    FD_ZERO(&fds);  // Clear the file descriptor set
    FD_SET(fdt, &fds);  // Add the button file descriptor to the set

    timeout.tv_sec = 0;  // Set timeout to 0 seconds
    timeout.tv_usec = 0;  // Set timeout to 0 microseconds

    // Use select to check if the button file descriptor is ready for reading
    if (select(fdt + 1, &fds, NULL, NULL, &timeout) > 0) {
        re = read(fdt, buff, size - 1);  // Read from the device
    } else {
        re = -1;  // Set return value to -1 if no data is available
    }

    return re;  // Return the number of bytes read or -1
}

// Check if any button is pressed
int Button_CheckAnyPress(int fd, char *buff, size_t size) {
    int ret = Button_Read(fd, buff, size);  // Read button state

    if (ret <= 0) {
        return 0;  // No button pressed
    } else {
        return 1;  // Button pressed
    }
}

// Get the pressed button number
int Button_Press(char *buff) {
    return atoi(buff);  // Convert button state from string to integer
}

// Wait until any button is pressed and return its value
int Button_WaitForAnyKey(int fd, char *buff, size_t size) {
    int pressed;

    // Loop until a button is pressed
    while (!Button_CheckAnyPress(fd, buff, size));

    pressed = Button_Press(buff);  // Get the pressed button value
    return pressed;  // Return the button value
}

// Check keypress and return the direction based on the button pressed
int Button_KeysPressedToDirection(int fd, char *buff, size_t size, int direction) {
    int pressed;
    
    // If a button is pressed
    if (Button_CheckAnyPress(fd, buff, size)) {
        pressed = Button_Press(buff);  // Get the pressed button value

        // Change direction based on the button pressed, ensuring no reverse direction
        if (direction != pressed) {
            if (pressed == DOWN && direction != UP) {
                direction = pressed;
            } else if (pressed == UP && direction != DOWN) {
                direction = pressed;
            } else if (pressed == LEFT && direction != RIGHT) {
                direction = pressed;
            } else if (pressed == RIGHT && direction != LEFT) {
                direction = pressed;
            }
        }
    }
    
    return direction;  // Return the new direction
}
