#ifndef CABINATO_H
#define CABINATO_H

#include <Adafruit_SSD1306.h>
#include <avr/wdt.h>
#include <Wire.h>

// Include game headers
#include "PacMan.h"
#include "Tetris.h"

// Define OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C
extern Adafruit_SSD1306 display;

// Joystick PS2
#define VRX A0
#define VRY A1
#define SW 12

// Game state variables
extern int selectedGame;
extern bool gameStarted;

void setup();
void loop();

void showMainMenu();
void handleMenuInput();
void displayMessage(const char* message, uint8_t x, uint8_t y, bool clear);
void resetArduino();
void gameOverJinjle();

#endif  // CABINATO_H
