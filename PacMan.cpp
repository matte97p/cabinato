// Define the variable to indicate if it is a cabinato setup
#define iscabinato

#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include <Arduino.h>
#include "PacMan.h"
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
uint8_t grid[GRID_WIDTH][GRID_HEIGHT];
uint8_t totalObjects;

// Pac-Man properties
uint8_t pacManX;
uint8_t pacManY;
uint8_t initialPacManX;
uint8_t initialPacManY;
bool animationActive;
uint8_t pacManDirection;
unsigned long powerUpStartTime = 0;
bool powerUpActive;

// Ghost properties
uint8_t numGhosts = 0;
uint8_t ghostX[5];
uint8_t ghostY[5];
uint8_t initialGhostX[5];
uint8_t initialGhostY[5];
unsigned long lastGhostMove = 0;

// Game state checks
unsigned long lastUpdate = 0;
uint8_t currLevel = 1;
int scorePacMan = 0;
bool gameOverPacMan = false;
bool winPacMan = false;
bool mainMenuPacMan = false;
bool test = false;
int lifes = 3;

/**
 * @brief Loads the specified level from levelDataPacMan into the game grid (grid[][]).
 * 
 * Initializes game parameters and sets up the grid based on the current level.
 * Uses levelDataPacMan array to configure grid cells with walls, objects, Pac-Man, ghosts, and fruits.
 * 
 * @param level Index of the level to load from levelDataPacMan.
 * 
 * levelDataPacMan array structure:
 * - Each level is represented as a 2D array of uint8_t values.
 * - Each element in the 2D array corresponds to a cell in the game grid.
 * - Values represent different elements:
 *   - 0: Empty cell
 *   - 1: Wall
 *   - 2: Pac-Man
 *   - 3: Ghost
 *   - 4: Fruit
 * 
 * @return None
 */
const uint8_t levelPacMan = 11;
const uint8_t levelDataPacMan[][GRID_HEIGHT][GRID_WIDTH] = {
  // Level 0: Empty level with just objects -- only for tests
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 1
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 3, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 2, 2, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 4, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 4, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 2
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1 },
    { 1, 0, 1, 2, 4, 0, 1, 0, 1, 0, 1, 2, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 3
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 4, 1, 0, 0, 0, 0, 2, 1, 0, 1 },
    { 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 4, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 4
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 1, 2, 0, 0, 0, 0, 4, 1, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 2, 1, 1, 0, 1, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 5
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 2, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 0, 0, 4, 1, 1, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 2, 1, 0, 1 },
    { 1, 3, 1, 0, 0, 0, 0, 0, 4, 1, 1, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 6
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 3, 1, 4, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 4, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 0, 1, 2, 0, 1, 2, 0, 0, 0, 1 },
    { 1, 0, 1, 2, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 1, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 7
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 2, 1, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 1, 4, 0, 0, 0, 1, 1, 0, 0, 0, 4, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 2, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 8
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1 },
    { 1, 0, 1, 0, 0, 1, 4, 1, 4, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 1, 2, 0, 1, 0, 1, 0, 1, 1, 2, 0, 1, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
    { 1, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 4, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 9
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1 },
    { 1, 0, 1, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1 },
    { 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 10
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
    { 1, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 1, 0, 1 },
    { 1, 0, 1, 0, 1, 1, 0, 2, 2, 0, 0, 1, 0, 1, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
  // Level 11
  {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1 },
    { 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * Used only in the cabinated version with more games
 */
void playPacMan() {
  setupPacMan();
  while (!mainMenuPacMan) {
    loopPacMan();
  }
}

#ifndef iscabinato
/**
 * @note    that 2 func are replace on cabinet version
 *          setup ->  setupPacMan
 *          loop  ->  loopPacMan
 */
void setup() {
  setupPacMan();
}
void loop() {
  loopPacMan();
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
 * @brief Initializes hardware and game components at startup.
 * 
 * This function sets up various components required for the game:
 * - Serial communication for debugging purposes.
 * - Input pin modes for analog joystick and switch.
 * - OLED display initialization and error handling.
 * - Random number generator initialization.
 * - Loads initial game level configuration onto the grid.
 */
void setupPacMan() {
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

  // Load initial game level configuration onto the grid
  loadLevel();
}

/**
 * @brief Main game loop handling Pac-Man gameplay and game state transitions.
 * 
 * This function continuously runs while the game is active:
 * - Moves Pac-Man and ghosts at regular intervals defined by updateInterval.
 * - Checks collisions between Pac-Man, ghosts, and game elements.
 * - Draws the game grid to display current game state.
 * - Checks winPacMan condition to determine if the player has won the game.
 * 
 * If the game is over (gameOverPacMan == true) or the player wins (winPacMan == true):
 * - Calls handleGameOverPacMan() to manage transitions between game levels or end game scenarios.
 */
void loopPacMan() {
  // Continue game loop if neither gameOverPacMan nor winPacMan conditions are met
  if (!gameOverPacMan && !winPacMan) {
    unsigned long currentMillis = millis();

    // Perform updates at regular intervals
    if (currentMillis - lastUpdate >= updateInterval) {
      lastUpdate = currentMillis;

      // Move Pac-Man and check collisions
      movePacMan();
      handleCollisionsPacMan();

      // Move ghosts and check collisions
      moveGhosts();
      handleCollisionsPacMan();

      // Draw the updated game grid
      drawGrid();

      // Check if the player has won the game
      checkWin();
    }
  } else {
    // Handle game over or winPacMan condition
    handleGameOverPacMan();
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief Initializes game parameters and sets up the grid for the current level.
 * 
 * This function iterates through each cell in the grid and sets its type based on the
 * current level's data from the levelDataPacMan array. It initializes Pac-Man's position, ghost positions,
 * and counts total objects in the grid.
 */
void loadLevel() {
  initializeParamsPacMan();  // Initialize game parameters

  // Iterate through each cell in the grid
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      if (x == 0 || x == GRID_WIDTH - 1 || y == 0 || y == GRID_HEIGHT - 1) {
        // External walls: Set as WALL
        grid[x][y] = WALL;
      } else if (levelDataPacMan[currLevel][y][x] == 0) {
        // Objects: Set as OBJECT and increase totalObjects count
        grid[x][y] = OBJECT;
        totalObjects++;
      } else if (levelDataPacMan[currLevel][y][x] == 1) {
        // Walls: Set as WALL
        grid[x][y] = WALL;
      } else if (levelDataPacMan[currLevel][y][x] == 2) {
        // Objects + Ghost: Set as OBJECT, increase totalObjects count,
        // and initialize ghost positions
        grid[x][y] = OBJECT;
        totalObjects++;
        ghostX[numGhosts] = x;
        ghostY[numGhosts] = y;
        initialGhostX[numGhosts] = x;
        initialGhostY[numGhosts] = y;
        numGhosts++;
      } else if (levelDataPacMan[currLevel][y][x] == 3) {
        // Pac-Man: Set as EMPTY and initialize Pac-Man position
        grid[x][y] = EMPTY;
        pacManX = x;
        pacManY = y;
        initialPacManX = x;
        initialPacManY = y;
      } else if (levelDataPacMan[currLevel][y][x] == 4) {
        // Fruit: Set as FRUIT and increase totalObjects count
        grid[x][y] = FRUIT;
        totalObjects++;
      }
    }
  }
}

/**
 * @brief Initializes all game parameters including walls, objects, Pac-Man, ghosts, and checks.
 * 
 * Resets all game-related variables to their initial state to start a new game or level.
 * Clears the display at the beginning of the initialization.
 */
void initializeParamsPacMan() {
  // Reset total objects count to 0
  totalObjects = 0;

  // Initialize Pac-Man parameters
  animationActive = true;  // Enable animation
  pacManDirection = 1;     // Set default Pac-Man direction
  switchPowerUp(false);    // Deactivate power-up

  // Reset ghost-related variables
  numGhosts = 0;      // No ghosts initially
  lastGhostMove = 0;  // Reset last ghost movement time

  // Reset game update timing
  lastUpdate = 0;  // Reset last update time

  // Initialize current level based on test mode or new game start
  if (test)  // If in test mode, start from level 0
    currLevel = 0;
  if (currLevel == 1) {
    scorePacMan = 0;  // Reset scorePacMan to 0
    lifes = 3;        // Set initial lifes to 3
  }

  // Reset game flags
  gameOverPacMan = false;  // Game is not over
  winPacMan = false;             // Player hasn't won yet

  // Clear the display buffer
  display.clearDisplay();
}

/**
 * @brief Draws the game grid including walls, objects, Pac-Man, and ghosts on the display.
 * 
 * Clears the display and then sequentially draws walls, objects, Pac-Man, and ghosts at their
 * respective positions based on the current game state.
 */
void drawGrid() {
  display.clearDisplay();  // Clear the display buffer

  drawWallsAndObjects();  // Draw walls and objects based on grid state
  drawPacMan();           // Draw Pac-Man at its current position
  drawGhosts();           // Draw all ghosts at their respective positions

  display.display();  // Display the updated grid on the OLED screen

  // Check if power-up mode has timed out and deactivate if necessary
  if (!isPowerUpActive()) {
    switchPowerUp(false);
  }
}

/**
 * @brief Draws walls, objects, and fruits on the display based on the grid state.
 * 
 * Iterates through the entire grid to draw rectangles for walls, circles for objects,
 * and larger circles with specific patterns for fruits.
 */
void drawWallsAndObjects() {
  for (uint8_t x = 0; x < GRID_WIDTH; x++) {
    for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
      if (grid[x][y] & WALL) {
        // Draw a small rectangle for a wall
        display.drawRect(x * 8, y * 8, 8, 8, SSD1306_WHITE);
      } else if (grid[x][y] & OBJECT) {
        // Draw a small circle for an object
        display.fillCircle(x * 8 + 4, y * 8 + 4, 1.5, SSD1306_WHITE);
      } else if (grid[x][y] & FRUIT) {
        // Draw a larger circle for a fruit
        display.fillCircle(x * 8 + 4, y * 8 + 4, 3, SSD1306_WHITE);

        // Draw a distinctive pattern inside the fruit
        display.fillCircle(x * 8 + 6, y * 8 + 3, 1, SSD1306_BLACK);
        display.fillCircle(x * 8 + 2, y * 8 + 3, 1, SSD1306_BLACK);
        display.fillCircle(x * 8 + 4, y * 8 + 6, 1, SSD1306_BLACK);
      }
    }
  }
}

/**
 * @brief Draws Pac-Man on the display at its current position.
 * 
 * Draws Pac-Man as a filled circle with optional animation based on movement direction.
 * Pac-Man's appearance changes when in power-up mode, showing additional features.
 */
void drawPacMan() {
  uint8_t centerX = pacManX * 8 + 4;
  uint8_t centerY = pacManY * 8 + 4;
  uint8_t radius = 3;

  if (isPowerUpActive()) {
    // Draw Pac-Man differently when in power-up mode
    display.fillRect(pacManX * 8, pacManY * 8, 8, 8, SSD1306_BLACK);  // Clear Pac-Man space
    display.fillCircle(centerX, centerY, radius, SSD1306_WHITE);      // Pac-Man body

    // Add distinctive features for power-up mode
    display.drawLine(centerX - 2, centerY, centerX + 2, centerY, SSD1306_BLACK);
    display.drawLine(centerX, centerY - 2, centerX, centerY + 2, SSD1306_BLACK);
  } else {
    // Draw regular Pac-Man body
    display.fillCircle(centerX, centerY, radius, SSD1306_WHITE);

    // Add animation based on movement direction
    if (animationActive) {
      switch (pacManDirection) {
        case UP:
          display.fillTriangle(centerX, centerY, centerX - 3, centerY - 3, centerX + 3, centerY - 3, SSD1306_BLACK);
          break;
        case RIGHT:
          display.fillTriangle(centerX, centerY, centerX + 3, centerY - 3, centerX + 3, centerY + 3, SSD1306_BLACK);
          break;
        case DOWN:
          display.fillTriangle(centerX, centerY, centerX - 3, centerY + 3, centerX + 3, centerY + 3, SSD1306_BLACK);
          break;
        case LEFT:
          display.fillTriangle(centerX, centerY, centerX - 3, centerY - 3, centerX - 3, centerY + 3, SSD1306_BLACK);
          break;
      }
    }
  }
}

/**
 * @brief Draws ghosts on the display based on their current positions.
 * 
 * Draws the body and eyes of each ghost at their respective grid positions.
 * Depending on the animation state, eyes are drawn either open or closed.
 */
void drawGhosts() {
  for (uint8_t i = 0; i < numGhosts; i++) {
    uint8_t startX = ghostX[i] * 8;
    uint8_t startY = ghostY[i] * 8;

    // Draw ghost body
    display.fillRect(startX, startY, 8, 8, SSD1306_WHITE);                        // Ghost body
    display.drawLine(startX, startY + 7, startX + 8, startY + 7, SSD1306_BLACK);  // Ghost base

    // Draw eyes based on animation state
    if (animationActive) {
      display.fillCircle(startX + 2, startY + 3, 1, SSD1306_BLACK);  // Left eye
      display.fillCircle(startX + 6, startY + 3, 1, SSD1306_BLACK);  // Right eye
    } else {
      display.fillRect(startX + 1, startY + 3, 2, 2, SSD1306_BLACK);  // Left eye closed
      display.fillRect(startX + 5, startY + 3, 2, 2, SSD1306_BLACK);  // Right eye closed
    }
  }
}

/**
 * @brief Updates Pac-Man's position based on joystick input and handles movement constraints.
 * 
 * This function reads the analog joystick values to determine Pac-Man's movement direction.
 * It prioritizes movement along the axis with the greatest input deviation from center,
 * ensuring Pac-Man does not move through walls or out of bounds.
 * If Pac-Man moves, it updates his grid position and toggles animation state.
 */
void movePacMan() {
  uint8_t newPx = pacManX;
  uint8_t newPy = pacManY;

  // Read joystick values
  int vrxValue = analogRead(VRX);
  int vryValue = analogRead(VRY);

  // Controls the preferred axis of movement (horizontal or vertical priority)
  if (abs(vrxValue - 512) > abs(vryValue - 512)) {
    // Priority movement on the horizontal axis
    if (vrxValue < VRX_THRESHOLD_LOW && pacManX > 0 && !(grid[pacManX - 1][pacManY] & WALL)) {
      newPx--;
      pacManDirection = LEFT;
    } else if (vrxValue > VRX_THRESHOLD_HIGH && pacManX < GRID_WIDTH - 1 && !(grid[pacManX + 1][pacManY] & WALL)) {
      newPx++;
      pacManDirection = RIGHT;
    }
  } else {
    // Priority movement on the vertical axis
    if (vryValue < VRY_THRESHOLD_LOW && pacManY > 0 && !(grid[pacManX][pacManY - 1] & WALL)) {
      newPy--;
      pacManDirection = UP;
    } else if (vryValue > VRY_THRESHOLD_HIGH && pacManY < GRID_HEIGHT - 1 && !(grid[pacManX][pacManY + 1] & WALL)) {
      newPy++;
      pacManDirection = DOWN;
    }
  }

  // Update Pac-Man's position and animation state if it moved
  if (newPx != pacManX || newPy != pacManY) {
    pacManX = newPx;
    pacManY = newPy;
    animationActive = !animationActive;
  }
}

/**
 * @brief Activates or deactivates the power-up and adjusts display inversion accordingly.
 * 
 * @param active true to activate the power-up, false to deactivate.
 * 
 * This function toggles the state of the power-up based on the provided boolean parameter.
 * When activating the power-up, it sets `powerUpActive` to true and records the activation time.
 * It also inverts the display to indicate the activation visually.
 * When deactivating the power-up, it resets `powerUpActive` and clears the activation time.
 * The display inversion is turned off to restore the normal display appearance.
 */
void switchPowerUp(bool active) {
  if (active) {
    powerUpActive = true;
    powerUpStartTime = millis();
    display.invertDisplay(true);  // Invert display to indicate power-up activation
  } else {
    powerUpActive = false;
    powerUpStartTime = 0;
    display.invertDisplay(false);  // Restore display to normal when deactivating power-up
  }
}

/**
 * @brief Checks if the power-up is currently active.
 * 
 * The power-up is considered active if it has been activated and its duration, defined
 * by `POWER_UP_DURATION`, has not elapsed.
 * 
 * @return true if the power-up is active, false otherwise.
 */
bool isPowerUpActive() {
  return powerUpActive && (millis() - powerUpStartTime < POWER_UP_DURATION);
}

/**
 * @brief Moves each ghost towards Pac-Man or away from Pac-Man depending on game state.
 *        Ghosts prioritize moving horizontally or vertically towards Pac-Man.
 *        If Pac-Man is not directly reachable, ghosts move randomly in valid directions.
 * 
 * This function updates the positions of all ghosts based on their movement rules:
 * - Ghosts move towards Pac-Man's position if possible.
 * - If Pac-Man is unreachable or in power-up mode, ghosts move randomly.
 * - Ghosts avoid walls and other ghosts while moving.
 */
void moveGhosts() {
  unsigned long currentMillis = millis();

  // Check if enough time has passed since the last ghost move
  if (currentMillis - lastGhostMove < ghostSpeed) {
    return;  // Skip ghost movement if not enough time has elapsed
  }

  lastGhostMove = currentMillis;  // Update last ghost move time

  for (uint8_t i = 0; i < numGhosts; i++) {
    uint8_t newX = ghostX[i];
    uint8_t newY = ghostY[i];
    bool foundValidMove = false;

    // Calculate direction towards Pac-Man
    int8_t dx = pacManX - ghostX[i];
    int8_t dy = pacManY - ghostY[i];

    // Move ghosts away from Pac-Man when in power-up mode
    if (isPowerUpActive()) {
      dx *= -1;
      dy *= -1;
    }

    // Prioritize horizontal or vertical movement towards Pac-Man
    if (abs(dx) > abs(dy)) {
      // Move horizontally towards Pac-Man
      if (dx > 0 && newX + 1 < GRID_WIDTH && !(grid[newX + 1][newY] & (WALL | GHOST))) {
        newX++;
        foundValidMove = true;
      } else if (dx < 0 && newX - 1 >= 0 && !(grid[newX - 1][newY] & (WALL | GHOST))) {
        newX--;
        foundValidMove = true;
      }
    } else {
      // Move vertically towards Pac-Man
      if (dy > 0 && newY + 1 < GRID_HEIGHT && !(grid[newX][newY + 1] & (WALL | GHOST))) {
        newY++;
        foundValidMove = true;
      } else if (dy < 0 && newY - 1 >= 0 && !(grid[newX][newY - 1] & (WALL | GHOST))) {
        newY--;
        foundValidMove = true;
      }
    }

    // If Pac-Man is not directly reachable, try each valid direction in a random order
    if (!foundValidMove) {
      uint8_t directions[4] = { UP, RIGHT, DOWN, LEFT };
      shuffleArray(directions, 4);  // Shuffle directions for random movement

      for (uint8_t dir : directions) {
        switch (dir) {
          case UP:
            if (newY > 0 && !(grid[newX][newY - 1] & (WALL | GHOST))) {
              newY--;
              foundValidMove = true;
            }
            break;
          case RIGHT:
            if (newX < GRID_WIDTH - 1 && !(grid[newX + 1][newY] & (WALL | GHOST))) {
              newX++;
              foundValidMove = true;
            }
            break;
          case DOWN:
            if (newY < GRID_HEIGHT - 1 && !(grid[newX][newY + 1] & (WALL | GHOST))) {
              newY++;
              foundValidMove = true;
            }
            break;
          case LEFT:
            if (newX > 0 && !(grid[newX - 1][newY] & (WALL | GHOST))) {
              newX--;
              foundValidMove = true;
            }
            break;
        }

        // Exit loop if a valid move is found
        if (foundValidMove) {
          break;
        }
      }
    }

    // Update ghost's position
    ghostX[i] = newX;
    ghostY[i] = newY;
  }
}

/**
 * @brief Shuffles the elements of an array using the Fisher-Yates algorithm.
 * 
 * @param arr The array to be shuffled.
 * @param size The number of elements in the array.
 */
void shuffleArray(uint8_t arr[], int size) {
  for (int i = size - 1; i > 0; --i) {
    // Generate a random index between 0 and i (inclusive)
    int j = random(i + 1);

    // Swap the current element with the element at index j
    uint8_t temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

/**
 * @brief Handles interactions including object pickup, fruit collection, and ghost collision.
 * 
 * This function first checks if Pac-Man collides with an object or a fruit:
 * - If Pac-Man collides with an object, it clears the object from the grid, plays a sound effect,
 *   decreases the count of total objects, and increases the scorePacMan.
 * - If Pac-Man collides with a fruit, it clears the fruit from the grid, plays a sound effect,
 *   decreases the count of total objects, increases the scorePacMan, and activates power-up mode.
 * 
 * Then the function checks for collisions with ghosts:
 * - If Pac-Man collides with a ghost:
 *   - If a power-up is active, resets the ghost's position, plays a sound effect,
 *     increases the scorePacMan, and potentially handles object or fruit pickup.
 *   - If no power-up is active, decreases Pac-Man's lifes:
 *     - If lifes drop to zero, sets the game over flag.
 *     - Otherwise, respawns Pac-Man and updates the game state.
 */
void handleCollisionsPacMan() {
  // Check for collision with an object
  if (grid[pacManX][pacManY] & OBJECT) {
    grid[pacManX][pacManY] &= EMPTY;  // Clear object from grid
    tone(BUZZER, 300, 100);           // Play object pickup sound effect
    totalObjects--;                   // Decrease total objects
    scorePacMan += 10;                // Increase scorePacMan for object pickup
  }
  // Check for collision with a fruit
  else if (grid[pacManX][pacManY] & FRUIT) {
    grid[pacManX][pacManY] &= EMPTY;  // Clear object from grid
    tone(BUZZER, NOTE_E5, 100);       // Play object pickup sound effect
    totalObjects--;                   // Decrease total objects
    scorePacMan += 50;                // Increase scorePacMan for object fruit
    switchPowerUp(true);              // Activate power-up mode
  }

  // Check for collision with ghosts
  for (uint8_t i = 0; i < numGhosts; i++) {
    if (pacManX == ghostX[i] && pacManY == ghostY[i]) {
      if (isPowerUpActive()) {
        resetGhostPosition(i);       // Reset ghost's position
        tone(BUZZER, NOTE_E5, 100);  // Play ghost collision sound effect
        scorePacMan += 100;          // Increase scorePacMan for ghost collision in power-up mode

        return;  // Exit function after handling collision and potential object/fruit pickup
      } else {
        gameOverPacMan = true;
        return;  // Exit function after handling collision
      }
    }
  }
}

/**
 * @brief Resets the position of a ghost to its initial position.
 * 
 * @param index The index of the ghost to reset (0-based).
 *              Must be within the range [0, numGhosts - 1].
 */
void resetGhostPosition(uint8_t index) {
  if (index < numGhosts) {                 // Check if index is within valid range
    ghostX[index] = initialGhostX[index];  // Reset ghost's X position to initial value
    ghostY[index] = initialGhostY[index];  // Reset ghost's Y position to initial value
  }
}

/**
 * @brief Resets Pac-Man's position and resets all ghosts for a new life.
 * 
 * This function is called to reset Pac-Man and all ghosts when a new life starts.
 * It sets Pac-Man's position to its initial coordinates (initialPacManX, initialPacManY)
 * and resets all ghosts to their initial positions using resetGhostPosition().
 * 
 * After resetting positions, this function resets the gameOverPacMan flag to false
 * to indicate that the game is not over.
 */
void newLife() {
  // Reset Pac-Man's position to the initial coordinates
  pacManX = initialPacManX;  // Set Pac-Man's X position
  pacManY = initialPacManY;  // Set Pac-Man's Y position

  // Reset all ghosts to their initial positions
  for (uint8_t i = 0; i < numGhosts; i++) {
    resetGhostPosition(i);  // Reset ghost at index i
  }

  gameOverPacMan = false;  // Reset game over flag for Pac-Man
}

/**
 * @brief Checks if all objects have been collected to determine if the player has won.
 * 
 * Checks the total number of objects remaining. If no objects are left:
 * - Deactivates any active power-up.
 * - Sets the winPacMan flag to true.
 * 
 * This function is typically called to determine if the player has completed the current level.
 */
void checkWin() {
  if (totalObjects <= 0) {
    switchPowerUp(false);  // Deactivate any active power-up
    winPacMan = true;            // Set the winPacMan flag to true
  }
}

/**
 * @brief Handles game over conditions and level transitions for Pac-Man.
 * 
 * This function evaluates the game state to determine if the game has ended due to a winPacMan or loss scenario:
 * - If the `winPacMan` flag is true:
 *   - If the current level (`currLevel`) is less than 5:
 *     - Increments `currLevel` to load the next level.
 *     - Displays "LOADING LEVEL: X" using the `displayMessage` function.
 *     - Plays a winner sound effect with the `nextLevelJingle` function.
 *     - Calls `loadLevel()` to load the next level.
 *   - If the current level is 5:
 *     - Increases the `scorePacMan` by adding the product of `lifes` and 100.
 *     - Displays "YOU winPacMan!!!" using the `displayMessage` function.
 *     - Displays the final score using the `displayMessage` function.
 *     - Plays a winning jingle through the `pacManJingle` function.
 *     - Sets `mainMenuPacMan` flag to true to return to the main menu.
 * - If the `gameOverPacMan` flag is true:
 *   - Decrements `lifes` by one to reflect the loss of a life.
 *   - If remaining `lifes` are greater than 0:
 *     - Displays "LIFES REMAINING: X" using the `displayMessage` function.
 *     - Plays a losing jingle through the `loseLifeJingle` function.
 *     - Calls `newLife()` to respawn Pac-Man and update game state.
 *   - If no lives are remaining:
 *     - Resets `currLevel` to 1 to restart the game.
 *     - Displays "Game Over" using the `displayMessage` function.
 *     - Displays the final score using the `displayMessage` function.
 *     - Plays a losing sound effect through the `gameOverJingle` function.
 *     - Sets `mainMenuPacMan` flag to true to return to the main menu.
 * 
 * If `iscabinato` is not defined, the function resets the game state and loads the level again.
 */
void handleGameOverPacMan() {
  // Handle winPacMan scenario
  if (winPacMan) {
    if (currLevel < levelPacMan) {
      // Player wins this level, load next level
      currLevel++;
      String message = "LOADING LEVEL: " + String(currLevel);
      displayMessage(message.c_str(), 15, 25, true);
      nextLevelJinjle();
      loadLevel();
    } else {
      // Player wins the game
      currLevel = 1;
      scorePacMan = scorePacMan + (lifes * 100);
      displayMessage("YOU winPacMan!!!", 37, 25, true);
      String message = "Score: " + String(scorePacMan);
      displayMessage(message.c_str(), 35, 35, false);
      pacManJingle();
      // Set flag to return to main menu
      mainMenuPacMan = true;
    }
  } else if (gameOverPacMan) {
    // Pac-Man loses a life
    lifes--;

    if (lifes > 0) {
      // Player lose one of his lifes, reload current level
      String message = "LIFES REMAINING: " + String(lifes);
      displayMessage(message.c_str(), 7, 25, true);
      loseLifeJinjle();
      newLife();
      return;
    } else {
      // Handle loss scenario
      currLevel = 1;
      displayMessage("Game Over", 37, 25, true);
      String message = "Score: " + String(scorePacMan);
      displayMessage(message.c_str(), 35, 35, false);
      gameOverJinjle();
      // Set flag to return to main menu
      mainMenuPacMan = true;
    }
  }

  delay(2000);
  #ifndef iscabinato
    gameOverPacMan = false;
    winPacMan = false;
    loadLevel();
  #endif
}

/**
 * @brief Plays the melody for the winning scenario.
 * 
 * Iterates through the nextLevelMelody array to play each note.
 * Each note is played with a duration slightly reduced by 10% for a pause effect after each note.
 *
  * The melody is stored in PROGMEM (program memory) to save RAM.
  * Each note is defined with its pitch and duration.
 *
   * The melody array stores pairs of values for each note: pitch and duration.
   * sizeof(melody) returns the size of the array in bytes, and sizeof(melody[0]) returns 
   * the size of the first element in bytes. Dividing by 2 accounts for each note being represented 
   * by two integers (pitch and duration).
 */
const int nextLevelMelody[] PROGMEM = {
  NOTE_E4, 200, NOTE_G4, 200, NOTE_C4, 400,  // Bar 1
  NOTE_F4, 200, NOTE_F4, 200, NOTE_E4, 400,  // Bar 2
  NOTE_D4, 400, NOTE_B3, 800                 // Bar 3
};
void nextLevelJinjle() {
  const int notes = sizeof(nextLevelMelody) / sizeof(nextLevelMelody[0]) / 2;

  for (uint8_t thisNote = 0; thisNote < notes; thisNote++) {
    // Read pitch (note) and duration from PROGMEM (flash memory)
    uint16_t note = pgm_read_word(&nextLevelMelody[thisNote * 2]);              // Read pitch (note)
    uint16_t noteDuration = pgm_read_word(&nextLevelMelody[thisNote * 2 + 1]);  // Read duration

    // Play the note with reduced duration for pause effect
    tone(BUZZER, note, noteDuration * 0.9);
    delay(noteDuration);
    noTone(BUZZER);
  }
}

/**
 * @brief Plays the melody for the losing a life scenario.
 * 
 * Iterates through the loseLifeMelody array to play each note.
 * Each note is played with a duration slightly reduced by 10% for a pause effect after each note.
 *
  * The melody is stored in PROGMEM (program memory) to save RAM.
  * Each note is defined with its pitch and duration.
 *
   * The melody array stores pairs of values for each note: pitch and duration.
   * sizeof(melody) returns the size of the array in bytes, and sizeof(melody[0]) returns 
   * the size of the first element in bytes. Dividing by 2 accounts for each note being represented 
   * by two integers (pitch and duration).
 */
const int loseLifeMelody[] PROGMEM = {
  NOTE_C4, 200, NOTE_A3, 200, NOTE_D4, 400,  // Bar 1
  NOTE_G3, 200, NOTE_G3, 200, NOTE_F3, 400,  // Bar 2
  NOTE_E3, 400, NOTE_C3, 800                 // Bar 3
};
void loseLifeJinjle() {
  const int notes = sizeof(loseLifeMelody) / sizeof(loseLifeMelody[0]) / 2;

  for (uint8_t thisNote = 0; thisNote < notes; thisNote++) {
    // Read pitch (note) and duration from PROGMEM (flash memory)
    uint16_t note = pgm_read_word(&loseLifeMelody[thisNote * 2]);              // Read pitch (note)
    uint16_t noteDuration = pgm_read_word(&loseLifeMelody[thisNote * 2 + 1]);  // Read duration

    // Play the note with reduced duration for pause effect
    tone(BUZZER, note, noteDuration * 0.9);
    delay(noteDuration);
    noTone(BUZZER);
  }
}

/**
 * @brief Plays the Pac-Man theme melody.
 * 
 * This function plays the Pac-Man theme melody using a buzzer. 
 * The melody is stored in PROGMEM (program memory) to save RAM.
 * 
 * Note: Change the tempo variable to adjust the speed of the melody.
 */
void pacManJingle() {
  // Pac-Man theme melody stored in PROGMEM
  int melody[] PROGMEM = {
    // Melody credits: https://e-boombots.com/download/professor-laytons-theme/
    NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16,  //1
    NOTE_B5, 32, NOTE_FS5, -16, NOTE_DS5, 8, NOTE_C5, 16,
    NOTE_C6, 16, NOTE_G6, 16, NOTE_E6, 16, NOTE_C6, 32, NOTE_G6, -16, NOTE_E6, 8,
    NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_B5, 32,  //2
    NOTE_FS5, -16, NOTE_DS5, 8, NOTE_DS5, 32, NOTE_E5, 32, NOTE_F5, 32,
    NOTE_F5, 32, NOTE_FS5, 32, NOTE_G5, 32, NOTE_G5, 32, NOTE_GS5, 32, NOTE_A5, 16, NOTE_B5, 8
  };
  // sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
  // there are two values per note (pitch and duration), so for each note there are four bytes
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // Calculate duration of a whole note in milliseconds based on the tempo
  const int tempo = 105;  // Adjust this to change the tempo (slower or faster)
  const int wholenote = (60000 * 4) / tempo;

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    // Calculate the duration of each note based on its divider
    int divider = melody[thisNote + 1];
    int noteDuration = 0;
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;  // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BUZZER, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);  // Wait for the specified duration before playing the next note
    noTone(BUZZER);       // Stop the waveform generation before the next note
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