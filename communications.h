/** @file   communications.h
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions required for communicating between boards via ir
*/


#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

#include "ball.h"
#include "system.h"

#define START_CHAR '0'
#define ALPHA_UPPER 17
#define ALPHA_LOWER 49
#define TRANSMISSION_COL 0
#define BACK_BOTTOM '0'
#define BACK_TOP '6'
#define BACK_LEFT_BOTTOM 'A'
#define BACK_LEFT_TOP 'G'
#define BACK_RIGHT_BOTTOM 'a'
#define BACK_RIGHT_TOP 'g'
#define Y_DIFF_BACK 48
#define Y_DIFF_BACK_LEFT 65
#define Y_DIFF_BACK_RIGHT 97
#define OUT_OF_PLAY 10
#define NUM_ROWS 6
#define WIN 'W'
#define LOSE 'L'


void transmit_ball_info(Ball * ball);
// Sends encoded ball struct over IR, moves the ball off the screen so
// that it is not displayed, and makes it stationary


void receive_char(Ball * ball, bool * game_over);
// Polls the uart if the ball is not in play, and decodes character if
// it is received 


#endif /* COMMUNICATIONS_H  */
