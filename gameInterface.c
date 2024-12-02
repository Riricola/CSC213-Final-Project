#include <curses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

// #include "util.h"
// #include "scheduler.h"


// Defines used to track the worm direction
#define DIR_NORTH 0
#define DIR_EAST 1
#define DIR_SOUTH 2
#define DIR_WEST 3

// Game parameters

#define DRAW_BOARD_INTERVAL 33
#define APPLE_UPDATE_INTERVAL 120
#define READ_INPUT_INTERVAL 150
#define GENERATE_APPLE_INTERVAL 2000
#define BOARD_WIDTH 50
#define BOARD_HEIGHT 25


// Player Parameters
int score = 0;
enum states {STAY, HIT, BUST}; 
//** If state = bust, skip. If state = hit, draw a card. If state = stay, skip player */
// Once all players stay or are busted, reveal points of the board

/**
 * In-memory representation of the game board
 * Zero represents an empty cell
 * Positive numbers represent worm cells (which count up at each time step until they reach
 * worm_length) Negative numbers represent apple cells (which count up at each time step)
 */
int board[BOARD_HEIGHT][BOARD_WIDTH];

// Is the game running?
bool running = true;

/**
 * Convert a board row number to a screen position
 * \param   row   The board row number to convert
 * \return        A corresponding row number for the ncurses screen
 */
int screen_row(int row) {
  return 2 + row;
}

/**
 * Convert a board column number to a screen position
 * \param   col   The board column number to convert
 * \return        A corresponding column number for the ncurses screen
 */
int screen_col(int col) {
  return 2 + col;
}

/**
 * Initialize the board display by printing the title and edges
 */
void init_display() {
  // Print Title Line
  move(screen_row(-2), screen_col(BOARD_WIDTH / 2 - 5));
  addch(ACS_DIAMOND);
  addch(ACS_DIAMOND);
  printw(" BlackJack! ");
  addch(ACS_DIAMOND);
  addch(ACS_DIAMOND);

  // Print corners
  mvaddch(screen_row(-1), screen_col(-1), ACS_ULCORNER);
  mvaddch(screen_row(-1), screen_col(BOARD_WIDTH), ACS_URCORNER);
  mvaddch(screen_row(BOARD_HEIGHT), screen_col(-1), ACS_LLCORNER);
  mvaddch(screen_row(BOARD_HEIGHT), screen_col(BOARD_WIDTH), ACS_LRCORNER);

  // Print top and bottom edges
  for (int col = 0; col < BOARD_WIDTH; col++) {
    mvaddch(screen_row(-1), screen_col(col), ACS_HLINE);
    mvaddch(screen_row(BOARD_HEIGHT), screen_col(col), ACS_HLINE);
  }

  // Print left and right edges
  for (int row = 0; row < BOARD_HEIGHT; row++) {
    mvaddch(screen_row(row), screen_col(-1), ACS_VLINE);
    mvaddch(screen_row(row), screen_col(BOARD_WIDTH), ACS_VLINE);
  }

  // Refresh the display
  //** we robably dont refresh until a card it drawn */
  //refresh();
}


/**
 * Show a game over message and wait for a key press.
 */
void end_game() {
  mvprintw(screen_row(BOARD_HEIGHT / 2) - 1, screen_col(BOARD_WIDTH / 2) - 6, "            ");
  mvprintw(screen_row(BOARD_HEIGHT / 2), screen_col(BOARD_WIDTH / 2) - 6, " You Lose! ");
  
  // Might have to make a separate "You Win" screen, and add conditionals to check

  mvprintw(screen_row(BOARD_HEIGHT / 2) + 1, screen_col(BOARD_WIDTH / 2) - 6, "            ");
  mvprintw(screen_row(BOARD_HEIGHT / 2) + 2, screen_col(BOARD_WIDTH / 2) - 11,
           "Press any key to exit.");
  refresh();
  timeout(-1);
  //task_readchar();
}

/**
 * Run in a task to draw the current state of the game board.
 */
void draw_board() {
  while (running) {
    // Loop over cells of the game board
    for (int r = 0; r < BOARD_HEIGHT; r++) {
      for (int c = 0; c < BOARD_WIDTH; c++) {
        if (board[r][c] == 0) {  // Draw blank spaces
          mvaddch(screen_row(r), screen_col(c), ' ');
        } else if (board[r][c] > 0) {  // Draw worm
          mvaddch(screen_row(r), screen_col(c), 'O');
        } else {  // Draw apple spinner thing
          char spinner_chars[] = {'|', '/', '-', '\\'};
          mvaddch(screen_row(r), screen_col(c), spinner_chars[abs(board[r][c] % 4)]);
        }
      }
    }

    // Draw the score
    mvprintw(screen_row(-2), screen_col(BOARD_WIDTH - 9), "Score %03d\r",
             21);

    //** Uncomment the below if we want to update the screen */

    // Refresh the display
    //refresh();

    // Sleep for a while before drawing the board again
    //task_sleep(DRAW_BOARD_INTERVAL);
  }
}

/**
 * Run in a task to process user input.
 */
/*
void read_input() {
  while (running) {
    // Read a character, potentially blocking this task until a key is pressed
    int key = task_readchar();

    // Make sure the input was read correctly
    if (key == ERR) {
      running = false;
      fprintf(stderr, "ERROR READING INPUT\n");
    }

    // Handle the key press
        if (key == 's') {
            return;
            // skip them
        } else if (key == 'h') {
            return;
            // draw a card
            // update the screen and score
        } else if (key == 'q'){
            return;
            running = false;
            // exit the game,, forfeit
        }
  }
}
*/

// Entry point: Set up the game, create jobs, then run the scheduler
int main(void) {
  // Initialize the ncurses window
  WINDOW* mainwin = initscr();
  if (mainwin == NULL) {
    fprintf(stderr, "Error initializing ncurses.\n");
    exit(2);
  }

  // Seed random number generator with the time in milliseconds
  //srand(time_ms());

  noecho();                // Don't print keys when pressed
  nodelay(mainwin, true);  // Non-blocking keyboard access

  // Initialize the game display
  init_display();

  // Zero out the board contents
  memset(board, 0, BOARD_WIDTH * BOARD_HEIGHT * sizeof(int));

    // Put the worm at the middle of the board
    //board[BOARD_HEIGHT / 2][BOARD_WIDTH / 2] = 1;

  // Task handles for each of the game tasks
  //task_t draw_board_task;
  //task_t read_input_task;
  

  // Create tasks for each task in the game
  //task_create(&draw_board_task, draw_board);
  //task_create(&read_input_task, read_input);
  

  // Wait for these tasks to exit
  //task_wait(draw_board_task);
  //task_wait(read_input_task);


  // Display the end of game message and wait for user input
  end_game();

  // Clean up window
  delwin(mainwin);
  endwin();

  return 0;
}
