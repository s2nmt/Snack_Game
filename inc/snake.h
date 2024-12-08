#ifndef SNAKE_H
#define SNAKE_H

#include "button.h"
#include "oled_i2c_ssd1306.h"

#define SNAKE_ARRAY_SIZE 310  // Maximum snake array size

/*
 * Get the game speed.
 */
int Snake_GetGameSpeed();

/*
 * Check if the snake has collided with itself.
 */
int Snake_CheckCollision(int x, int y, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int detect);

/*
 * Generate food at a random position.
 */
void Snake_GenerateFood(int fd, int foodXY[], int width, int height, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength);

/*
 * Initialize the snake array.
 */
void Snake_PrepareArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength);

/*
 * Move the snake by updating its position.
 */
void Snake_MoveArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction);

/*
 * Move the snake and update the display.
 */
void Snake_Move(int fd, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction);

/*
 * Check if the snake has eaten food.
 */
int Snake_EatFood(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[]);

/*
 * Detect collisions with walls or itself.
 */
int Snake_CollisionDetection(int snakeXY[][SNAKE_ARRAY_SIZE], int ScreenWidth, int ScreenHeight, int snakeLength);

/*
 * Load the snake on the display.
 */
void Snake_Load(int fd, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength);

/*
 * Refresh the information bar (score and speed).
 */
void Snake_RefreshInfoBar(int fd, int score, int speed);

/*
 * Display the game over screen.
 */
void Snake_GameOver(int fds, int fdb, char *buff, size_t size);

/*
 * Display the game win screen.
 */
void Snake_GameWin(int fds, int fdb, char *buff, size_t size);

/*
 * Start the snake game.
 */
void Snake_Start(int fds, int fdb, char *buff, size_t size, int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[], int ScreenWidth, int ScreenHeight, int snakeLength, int direction, int score, int speed);

/*
 * Load the game setup.
 */
void Snake_LoadGame(int fds, int fdb, char *buff, size_t size);

#endif
