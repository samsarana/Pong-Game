/** @file   ball.h
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions for ball movement and interactions
*/


#ifndef BALL_H
#define BALL_H

#include "system.h"
#include "paddle.h"

#define OUT_OF_PLAY 10
#define LEFT_WALL 6
#define RIGHT_WALL 0
#define	TRANSMISSION_COL 0
#define BOUNCE_COL 3
#define BALL_START 3
#define LOSE_COL 4


typedef enum { F, B, FL, FR, BL, BR, S } Dir;

typedef struct ball_s {
    uint8_t x;
    uint8_t y;
    Dir dir;
} Ball;


void move_ball_off_screen(Ball * ball);
// Takes a pointer to a ball, moves it off the screen and makes it
// stationary


void continue_straight(Ball * ball);
// Moves ball to next square according to its current direction


void bounce_off_wall(Ball * ball);
// Takes a pointer to a ball struct which will represent a ball that is
// at a wall (i.e. y is 0 or 6) and updates the ball's position and
// direction according to our logic for wall bouncing


void move_ball_top(Ball * ball);
// Takes a pointer to a ball struct which represents a ball at the top
// of the screen. The ball is either sent to the other player's screen
// if it is travelling forward, or it moves a square back, according to
// its current direction


void move_ball_bottom(Ball * ball, Paddle paddle);
// Takes a pointer to a ball struct which represents a ball at the bottom
// of the screen, and a paddle. If the ball should bounce off the paddle,
// its position and direction will be updated accordingly. Otherwise,
// ball will continue straight (and you will lose the game!)


void move_ball(Ball * ball, Paddle paddle);
// Takes a pointer to a ball struct and a paddle, and updates the ball
// position and direction


void ball_init(Ball * ball);
// Takes a pointer to a ball struct, moves it back to its starting
// position in the middle of the screen and makes it stationary


void serve_ball(Ball * ball);
// If the ball is stationary at the start of the game and the navswitch
// is pushed, the ball is served


void check_for_loss(Ball* ball, bool* game_over, bool* ball_placed);
// Takes a pointer to a ball and checks if it is in line with the paddle.
// If so, player has lost the game and 'L' is displayed

#endif /* BALL_H  */
