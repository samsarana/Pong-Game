/** @file   communications.h
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions required for communicating between boards via ir
*/


#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H


#define START_CHAR '0'
#define ALPHA_UPPER 17
#define ALPHA_LOWER 49
#define TRANSMISSION_COL 0
#define BACK_BOTTOM '0'
#define BACK_TOP '6'
#define BACK_LEFT_BOTTOM 'A'
#define BACK_LEFT_TOP 'G'
#define BACK_RIGHT_TOP 'a'
#define BACK_RIGHT_TOP 'g'
#define Y_DIFF_BACK 48
#define Y_DIFF_BACK_LEFT 65
#define Y_DIFF_BACK_RIGHT 97
#define OUT_OF_PLAY 10
#define NUM_ROWS 6
#define WIN 'W'
#define LOSE 'L'

char encode_ball_info(Ball * ball);
// Takes a ball struct and encodes it as an ASCII char to represent
// y value and direction. The character can be either numeric, lowercase
// or uppercase, which encodes direction. The ASCII code position within
// its 'group' indicates the y value
// '0' - '6' if travelling forward from one screen to the other
// 'A' - 'G' if travelling forward right
// 'a' - 'g' if travelling forward left
// e.g. 'c' encodes ball->y = 2 and ball->dir = FL
// Returns the encoded character


bool decode_info(char ch, Ball * ball);
// Takes a character received over IR and a pointer to a ball struct.
// The character is decoded, and if it indicates the the other player
// has lost, calls the  display_winner function and returns true,
// indicating the game is over.
// Otherwise, the character will be an encoding of a ball struct. This
// character is decoded (see encode_ball_info for information on the
// encoding) and the fields of the ball struct are updated accordingly.
// In this case, it returns false, indicating the game is over.


void transmit_ball_info(Ball * ball);
// Sends encoded ball struct over IR, moves the ball off the screen so
// that it is not displayed, and makes it stationary


void receive_char(Ball * ball, bool * game_over);
// Polls the uart if the ball is not in play, and decodes character if
// it is received 


#endif /* COMMUNICATIONS_H  */
