// Define the variable to indicate if it is a cabinato setup
#define iscabinato

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
#include "Snake.h"
#ifdef iscabinato
#include "Cabinato.h"
#endif

// Define OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C
#ifdef iscabinato
extern Adafruit_SSD1306 display;
#endif
#ifndef iscabinato
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

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

// Snake
typedef struct {
  uint8_t x;
  uint8_t y;
} SnakeSegment;
SnakeSegment snake[MAX_SNAKE_LENGTH];
uint8_t snakeLength;
uint8_t snakeDirection;

// Food
uint8_t foodX, foodY;
bool foodCaught = false;

// Game state checks
uint8_t scoreSnake = 0;
bool gameRunning = false;
bool gameOverSnake = false;
bool winSnake = false;
bool mainMenuSnake = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * Used only in the cabinated version with more games
 */
void playSnake() {
  mainMenuSnake = false;
  setupSnake();
  while (!mainMenuSnake) {
    loopSnake();
  }
}

#ifndef iscabinato
/**
 * @note    that 2 func are replace on cabinet version
 *          setup ->  setupSnake
 *          loop  ->  loopSnake
 */
void setup() {
  setupSnake();
}
void loop() {
  loopSnake();
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
#endif

/**
 * @brief Initializes the Snake game setup including serial communication, joystick and switch pin configurations,
 *        OLED display initialization, and random number generator seeding.
 * 
 * This function sets up the necessary components and displays the start screen.
 */
void setupSnake() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Configure input pin modes for analog joystick and switch
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    // Display error message if OLED initialization fails
    Serial.println(F("Error initializing the OLED display!"));
    // Enter infinite loop to indicate a fatal error
    while (true)
      ;
  }

  // Seed the random number generator
  randomSeed(analogRead(0));

  // Show start screen
  showStartScreen();
}

/**
 * @brief Controls the main game loop for the Snake game.
 * 
 * Manages game states such as starting, running, and handling game mechanics
 * like snake movement, direction updates, collision detection, and drawing the game.
 */
void loopSnake() {
  if (!gameRunning) {
    // Wait for button press to start game
    while (digitalRead(SW) == HIGH) {
      delay(100);
    }
    delay(500);               // Debounce delay
    initializeParamsSnake();  // Initialize game parameters
  } else {
    // Continue game loop if neither gameOverPacMan nor winSnake conditions are met
    if (!gameOverSnake && !winSnake) {
      // Update snakeDirection based on joystick input
      updateDirection();

      // Move snake
      moveSnake();

      // Check for collisions
      handleCollisionsSnake();

      // Draw everything on the OLED display
      drawGame();

      // Delay for snake speed
      delay(max(SNAKE_SPEED_INITIAL - (snakeLength * SNAKE_SPEED_INCREASE), SNAKE_SPEED_MIN));
    } else {
      // Handle game over or win condition
      handleGameOverSnake();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief Displays the start screen message to prompt the player to start the game.
 * 
 * Clears the OLED display and displays a "Press button" message with instructions
 * to start the game. This function is typically called at the beginning to prompt
 * the player to initiate the game by pressing a button.
 */
void showStartScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, SCREEN_HEIGHT / 2 - 10);
  display.println("Press button");
  display.setCursor(10, SCREEN_HEIGHT / 2 + 10);
  display.println("to start");
  display.display();
}

/**
 * @brief Initializes game parameters for Snake.
 * 
 * Resets game state, sets initial snake length, direction, and score.
 * Places initial food and initializes snake position.
 */
void initializeParamsSnake() {
  // Reset game state
  gameRunning = true;
  snakeLength = SNAKE_IN_LENGTH;
  snakeDirection = RIGHT;
  scoreSnake = 0;

  // Place initial food
  placeFood();

  // Initialize snake position
  for (int i = 0; i < snakeLength; i++) {
    snake[i].x = SCREEN_WIDTH / 2 - i * SNAKE_SEGMENT_SIZE;
    snake[i].y = SCREEN_HEIGHT / 2;
  }
}

/**
 * @brief Updates the direction of the snake based on joystick input.
 * 
 * Reads analog values from VRX and VRY pins to determine joystick movement.
 * Sets snakeDirection to LEFT, RIGHT, UP, or DOWN based on joystick position thresholds.
 */
void updateDirection() {
  int vrxValue = analogRead(VRX);
  int vryValue = analogRead(VRY);

  // Controls the preferred axis of movement (horizontal or vertical priority)
  if (abs(vrxValue - 512) > abs(vryValue - 512)) {
    // Priority movement on the horizontal axis
    if (vrxValue < VRX_THRESHOLD_LOW && snakeDirection != RIGHT) {
      snakeDirection = LEFT;
    } else if (vrxValue > VRX_THRESHOLD_HIGH && snakeDirection != LEFT) {
      snakeDirection = RIGHT;
    }
  } else {
    // Priority movement on the vertical axis
    if (vryValue < VRY_THRESHOLD_LOW && snakeDirection != DOWN) {
      snakeDirection = UP;
    } else if (vryValue > VRY_THRESHOLD_HIGH && snakeDirection != UP) {
      snakeDirection = DOWN;
    }
  }
}

/**
 * @brief Moves the snake according to its current direction and handles collisions.
 * 
 * Moves the snake's body segments and updates the snake's head position based on snakeDirection.
 */
void moveSnake() {
  // Move snake body segments
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  // Move snake head based on snakeDirection
  switch (snakeDirection) {
    case UP:
      snake[0].y -= SNAKE_SEGMENT_SIZE;
      break;
    case RIGHT:
      snake[0].x += SNAKE_SEGMENT_SIZE;
      break;
    case DOWN:
      snake[0].y += SNAKE_SEGMENT_SIZE;
      break;
    case LEFT:
      snake[0].x -= SNAKE_SEGMENT_SIZE;
      break;
  }

  // Wrap snake around screen edges
  if (snake[0].x >= SCREEN_WIDTH) {
    snake[0].x = 0;  // Wrap from right edge to left edge
  } else if (snake[0].x <= 0) {
    snake[0].x = SCREEN_WIDTH - SNAKE_SEGMENT_SIZE;  // Wrap from left edge to right edge
  }
}

/**
 * @brief Handles collisions of the snake with walls and itself.
 * 
 * Checks collisions of the snake's head with screen edges (only top and bottom walls)
 * and collisions with its own body segments.
 * Calls handleGameOverSnake() function if a collision is detected.
 */
void handleCollisionsSnake() {
  // Check screen edge collisions (only top and bottom walls)
  if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH || snake[0].y < 8 || snake[0].y >= SCREEN_HEIGHT) {
    gameOverSnake = true;
  }

  // Check self collisions
  for (int i = 1; i < snakeLength; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      gameOverSnake = true;
    }
  }

  // Check for food collision
  if (snake[0].x == foodX && snake[0].y == foodY) {
    scoreSnake++;                // Increment score
    snakeLength++;               // Increment lenght
    placeFood();                 // Place new food
    tone(BUZZER, NOTE_C6, 100);  // Play sound when food is eaten
    if (snakeLength == MAX_SNAKE_LENGTH) {
      winSnake = true;
    }
  }
}

/**
 * @brief Places food randomly on the game grid, ensuring it doesn't overlap with the snake.
 * 
 * Uses random() function to generate coordinates for foodX and foodY within the playable
 * area of the screen. Checks if the generated food coordinates overlap with any segment
 * of the snake. If overlap occurs, recursively calls itself to place the food again until
 * a valid position is found.
 */
void placeFood() {
  // Generate random coordinates for food
  foodX = random(1, SCREEN_WIDTH / SNAKE_SEGMENT_SIZE) * SNAKE_SEGMENT_SIZE;
  foodY = random(8, SCREEN_HEIGHT / SNAKE_SEGMENT_SIZE) * SNAKE_SEGMENT_SIZE;

  // Ensure food doesn't spawn on the snake
  for (int i = 0; i < snakeLength; i++) {
    if (foodX == snake[i].x && foodY == snake[i].y) {
      placeFood();  // Recursive call to place food again
      return;
    }
  }
}

/**
 * @brief Clears the display and draws the current game state, including the snake, food, and score.
 * 
 * Clears the OLED display and redraws the snake and food based on their current positions. 
 * The snake's head is drawn with eyes, and the body is drawn as segments. The food is represented 
 * as a filled rectangle. Also displays the current scoreSnake at the top-left corner of the display 
 * and draws a border around the playable area.
 */
void drawGame() {
  display.clearDisplay();

  // Draw snake
  for (int i = 0; i < snakeLength; i++) {
    if (i == 0) {
      // Draw snake head
      display.fillRect(snake[i].x, snake[i].y, SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE, SSD1306_WHITE);

      // Determine eye positions based on snake direction
      int eyeLeftX, eyeLeftY, eyeRightX, eyeRightY;
      switch (snakeDirection) {
        case UP:
          eyeLeftX = snake[i].x + 1;
          eyeLeftY = snake[i].y + 1;
          eyeRightX = snake[i].x + SNAKE_SEGMENT_SIZE - 2;
          eyeRightY = snake[i].y + 1;
          break;
        case RIGHT:
          eyeLeftX = snake[i].x + SNAKE_SEGMENT_SIZE - 2;
          eyeLeftY = snake[i].y + 1;
          eyeRightX = snake[i].x + SNAKE_SEGMENT_SIZE - 2;
          eyeRightY = snake[i].y + SNAKE_SEGMENT_SIZE - 2;
          break;
        case DOWN:
          eyeLeftX = snake[i].x + 1;
          eyeLeftY = snake[i].y + SNAKE_SEGMENT_SIZE - 2;
          eyeRightX = snake[i].x + SNAKE_SEGMENT_SIZE - 2;
          eyeRightY = snake[i].y + SNAKE_SEGMENT_SIZE - 2;
          break;
        case LEFT:
          eyeLeftX = snake[i].x + 1;
          eyeLeftY = snake[i].y + 1;
          eyeRightX = snake[i].x + 1;
          eyeRightY = snake[i].y + SNAKE_SEGMENT_SIZE - 2;
          break;
        default:
          // Default to UP direction
          eyeLeftX = snake[i].x + 1;
          eyeLeftY = snake[i].y + 1;
          eyeRightX = snake[i].x + SNAKE_SEGMENT_SIZE - 2;
          eyeRightY = snake[i].y + 1;
          break;
      }

      // Draw eyes
      display.drawPixel(eyeLeftX, eyeLeftY, SSD1306_BLACK);
      display.drawPixel(eyeRightX, eyeRightY, SSD1306_BLACK);

      bool mouthOpen = false;

      // Draw mouth if open
      if (mouthOpen) {
        //TODO
      } else {
        // Calculate tongue base coordinates at the center bottom of the head
        int tongueBaseX = snake[i].x + SNAKE_SEGMENT_SIZE / 2;
        int tongueBaseY = snake[i].y + SNAKE_SEGMENT_SIZE - 1;

        // Calculate tongue end coordinates based on snake direction
        int tongueEndLeftX, tongueEndLeftY, tongueEndRightX, tongueEndRightY;

        switch (snakeDirection) {
          case UP:
            tongueEndLeftX = tongueBaseX - 2;
            tongueEndLeftY = tongueBaseY - 4;
            tongueEndRightX = tongueBaseX + 2;
            tongueEndRightY = tongueBaseY - 4;
            break;
          case RIGHT:
            tongueEndLeftX = tongueBaseX + 4;
            tongueEndLeftY = tongueBaseY + 2;
            tongueEndRightX = tongueBaseX + 4;
            tongueEndRightY = tongueBaseY - 2;
            break;
          case DOWN:
            tongueEndLeftX = tongueBaseX - 2;
            tongueEndLeftY = tongueBaseY + 4;
            tongueEndRightX = tongueBaseX + 2;
            tongueEndRightY = tongueBaseY + 4;
            break;
          case LEFT:
            tongueEndLeftX = tongueBaseX - 4;
            tongueEndLeftY = tongueBaseY + 2;
            tongueEndRightX = tongueBaseX - 4;
            tongueEndRightY = tongueBaseY - 2;
            break;
          default:
            // Default to UP direction
            tongueEndLeftX = tongueBaseX - 2;
            tongueEndLeftY = tongueBaseY - 4;
            tongueEndRightX = tongueBaseX + 2;
            tongueEndRightY = tongueBaseY - 4;
            break;
        }

        // Draw bifurcated tongue
        display.drawLine(tongueBaseX, tongueBaseY, tongueEndLeftX, tongueEndLeftY, SSD1306_WHITE);    // Left segment
        display.drawLine(tongueBaseX, tongueBaseY, tongueEndRightX, tongueEndRightY, SSD1306_WHITE);  // Right segment
      }
    } else {
      // Draw snake body as dashes
      display.drawRect(snake[i].x, snake[i].y, SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE, SSD1306_WHITE);
    }
  }

  // Draw food
  display.fillRect(foodX, foodY, SNAKE_SEGMENT_SIZE, SNAKE_SEGMENT_SIZE, SSD1306_WHITE);

  // Draw score
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);  // Place scoreSnake at the top-left corner
  display.print("Score: ");
  display.println(scoreSnake);

  // Draw border
  display.drawRect(0, 8, SCREEN_WIDTH, SCREEN_HEIGHT - 7.5, SSD1306_WHITE);  // Border

  display.display();
}

/**
 * @brief Handles game over state by displaying a message and resetting game parameters.
 * 
 * Clears the OLED display and displays a "Game Over" message along with the final scoreSnake.
 * After displaying for 2 seconds, resets the game by setting snakeLength to initial value,
 * snakeDirection to right, scoreSnake to 0, and reinitializes snake and food positions.
 */
void handleGameOverSnake() {
  // Handle win scenario
  if (winSnake) {
    // Player wins the game
    displayMessage("YOU WIN!!!", 37, 25, true);
    String message = "Score: " + String(scoreSnake);
    displayMessage(message.c_str(), 35, 35, false);
    snakeJingle();
    // Set flag to return to main menu
    mainMenuSnake = true;
  } else if (gameOverSnake) {
    // Handle loss scenario
    displayMessage("Game Over", 37, 25, true);
    String message = "Score: " + String(scoreSnake);
    displayMessage(message.c_str(), 35, 35, false);
    gameOverJinjle();
    // Set flag to return to main menu
    mainMenuSnake = true;
  }

  delay(2000);
#ifndef iscabinato
  gameOverSnake = false;
  winSnake = false;
  initializeParamsSnake();
#endif
}

/**
 * @brief Plays the melody for the winning scenario.
 * 
 * This function iterates through a predefined sequence of notes to play a victory melody.
 * Each note is played with a duration of 200 milliseconds, followed by a 100-millisecond delay.
 */
void snakeJingle() {
  // Number of times to play the melody
  const uint8_t repeatCount = 2;

  // Duration for each note in milliseconds
  const uint16_t noteDuration = 100;

  // Notes for the game over melody
  const uint16_t melody[] = {
    NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6,
    NOTE_G5, NOTE_E5, NOTE_C5, NOTE_E5,
    NOTE_G5, NOTE_C6, NOTE_G5, NOTE_E5,
    NOTE_C6, NOTE_E6, NOTE_G6, NOTE_C7
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

#ifndef iscabinato
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
#endif