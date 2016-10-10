/** @file   communications.c
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions required for communicating between boards via ir
*/

#include "communications.h"
#include "ball.h"
#include "display.h"
#include "ir_uart.h"


char encode_ball_info(Ball * ball)
// Takes a ball struct and encodes it as an ASCII char to represent
// y value and direction. The character can be either numeric, lowercase
// or uppercase, which encodes direction. The ASCII code position within
// its 'group' indicates the y value
// '0' - '6' if travelling forward from one screen to the other
// 'A' - 'G' if travelling forward right
// 'a' - 'g' if travelling forward left
// e.g. 'c' encodes ball->y = 2 and ball->dir = FL
// Returns the encoded character
{
    char ch = START_CHAR;
    if (ball->dir == FR) {
        ch += ALPHA_UPPER;
    } else if (ball->dir == FL) {
        ch += ALPHA_LOWER;
    }
    ch += ball->y;
    return ch;
}


bool decode_info(char ch, Ball * ball)
// Takes a character received over IR and a pointer to a ball struct.
// The character is decoded, and if it indicates the the other player
// has lost, calls the  display_winner function and returns true,
// indicating the game is over.
// Otherwise, the character will be an encoding of a ball struct. This
// character is decoded (see encode_ball_info for information on the
// encoding) and the fields of the ball struct are updated accordingly.
// In this case, it returns false, indicating the game is over.
{
    if (ch == LOSE) {
        display_winner(WIN);
        return true;
    } else {
        ball->x = TRANSMISSION_COL;
        if (ch >= BACK_BOTTOM && ch <= BACK_TOP) {
            ball->dir = B;
            ch -= Y_DIFF_BACK;
        } else if (ch >= BACK_LEFT_BOTTOM && ch <= BACK_LEFT_TOP) {
            ball->dir = BL;
            ch -= Y_DIFF_BACK_LEFT;
        } else if (ch >= BACK_RIGHT_TOP && ch <= BACK_RIGHT_TOP) {
            ball->dir = BR;
            ch -= Y_DIFF_BACK_RIGHT;
        }
        ball->y = NUM_ROWS - ch;
    }
    return false;
}


void transmit_ball_info(Ball * ball)
// Sends encoded ball struct over IR, moves the ball off the screen so
// that it is not displayed, and makes it stationary
{
    char ch_transmit = encode_ball_info(ball);
    ir_uart_putc(ch_transmit);
    move_ball_off_screen(ball);
}


void receive_char(Ball * ball, bool * game_over)
// Polls the uart if the ball is not in play, and decodes character if
// it is received 
{
    char ch_received = '\0';
    if (ball->x == OUT_OF_PLAY && ball->y == OUT_OF_PLAY && ir_uart_read_ready_p()) {
        ch_received = ir_uart_getc();
        *game_over = decode_info(ch_received, ball);
    }
}
