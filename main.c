#include "button.h"
#include "oled_i2c_ssd1306.h"
#include "snake.h"

int fd_ssd, fd_button;  // File descriptors for the SSD1306 OLED display and button device
char kernel_data[2];    // Buffer to store kernel data

// Function to clear the terminal screen
void clrscr() {
    system("clear");  // Call system command to clear the terminal screen
    return;
}

int main() {
    int c;  // Variable to store keypress input

    fd_ssd = OLED_OpenDevFile();  // Open the device file for the SSD1306 OLED display

    fd_button = Button_OpenDevFile();  // Open the device file for the button input
    clrscr();  // Clear the terminal screen

    OLED_Clear(fd_ssd);  // Clear the OLED display

    do {
        // Load and start the Snake game
        Snake_LoadGame(fd_ssd, fd_button, kernel_data, sizeof(kernel_data));

        OLED_Clear(fd_ssd);  // Clear the OLED display after the game finishes

        // Display the "Play Again?" message on the OLED
        OLED_SetCursor(fd_ssd, 25, 2);  // Set cursor to position (25, 2)
        OLED_Display(fd_ssd, "PLAY AGAIN ?");  // Display message "PLAY AGAIN ?"

        OLED_SetCursor(fd_ssd, 0, 3);  // Set cursor to position (0, 3)
        OLED_Display(fd_ssd, "ENTER. YES  Other. NO");  // Display options "ENTER. YES Other. NO"

        // Wait for button press and get the key code
        c = Button_WaitForAnyKey(fd_button, kernel_data, sizeof(kernel_data));

        // If ENTER (code 5) is pressed, play the game again
        if (c == 5) {
            OLED_Clear(fd_ssd);  // Clear the OLED display before replaying
        }

    } while (c == 5);  // Repeat the game if ENTER (code 5) is pressed

    // Display "END GAME!" on the OLED screen when the game is over
    OLED_Clear(fd_ssd);  // Clear the OLED display
    OLED_SetCursor(fd_ssd, 30, 3);  // Set cursor to position (30, 3)
    OLED_Display(fd_ssd, " END GAME !");  // Display the message "END GAME!"

    return 0;  // End the program
}
