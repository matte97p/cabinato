#include <Adafruit_SSD1306.h>
#include <avr/wdt.h>
#include <Wire.h>

// Include game headers
#include "PacMan.h"
#include "Tetris.h"
#include "Snake.h"

// Define OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Joystick PS2
#define VRX A0
#define VRY A1
#define SW 12

// Game state variables
int selectedGame = 1;
bool gameStarted = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Configure input pin modes for analog joystick and switch
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    // Display error message if OLED initialization fails
    Serial.println(F("Error initializing the OLED display!"));
    // Enter infinite loop to indicate a fatal error
    while (1)
      ;
  }

  // Seed the random number generator
  randomSeed(analogRead(0));

  // Mostra il menu iniziale
  showMainMenu();
}

/**
 * Main program loop.
 * If game is not running, handle menu input.
 * If game is running, execute game logic based on selected game.
 */
void loop() {
  if (!gameStarted) {
    handleMenuInput();  // Handle menu navigation input
  } else {
    // Game logic
    if (selectedGame == 1) {
      playPacMan();
    } else if (selectedGame == 2) {
      playTetris();
    } else if (selectedGame == 3) {
      playSnake();
    }
    resetArduino();
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief Displays the main menu on the OLED display.
 * 
 * This function clears the OLED display buffer and draws the main menu with game selection options:
 * - Pac-Man: Displays Pac-Man icon and text. Highlights if selected.
 * - Tetris: Displays Tetris icon and text. Highlights if selected.
 * 
 * The selected game is highlighted with '>' before its name.
 */
void showMainMenu() {
  display.clearDisplay();  // Clear the display buffer

  // Set text and icon positions
  uint8_t imageX = 8;  // X coordinate for game icons
  uint8_t textX = 18;  // X coordinate for game names

  // Draw "Select a Game:" text
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("  Select a Game:");

  // Draw Pac-Man icon and text
  uint8_t pacManY = 18;                                                 // Y coordinate for Pac-Man icon and text
  uint8_t pacManRad = 4;                                                // Radius of Pac-Man icon
  display.fillCircle(imageX + 1.5, pacManY, pacManRad, SSD1306_WHITE);  // Pac-Man body
  if (selectedGame == 1) {
    // Draw Pac-Man mouth if selected
    display.fillTriangle(imageX + 2.5, pacManY, imageX + pacManRad + 1.5, pacManY - pacManRad, imageX + pacManRad + 1.5, pacManY + pacManRad, SSD1306_BLACK);
  }
  display.setCursor(textX, pacManY - (pacManRad / 2) - 1);                          // Position cursor for Pac-Man text
  display.println(selectedGame == 1 ? "  > Pac-Man (1980)" : "   Pac-Man (1980)");  // Highlight selected game

  // Draw Tetris icon and text
  uint8_t tetrisY = 33;                                                                // Y coordinate for Tetris icon and text
  uint8_t tetrisH = 4;                                                                 // Height of Tetris blocks
  display.fillRect(imageX - tetrisH + 0.5, tetrisY, tetrisH, tetrisH, SSD1306_WHITE);  // Tetris block 1
  display.fillRect(imageX + 0.5, tetrisY, tetrisH, tetrisH, SSD1306_WHITE);            // Tetris block 2
  display.fillRect(imageX + tetrisH + 0.5, tetrisY, tetrisH, tetrisH, SSD1306_WHITE);  // Tetris block 3
  display.fillRect(imageX + 0.5, tetrisY + tetrisH, tetrisH, tetrisH, SSD1306_WHITE);  // Tetris block 4
  if (selectedGame == 2) {
    display.invertDisplay(true);
  } else {
    display.invertDisplay(false);
  }
  display.setCursor(textX, tetrisY);                                                // Position cursor for Tetris text
  display.println(selectedGame == 2 ? "  > Tetris  (1984)" : "   Tetris  (1984)");  // Highlight selected game

  // Draw Tetris icon and text
  uint8_t snakeY = 52;                                                         // Y coordinate for Snake icon and text
  uint8_t snakeW = 5;                                                          // Height of Snake segments
  uint8_t snakeH = 6;                                                          // Width of Snake segments
  display.drawRect(imageX - snakeW, snakeY, snakeW, snakeH, SSD1306_WHITE);    // Snake tail
  display.drawRect(imageX, snakeY, snakeW, snakeH, SSD1306_WHITE);             // Snake body
  display.fillRect(imageX + snakeW, snakeY, snakeW, snakeH, SSD1306_WHITE);    // Snake head
  display.drawPixel(imageX + snakeW + 2, snakeY + 1, SSD1306_BLACK);           // Left eye
  display.drawPixel(imageX + snakeW + 2, snakeY + snakeH - 2, SSD1306_BLACK);  // Right eye
  if (selectedGame == 3) {
    uint8_t tongueBaseX = imageX + snakeW + snakeW - 2;                                           // Base X coordinate of the tongue
    uint8_t tongueBaseY = snakeY + snakeH / 2;                                                    // Base Y coordinate of the tongue
    uint8_t tongueEndLeftX = tongueBaseX + 3;                                                     // Endpoint X coordinate for the left part of the tongue
    uint8_t tongueEndLeftY = tongueBaseY + 2;                                                     // Endpoint Y coordinate for the left part of the tongue
    uint8_t tongueEndRightX = tongueBaseX + 3;                                                    // Endpoint X coordinate for the right part of the tongue
    uint8_t tongueEndRightY = tongueBaseY - 2;                                                    // Endpoint Y coordinate for the right part of the tongue
    display.drawLine(tongueBaseX, tongueBaseY, tongueEndLeftX, tongueEndLeftY, SSD1306_WHITE);    // Left segment
    display.drawLine(tongueBaseX, tongueBaseY, tongueEndRightX, tongueEndRightY, SSD1306_WHITE);  // Right segment
  }
  display.setCursor(textX, snakeY - 1);                                             // Position cursor for Snake text
  display.println(selectedGame == 3 ? "  > Snake   (2000)" : "   Snake   (2000)");  // Highlight selected game

  display.display();  // Update OLED display with all changes
}

/**
 * Handles input from the joystick to navigate the main menu.
 * Reads joystick analog values for vertical movement (VRX, VRY) and button press (SW).
 * Updates the selected game based on joystick movement and starts the game if the button is pressed.
 */
void handleMenuInput() {
  // Read analog values from joystick
  int vrxValue = analogRead(VRX);           // Vertical X-axis (not used for navigation)
  int vryValue = analogRead(VRY);           // Vertical Y-axis for navigation
  bool swPressed = digitalRead(SW) == LOW;  // Read joystick button state

  // Debounce delay for button press
  if (swPressed) {
    String message = "Loading ";
    switch (selectedGame) {
      case 1:
        message += "Pac-Man(1980)";
        break;
      case 2:
        message += "Tetris(1984)";
        break;
      case 3:
        message += "Snake(2000)";
        break;
    }
    displayMessage(message.c_str(), 0, 30, true);
    delay(1000);  // Display message for 1 second
    gameStarted = true;
    return;
  }

  // Handle joystick up movement
  if (vryValue < 400) {  // Up
    if (selectedGame > 1) {
      selectedGame--;
      showMainMenu();
      delay(300);
    }
  }

  // Handle joystick down movement
  if (vryValue > 600) {  // Down
    if (selectedGame < 3) {
      selectedGame++;
      showMainMenu();
      delay(300);
    }
  }
}

/**
 * @brief Displays a message on the OLED display.
 * 
 * Clears the display and prints the specified message at the specified position.
 * 
 * @param message The message to display.
 * @param x The x-coordinate (column) where the message starts.
 * @param y The y-coordinate (row) where the message starts.
 */
void displayMessage(const char* message, uint8_t x, uint8_t y, bool clear) {
  if (clear) {
    display.clearDisplay();  // Clear the OLED display
  }
  display.setTextSize(1);               // Set text size to 1
  display.setTextColor(SSD1306_WHITE);  // Set text color to white
  display.setCursor(x, y);              // Set cursor position for the message
  display.println(message);             // Print the specified message
  display.display();                    // Display on OLED screen
}

/**
 * @brief Resets the game menu state and the Arduino board.
 * 
 * This function resets the game menu and then performs an Arduino board reset.
 * The steps are as follows:
 * - Adds a delay of 1000 milliseconds to ensure a pause.
 * - Resets the Arduino using the watchdog timer.
 */
void resetArduino() {
  // Add a delay of 1000 milliseconds to ensure a pause
  delay(1000);
  // Disable interrupts to ensure the reset happens
  cli();
  // Enable the watchdog timer with a 15 ms timeout
  wdt_enable(WDTO_15MS);
  // Wait for the watchdog timer to reset the board
  while (1)
    ;
}

/**
 * @brief Plays the melody for the losing scenario.
 * 
 * This function iterates through a predefined sequence of notes to play a game-over melody.
 * Each note is played with a duration of 100 milliseconds, followed by a 100-millisecond delay.
 */
void gameOverJinjle() {
  // Number of times to play the melody
  const uint8_t repeatCount = 2;

  // Duration for each note in milliseconds
  const uint16_t noteDuration = 100;

  // Notes for the game over melody
  const uint16_t melody[] = {
    NOTE_E5, NOTE_DS5, NOTE_D5, NOTE_CS5,
    NOTE_C5, NOTE_B4, NOTE_AS4, NOTE_A4
  };

  // Number of notes in the melody
  const uint8_t melodyLength = sizeof(melody) / sizeof(melody[0]);

  // Play the melody repeatCount times
  for (uint8_t i = 0; i < repeatCount; i++) {
    for (uint8_t j = 0; j < melodyLength; j++) {
      tone(BUZZER, melody[j], noteDuration);  // Play the current note
      delay(noteDuration);                    // Pause for note duration
    }
  }
}