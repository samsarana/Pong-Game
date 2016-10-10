/** @file   graphics.h
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions for LED matrix manipulation
*/


#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "paddle.h"
#include "ball.h"

#define MESSAGE_RATE 10
#define WAIT_TIME 1000
#define MIDDLE_SQUARE 0, 4
#define WIN 'W'
#define LOSE 'L'
#define PADDLE_COL 4
#define LED_ON 1


void wait(void);
// Waits for a brief time while win/lose message is displayed


void display_char(char ch);
// Takes a W or L, displays that character on the screen and sends 'L'
// over IR if player has lost


void display_winner(char ch);
// Displays a W if player has won, or L if player has lost


void display_task(Paddle paddle, Ball ball);
// Takes a paddle and a ball, and displays them on the screen


void graphics_init(void);
// Initialises tinygl functionality


#endif /* GRAPHICS_H  */
