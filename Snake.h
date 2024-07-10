#ifndef SNAKEGAME_H
#define SNAKEGAME_H

// Define the variable to indicate if it is a cabinato setup
#define iscabinato

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
#ifdef iscabinato
  #include "Cabinato.h"
#endif

// Define OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C

// Joystick PS2
#define VRX A0
#define VRY A1
#define SW 12
#define VRX_THRESHOLD_LOW 400
#define VRX_THRESHOLD_HIGH 600
#define VRY_THRESHOLD_LOW 400
#define VRY_THRESHOLD_HIGH 600
extern Adafruit_SSD1306 display;

// Buzzer
#define BUZZER 13
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_C6 1047
#define NOTE_C7 2093
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_E6 1319
#define NOTE_G5 784
#define NOTE_G6 1568

// Directions are like a clock
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// Game settings
#define SNAKE_SPEED_INITIAL 200  // Initial delay between movements in milliseconds
#define SNAKE_SPEED_MIN 50       // Minimum delay (maximum speed)
#define SNAKE_SPEED_INCREASE 10  // Speed increase factor
#define MAX_SNAKE_LENGTH 40      // Maximum snake length
#define SNAKE_SEGMENT_SIZE 4     // Size of each snake segment (pixels)
#define SNAKE_IN_LENGTH 5        // Initial snake length

// Struct to represent each snake segment
// typedef struct {
//   uint8_t x;
//   uint8_t y;
// } SnakeSegment;

// Global variables
// extern SnakeSegment snake[MAX_SNAKE_LENGTH];
extern uint8_t snakeLength;
extern uint8_t snakeDirection;
extern uint8_t foodX, foodY;
extern bool foodCaught;
extern uint8_t scoreSnake;
extern bool gameRunning;
extern bool gameOverSnake;
extern bool winSnake;
extern bool mainMenuSnake;

// Function prototypes
void playSnake();
#ifndef iscabinato
void setup();
void loop();
void displayMessage(const char* message, uint8_t x, uint8_t y, bool clear);
#endif
void setupSnake();
void loopSnake();
void showStartScreen();
void initializeParamsSnake();
void updateDirection();
void moveSnake();
void handleCollisionsSnake();
void placeFood();
void drawGame();
void handleGameOverSnake();
void snakeJingle();
#ifndef iscabinato
void gameOverJinjle();
#endif

#endif // SNAKEGAME_H
