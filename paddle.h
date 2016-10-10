/** @file   paddle.h
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions for paddle movement
*/

#ifndef PADDLE_H
#define PADDLE_H

#include "system.h"

#define LEFT_WALL 6
#define RIGHT_WALL 0
#define PADDLE_START_LEFT 4
#define PADDLE_START_MID 3
#define PADDLE_START_RIGHT 2


typedef enum { Left, Right } P_Dir;


typedef struct paddle_s {
    uint8_t left;
    uint8_t mid;
    uint8_t right;
} Paddle;


void move_paddle(Paddle * paddle, P_Dir dir);
// Takes a pointer to a paddle struct and moves the entire paddle one
// square up or one square down in the given direction. The paddle will
// not move if you try to push it over the edge of the screen


void move_paddle_task(Paddle * paddle);
// Takes a pointer to a paddle and polls the navswitch to see if player
// wants to move the paddle. If so, it moves the paddle according to
// whether the navswitch is pressed north or south


void paddle_init(Paddle * paddle);
// Takes a pointer to a paddle struct and moves it back to its starting
// position in the middle of the screen


#endif /* PADDLE_H  */
