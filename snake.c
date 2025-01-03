// Ryan Putka
// Fundamentals of Computing
// Final Project - Dec 15
//
// This program simulates the game "Snake" using the X11 graphics library. Use W (up), A (left), S (down), and D (right) to move the snake around the screen, collecting as many apples as possible without running into the wall/border or intersecting yourself. The objective is to continue eating apples until the snake fills the entire screen. Good luck!

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "gfx2.h"

#define GRID_WIDTH 40
#define GRID_HEIGHT 30
#define CELL_SIZE 20
#define WINDOW_WIDTH (GRID_WIDTH * CELL_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT * CELL_SIZE)

// Directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// Coordinate struct
typedef struct {
    int x;
    int y;
} Point;

// Snake attributes struct
typedef struct {
    Point* body;
    int length;
    int max_length;
    int direction;
} Snake;

// Current state of game struct
typedef struct {
    Snake snake;
    Point food;
    int game_over;
    int score;
} GameState;

// Function prototypes
void init_snake(Snake* snake);
void spawn_food(GameState* gameState);
void draw_game(const GameState gameState);
int move_snake(GameState* gameState);
void handle_input(GameState* gameState, char input);
void cleanup_game_state(GameState* gameState);

int main() {
    
    // Open graphics window
    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake");
    
    // Initialize game state
    GameState gameState;
    gameState.score = 0;
    gameState.game_over = 0;
    
    // Initialize snake and food
    init_snake(&gameState.snake);
    spawn_food(&gameState);
    
    // Game loop
    while (!gameState.game_over) {
        // Check for change of direction
        if (gfx_event_waiting()) {
            char c = gfx_wait();
            handle_input(&gameState, c);
        }
        
        // Move snake and check for game over
        gameState.game_over = move_snake(&gameState);
        
        // Draw game
        draw_game(gameState);
        
        usleep(100000);
    }
    
    // Game over screen
    gfx_clear();
    gfx_color(255, 255, 255);
    char game_over_text[50];
    snprintf(game_over_text, sizeof(game_over_text), "Game Over! Score: %d", gameState.score);
    gfx_text(WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2, game_over_text);
	gfx_text(WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2 + 15, "Press 'q' to quit");
    gfx_flush();
    
    gfx_wait();
    
    return 0;
}

// Function to create snake
void init_snake(Snake* snake) {
    snake->max_length = GRID_WIDTH * GRID_HEIGHT;
    snake->body = malloc(sizeof(Point) * snake->max_length);
    snake->length = 1;
    snake->direction = RIGHT;
    
    // Place snake in the middle of the screen
    snake->body[0].x = GRID_WIDTH / 2;
    snake->body[0].y = GRID_HEIGHT / 2;
}

// Function to create food
void spawn_food(GameState* gameState) {
    while(1){
        gameState->food.x = rand() % GRID_WIDTH;
        gameState->food.y = rand() % GRID_HEIGHT;
        
        // Check that food doesn't spawn on snake body
        int collision = 0;
        for (int i = 0; i < gameState->snake.length; i++) {
            if (gameState->food.x == gameState->snake.body[i].x && 
                gameState->food.y == gameState->snake.body[i].y) {
                collision = 1;
                break;
            }
        }
        
        if (!collision) break;
    }
}

// Drawing game
void draw_game(const GameState gameState) {
   	gfx_clear();
 
    gfx_color(0, 255, 0); // Green for snake
    
    // Draw snake body
    for (int i = 0; i < gameState.snake.length; i++) {
        int x = gameState.snake.body[i].x * CELL_SIZE;
        int y = gameState.snake.body[i].y * CELL_SIZE;
        gfx_fill_circle(x + CELL_SIZE/2, y + CELL_SIZE/2, CELL_SIZE/2 - 1);
    }
    
    // Draw food
    gfx_color(255, 0, 0); // Red for food
    int food_x = gameState.food.x * CELL_SIZE;
    int food_y = gameState.food.y * CELL_SIZE;
    gfx_fill_circle(food_x + CELL_SIZE/2, food_y + CELL_SIZE/2, CELL_SIZE/2 - 1);
    
    // Draw score
    gfx_color(255, 255, 255); // White for text
    char score_text[20];
    snprintf(score_text, sizeof(score_text), "Score: %d", gameState.score);
    gfx_text(10, 20, score_text);

	gfx_flush();

}

// Function to move the snake
int move_snake(GameState* gameState) {
    Snake* snake = &(gameState->snake);
    
    // Move each segment (circle) of the body
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i-1];
    }
    
    // Move head (first circle) based on current direction
    switch(snake->direction) {
        case UP:
            snake->body[0].y--;
            break;
        case RIGHT:
            snake->body[0].x++;
            break;
        case DOWN:
            snake->body[0].y++;
            break;
        case LEFT:
            snake->body[0].x--;
            break;
    }
    
    // Check if snake eats food
    if (snake->body[0].x == gameState->food.x && 
        snake->body[0].y == gameState->food.y) {
        // Increase snake length
        if (snake->length < snake->max_length) {
            snake->length++;
            gameState->score++;
            spawn_food(gameState);
        }
    }
    
    // Check if snake hits border/wall
    if (snake->body[0].x < 0 || snake->body[0].x >= GRID_WIDTH ||
        snake->body[0].y < 0 || snake->body[0].y >= GRID_HEIGHT) {
        return 1; // Game over
    }
    
    // Check for self-collision
    for (int i = 1; i < snake->length; i++) {
        if (snake->body[0].x == snake->body[i].x && 
            snake->body[0].y == snake->body[i].y) {
            return 1; // Game over
        }
    }
    
    return 0; // Continue game
}

// Handling input for change of direction + quit
void handle_input(GameState* gameState, char input) {
    Snake* snake = &(gameState->snake);
    
    switch(input) {
        case 'w': // Up
            if (snake->direction != DOWN)
                snake->direction = UP;
            break;
        case 'd': // Right
            if (snake->direction != LEFT)
                snake->direction = RIGHT;
            break;
        case 's': // Down
            if (snake->direction != UP)
                snake->direction = DOWN;
            break;
        case 'a': // Left
            if (snake->direction != RIGHT)
                snake->direction = LEFT;
            break;
        case 'q': // Quit
            gameState->game_over = 1;
            break;
    }
}

