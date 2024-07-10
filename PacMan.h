#ifndef PACMAN_H
#define PACMAN_H

// Define the variable to indicate if it is a cabinato setup
#define iscabinato

#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include <Arduino.h>
#ifdef iscabinato
  #include "Cabinato.h"
#endif

// Define OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C
extern Adafruit_SSD1306 display;

// Define the grid and cell sizes
#define GRID_WIDTH 16
#define GRID_HEIGHT 8
#define CELL_WIDTH (SCREEN_WIDTH / GRID_WIDTH)
#define CELL_HEIGHT (SCREEN_HEIGHT / GRID_HEIGHT)

// Joystick PS2
#define VRX A0
#define VRY A1
#define SW 12
#define VRX_THRESHOLD_LOW 400
#define VRX_THRESHOLD_HIGH 600
#define VRY_THRESHOLD_LOW 400
#define VRY_THRESHOLD_HIGH 600

// Buzzer
#define BUZZER 13
#define NOTE_A3 220
#define NOTE_A4 440
#define NOTE_A5 880
#define NOTE_AS4 466
#define NOTE_B3 247
#define NOTE_B4 494
#define NOTE_B5 988
#define NOTE_C3 131
#define NOTE_C4 262
#define NOTE_C5 523
#define NOTE_C6 1047
#define NOTE_CS5 554
#define NOTE_D4 294
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E3 165
#define NOTE_E4 330
#define NOTE_E5 659
#define NOTE_E6 1319
#define NOTE_F3 175
#define NOTE_F4 349
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G3 196
#define NOTE_G4 392
#define NOTE_G5 784
#define NOTE_G6 1568
#define NOTE_GS5 831

// Directions are like a clock
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// Game settings
#define EMPTY 0x00
#define OBJECT 0x01
#define FRUIT 0x02
#define WALL 0x10
#define GHOST 0x20
#define updateInterval 200
#define POWER_UP_DURATION 8000
#define ghostSpeed 400

// Grid
extern uint8_t grid[GRID_WIDTH][GRID_HEIGHT];
extern uint8_t totalObjects;

// Pac-Man properties
extern uint8_t pacManX;
extern uint8_t pacManY;
extern uint8_t initialPacManX;
extern uint8_t initialPacManY;
extern bool animationActive;
extern uint8_t pacManDirection;
extern unsigned long powerUpStartTime;
extern bool powerUpActive;

// Ghost properties
extern uint8_t numGhosts;
extern uint8_t ghostX[5];
extern uint8_t ghostY[5];
extern uint8_t initialGhostX[5];
extern uint8_t initialGhostY[5];
extern unsigned long lastGhostMove;

// Game state checks
extern unsigned long lastUpdate;
extern uint8_t currLevel;
extern int scorePacMan;
extern bool gameOverPacMan;
extern bool win;
extern bool mainMenuPacMan;
extern bool test;
extern int lifes;

void playPacMan();
void setup();
void loop();
void setupPacMan();
void loopPacMan();
void loadLevel();
void initializeParamsPacMan();
void drawGrid();
void drawWallsAndObjects();
void drawPacMan();
void drawGhosts();
void movePacMan();
void switchPowerUp(bool active);
bool isPowerUpActive();
void moveGhosts();
void shuffleArray(uint8_t arr[], int size);
void handleCollisionsPacMan();
void resetGhostPosition(uint8_t index);
void newLife();
void checkWin();
void handleGameOverPacMan();
void nextLevelJinjle();
void loseLifeJinjle();
void pacManJingle();
void gameOverJinjle();

#endif // PACMAN_H
