#include "snake.h"

int fx[26] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125};  // Array of possible x-coordinates for food

// Get the game speed (from 1 to 9)
int Snake_GetGameSpeed() {
    int speed = 1;
    while (speed < 1 || speed > 9) {
        return(speed);  // Return the valid speed
    }
    return 1;
}

// Check collision with snake's body
int Snake_CheckCollisionWithBody(int x, int y, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int detect) {
    int i;
    for (i = detect; i < snakeLength; i++) {
        if (x == snakeXY[0][i] && y == snakeXY[1][i]) {
            return 1;  // Collision detected
        }
    }
    return 0;  // No collision
}

// Generate food at a random position and display it on the screen
void Snake_GenerateFood(int fd, int foodXY[], int width, int height, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength) {
    do {
        srand(time(NULL));
        int temp = rand() % 27;  // Random column
        foodXY[0] = fx[temp];    // Assign random x-coordinate
        srand(time(NULL));
        foodXY[1] = rand() % 7;  // Random row
    } while (Snake_CheckCollisionWithBody(foodXY[0], foodXY[1], snakeXY, snakeLength, 0));

    OLED_SetCursor(fd, foodXY[0], foodXY[1]);
    OLED_Display(fd, "o");  // Display food as 'o'
}

// Prepare snake's initial position in the array
void Snake_PrepareArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength) {
    int i;
    int snakeX = snakeXY[0][0];
    int snakeY = snakeXY[1][0];

    for (i = 1; i <= snakeLength; i++) {
        snakeXY[0][i] = snakeX - 5 * i;  // Initialize snake's x-positions
        snakeXY[1][i] = snakeY;          // Keep y-positions same
    }
}

// Move the snake array based on the direction
void Snake_MoveArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction) {
    int i;
    for (i = snakeLength - 1; i >= 1; i--) {
        snakeXY[0][i] = snakeXY[0][i - 1];
        snakeXY[1][i] = snakeXY[1][i - 1];
    }

    // Update the head's position based on direction
    switch (direction) {
        case DOWN:
            snakeXY[1][0]++;
            break;
        case RIGHT:
            snakeXY[0][0] += 5;
            break;
        case UP:
            snakeXY[1][0]--;
            break;
        case LEFT:
            snakeXY[0][0] -= 5;
            break;
    }
}

// Move the snake and update the display
void Snake_Move(int fd, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction) {
    int x = snakeXY[0][snakeLength - 1];
    int y = snakeXY[1][snakeLength - 1];

    // Clear the tail
    OLED_SetCursor(fd, x, y);
    OLED_Display(fd, " ");

    // Convert the head to a body part
    OLED_SetCursor(fd, snakeXY[0][0], snakeXY[1][0]);
    OLED_Display(fd, "*");

    // Move the snake
    Snake_MoveArray(snakeXY, snakeLength, direction);

    // Update the new head position
    OLED_SetCursor(fd, snakeXY[0][0], snakeXY[1][0]);
    OLED_Display(fd, "O");

    // Avoid flashing underscore by resetting the cursor
    OLED_SetCursor(fd, 1, 1);
}

// Check if the snake has eaten the food
int Snake_EatFood(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[]) {
    if (snakeXY[0][0] == foodXY[0] && snakeXY[1][0] == foodXY[1]) {
        foodXY[0] = 0;
        foodXY[1] = 0; 
        return 1;  // Food eaten
    }
    return 0;  // No food eaten
}

// Check for collisions with walls or itself
int Snake_CollisionDetection(int snakeXY[][SNAKE_ARRAY_SIZE], int ScreenWidth, int ScreenHeight, int snakeLength) {
    // Collision with walls
    if (snakeXY[0][0] < 0 || snakeXY[1][0] < 0 || snakeXY[0][0] >= ScreenWidth || snakeXY[1][0] >= ScreenHeight) {
        return 1;  // Collision detected
    }

    // Collision with itself
    for (int i = 1; i < snakeLength; i++) {
        if (snakeXY[0][0] == snakeXY[0][i] && snakeXY[1][0] == snakeXY[1][i]) {
            return 1;  // Collision detected
        }
    }

    return 0;  // No collision
}

// Load and display the snake on the OLED screen
void Snake_Load(int fd, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength) {
    for (int i = 0; i < snakeLength; i++) {
        OLED_SetCursor(fd, snakeXY[0][i], snakeXY[1][i]);
        OLED_Display(fd, "*");  // Display snake body as '*'
    }
}

// Update the score and speed in the info bar
void Snake_RefreshInfoBar(int fd, int score, int speed) {
    char str[10];

    // Display score
    OLED_SetCursor(fd, 0, 7);
    sprintf(str, "score:%d", score);
    OLED_Display(fd, str);

    // Display speed
    OLED_SetCursor(fd, 70, 7);
    sprintf(str, "speed:%d", speed);
    OLED_Display(fd, str);
}

// Display the game over screen
void Snake_GameOverScreen(int fds, int fdb, char *buff, size_t size) {
    OLED_SetCursor(fds, 25, 3);
    OLED_Display(fds, "GAME OVER!");
    OLED_SetCursor(fds, 10, 4);
    OLED_Display(fds, "PRESS TO CONTINUE");
    Button_WaitForAnyKey(fdb, buff, size);  // Wait for key press
}

// Display the game win screen
void Snake_GameWin(int fds, int fdb, char *buff, size_t size) {
    OLED_SetCursor(fds, 25, 3);
    OLED_Display(fds, "YOU WIN!");
    OLED_SetCursor(fds, 10, 4);
    OLED_Display(fds, "PRESS TO CONTINUE");
    Button_WaitForAnyKey(fdb, buff, size);  // Wait for key press
}

// Start the snake game and handle gameplay
void Snake_StartGame(int fds, int fdb, char *buff, size_t size, int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[], int ScreenWidth, int ScreenHeight, int snakeLength, int direction, int score, int speed) {
    int gameOver = 0;
    clock_t endWait;
    int waitMili = CLOCKS_PER_SEC - (speed) * (CLOCKS_PER_SEC / 10);  // Set wait time based on speed
    int tempScore = 10 * speed;
    int oldDirection;
    int canChangeDirection = 1;

    endWait = clock() + waitMili;

    do {
        if (canChangeDirection) {
            oldDirection = direction;
            direction = Button_KeysPressedToDirection(fdb, buff, size, direction);  // Get new direction from button press
        }

        if (oldDirection != direction) {
            canChangeDirection = 0;  // Prevent snake from moving in the opposite direction
        }

        if (clock() >= endWait) {  // Move snake based on the game speed
            Snake_Move(fds, snakeXY, snakeLength, direction);
            canChangeDirection = 1;

            if (Snake_EatFood(snakeXY, foodXY)) {  // Check if snake eats food
                Snake_GenerateFood(fds, foodXY, ScreenWidth, ScreenHeight, snakeXY, snakeLength);  // Generate new food
                snakeLength++;
                score += 10;

                // Increase speed based on score
                if (score >= 10 * speed + tempScore) {
                    speed++;
                    tempScore = score;

                    if (speed <= 9) {
                        waitMili -= (CLOCKS_PER_SEC / 10);
                    } else if (waitMili >= 40) {  // Maximum speed
                        waitMili -= (CLOCKS_PER_SEC / 200);
                    }
                }

                Snake_RefreshInfoBar(fds, score, speed);  // Update info bar
            }

            endWait = clock() + waitMili;  // Reset wait time
        }

        gameOver = Snake_CollisionDetection(snakeXY, ScreenWidth, ScreenHeight, snakeLength);  // Check for collisions

        // Check if the snake has reached maximum length (win condition)
        if (snakeLength >= SNAKE_ARRAY_SIZE - 5) {
            gameOver = 2;  // Win condition
            score += 1500;  // Bonus points for winning
        }

    } while (!gameOver);

    // Display the appropriate screen based on game over condition
    if (gameOver == 1) {
        Snake_GameOverScreen(fds, fdb, buff, size);
    } else if (gameOver == 2) {
        Snake_GameWin(fds, fdb, buff, size);
    }
}

// Initialize the game and start it
void Snake_LoadGame(int fds, int fdb, char *buff, size_t size) {
    int snakeXY[2][SNAKE_ARRAY_SIZE];  // Snake position array
    int snakeLength = 2;  // Initial length of the snake
    int direction = LEFT;  // Initial direction
    int foodXY[] = {20, 5};  // Initial food position
    int score = 0;
    int ScreenWidth = 128;  // OLED screen width
    int ScreenHeight = 7;   // OLED screen height
    int speed = Snake_GetGameSpeed();  // Get game speed from user

    // Initial snake position
    snakeXY[0][0] = 20;
    snakeXY[1][0] = 1;

    // Prepare and load the snake on the display
    Snake_PrepareArray(snakeXY, snakeLength);
    Snake_Load(fds, snakeXY, snakeLength);
    Snake_GenerateFood(fds, foodXY, ScreenWidth, ScreenHeight, snakeXY, snakeLength);
    Snake_RefreshInfoBar(fds, score, speed);  // Display the info bar
    Snake_StartGame(fds, fdb, buff, size, snakeXY, foodXY, ScreenWidth, ScreenHeight, snakeLength, direction, score, speed);  // Start the game
}

