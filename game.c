/** @file   game.c
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  This implements a wicked pong game
*/

#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "delay.h"
#include "ir_uart.h"
#include "button.h"
#include "led.h"

/* This program implements a pong game. When initialised, a welcome
   message appears, and the program awaits a push of the navswitch.
   The first player to push the navswitch starts with the ball on their
   paddle, and can fire it off in whichever direction they choose.
   The aim is then to block the ball from reaching the end of your screen.
   If it does, a W or L will be displayed to indicate win or loss.
   This message is displayed for a brief time, and the welcome message
   comes on again.
   
   The ball speed can be adjusted from the welcome screen. Simply press
   the other button. The brightness of the blue LED indicates the selected
   game speed.
   Players can even play at different speed simultaneously, to handicap
   one player for ultimate humiliation! 
*/



/* Define polling rates in Hz.  */
#define MOVE_PADDLE_RATE 2
#define DISPLAY_TASK_RATE 250
#define MESSAGE_RATE 10

#define BallPlaced 'P'

typedef enum { F, B, FL, FR, BL, BR, S } Dir;

typedef enum { Left, Right } P_Dir;

typedef struct paddle_s {
    uint8_t left;
    uint8_t mid;
    uint8_t right;
} Paddle;

typedef struct ball_s {
    uint8_t x;
    uint8_t y;
    Dir dir;
} Ball;


void move_paddle(Paddle * paddle, P_Dir dir)
// Takes a pointer to a paddle struct and moves the entire paddle one
// square up or one square down in the given direction. The paddle will
// not move if you try to push it over the edge of the screen
{
    if (dir == Left && paddle->left != 6) {
        paddle->left++;
        paddle->mid++;
        paddle->right++;
    } else if (dir == Right && paddle->right != 0) {
        paddle->left--;
        paddle->mid--;
        paddle->right--;
    }
}


void move_paddle_task(Paddle * paddle)
// Takes a pointer to a paddle and polls the navswitch to see if player
// wants to move the paddle. If so, it moves the paddle according to
// whether the navswitch is pressed north or south
{
    navswitch_update();

    if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        move_paddle(paddle, Left);
    }
    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        move_paddle(paddle, Right);
    }
}


void continue_straight(Ball * ball)
// Moves ball to next square according to its current direction
{
    if (ball->dir == F) {
        ball->x--;
    } else if (ball->dir == B) {
        ball->x++;
    } else if (ball->dir == FL) {
        ball->x--;
        ball->y++;
    } else if (ball->dir == FR) {
        ball->x--;
        ball->y--;
    } else if (ball->dir == BL) {
        ball->x++;
        ball->y++;
    } else if (ball->dir == BR) {
        ball->x++;
        ball->y--;
    }
}

void wait(void)
// Waits for a brief time while win/lose message is displayed
{
    uint16_t i = 0;
    while (i <= 1000) {
        pacer_wait();
        tinygl_update();
        i++;
    }
}


void display_char(char ch)
// Takes a W or L, displays that character on the screen and sends 'L'
// over IR if player has lost
{
    tinygl_point_t display_square = { 0, 4 };
    if (ch == 'W') {
        tinygl_draw_char(ch, display_square);
    } else {
        ir_uart_putc(ch);
        tinygl_draw_char(ch, display_square);
    }
    wait();
}

void display_winner(char ch)
// Displays a W if player has won, or L if player has lost
{
    tinygl_clear();
    tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);
    display_char(ch);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
}


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
    char ch = '0';
    if (ball->dir == FR) {
        ch += 17;
    } else if (ball->dir == FL) {
        ch += 49;
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
    if (ch == 'L') {
        display_winner('W');
        return true;
    } else {
        ball->x = 0;
        if (ch >= '0' && ch <= '6') {
            ball->dir = B;
            ch -= 48;
        } else if (ch >= 'A' && ch <= 'G') {
            ball->dir = BL;
            ch -= 65;
        } else if (ch >= 'a' && ch <= 'g') {
            ball->dir = BR;
            ch -= 97;
        }
        ball->y = 6 - ch;
    }
    return false;
}


void move_ball_off_screen(Ball * ball)
// Takes a pointer to a ball, moves it off the screen and makes it
// stationary
{
    ball->x = 10;
    ball->y = 10;
    ball->dir = S;
}


void transmit_ball_info(Ball * ball)
// Sends encoded ball struct over IR, moves the ball off the screen so
// that it is not displayed, and makes it stationary
{
    char ch_transmit = encode_ball_info(ball);
    ir_uart_putc(ch_transmit);
    move_ball_off_screen(ball);
}


void bounce_off_wall(Ball * ball)
// Takes a pointer to a ball struct which will represent a ball that is
// at a wall (i.e. y is 0 or 6) and updates the ball's position and
// direction according to our logic for wall bouncing
{
    if (ball->y == 0) {
        if (ball->dir == BR) {
            ball->x++;
            ball->y++;
            ball->dir = BL;
        } else if (ball->dir == FR) {
            ball->x--;
            ball->y++;
            ball->dir = FL;
        }
    } else if (ball->y == 6) {
        if (ball->dir == BL) {
            ball->x++;
            ball->y--;
            ball->dir = BR;
        } else if (ball->dir == FL) {
            ball->x--;
            ball->y--;
            ball->dir = FR;
        }
    }
}


void move_ball_top(Ball * ball)
// Takes a pointer to a ball struct which represents a ball at the top
// of the screen. The ball is either sent to the other player's screen
// if it is travelling forward, or it moves a square back, according to
// its current direction
{
    if (ball->dir == FL) {
        if (ball->y == 6) {
            ball->y--;
            ball->dir = FR;
        } else {
            ball->y++;
        }
        transmit_ball_info(ball);
    } else if (ball->dir == FR) {
        if (ball->y == 0) {
            ball->y++;
            ball->dir = FL;
        } else {
            ball->y--;
        }
        transmit_ball_info(ball);
    } else if (ball->dir == F) {
        transmit_ball_info(ball);
    } else if (ball->y == 0 || ball->y == 6) {
        bounce_off_wall(ball);
    } else {
        continue_straight(ball);
    }
}

void move_ball_bottom(Ball * ball, Paddle paddle)
// Takes a pointer to a ball struct which represents a ball at the bottom
// of the screen, and a paddle. If the ball should bounce off the paddle,
// its position and direction will be updated accordingly. Otherwise,
// ball will continue straight (and you will lose the game!)
{
    if (ball->y == paddle.left
        || (ball->y == paddle.right && ball->y == 0)) {
        ball->x--;
        ball->y++;
        ball->dir = FL;
    } else if (ball->y == paddle.mid) {
        ball->x--;
        ball->dir = F;
    } else if (ball->y == paddle.right
               || (ball->y == paddle.left && ball->y == 6)) {
        ball->x--;
        ball->y--;
        ball->dir = FR;
    } else if (ball->y == 0 || ball->y == 6) {
        bounce_off_wall(ball);
    } else {
		continue_straight(ball);
	}
}


void move_ball(Ball * ball, Paddle paddle)
// Takes a pointer to a ball struct and a paddle, and updates the ball
// position and direction
{
    if (ball->dir == S) {
        // Stationary, don't move
    } else if (ball->x > 0 && ball->x < 3 && ball->y > 0 && ball->y < 6) {
        continue_straight(ball);
    } else if (ball->x == 0) {
        move_ball_top(ball);
    } else if (ball->x == 3) {
        move_ball_bottom(ball, paddle);
    } else {
        bounce_off_wall(ball);
    }
}


void display_task(Paddle paddle, Ball ball)
// Takes a paddle and a ball, and displays them on the screen
{
    tinygl_point_t paddle_left = { 4, paddle.left };
    tinygl_point_t paddle_right = { 4, paddle.right };
    tinygl_point_t ball_pos = { ball.x, ball.y };

    tinygl_clear();
    tinygl_draw_line(paddle_left, paddle_right, 1);
    tinygl_draw_point(ball_pos, 1);
    tinygl_update();
}

void graphics_init(void)
// Initialises tinygl functionality
{
    tinygl_font_set(&font3x5_1);
    tinygl_text_speed_set(MESSAGE_RATE);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set(TINYGL_TEXT_DIR_ROTATE);
}

void ball_init(Ball * ball)
// Takes a pointer to a ball struct, moves it back to its starting
// position in the middle of the screen and makes it stationary
{
    ball->x = 3;
    ball->y = 3;
    ball->dir = S;
}

void paddle_init(Paddle * paddle)
// Takes a pointer to a paddle struct and moves it back to its starting
// position in the middle of the screen
{
    paddle->left = 4;
    paddle->mid = 3;
    paddle->right = 2;
}

uint16_t update_speed(uint16_t speed)
// Takes the current speed selection and toggles it to the next speed
// selection. Returns the new speed
{
    if (speed == 100) {
        speed = 50;
        led_set(0, 1);
    } else if (speed == 50) {
        speed = 30;
        led_set(0, 1);
    } else if (speed == 30) {
        speed = 100;
        led_set(0, 0);
    }
    return speed;
}


void dim_led(uint16_t speed)
// If speed is medium, toggles the led on successive iterations through
// the while loop such that led appears dimmed
{
	static uint8_t toggle = 0;
    if (speed == 50) {
        if (toggle == 0) {
            led_set(0, toggle);
            toggle = 1;
        } else {
            led_set(0, toggle);
            toggle = 0;
        }
    }
}


uint16_t wait_for_start(bool * ball_placed, uint16_t speed)
// Display welcome message and poll pushbutton to see if player is ready
// to start. Also polls usart to see if opponent has placed ball
{
    tinygl_clear();
    tinygl_text("PONG");
    char ch = '\0';

    while (!navswitch_push_event_p(NAVSWITCH_PUSH)) {
        pacer_wait();
        tinygl_update();
        navswitch_update();
        button_update();
        if (ir_uart_read_ready_p()) {
            ch = ir_uart_getc();
            if (ch == BallPlaced) {
                *ball_placed = true;
            }
        }
        if (button_push_event_p(0)) {
            speed = update_speed(speed);
        }
        dim_led(speed);
    }
    return speed;
}


void receive_char(Ball * ball, bool * game_over)
// Polls the uart if the ball is not in play, and decodes character if
// it is received 
{
	char ch_received = '\0';
    if (ball->x == 10 && ball->y == 10 && ir_uart_read_ready_p()) {
        ch_received = ir_uart_getc();
        *game_over = decode_info(ch_received, ball);
    }
}

void serve_ball(Ball * ball)
// If the ball is stationary at the start of the game and the navswitch
// is pushed, the ball is served
{
    if (ball->dir == S && navswitch_push_event_p(NAVSWITCH_PUSH)) {
        ball->dir = F;
    }
}


void check_for_loss(Ball* ball, bool* game_over, bool* ball_placed)
// Takes a pointer to a ball and checks if it is in line with the paddle.
// If so, player has lost the game and 'L' is displayed
{
    if (ball->x == 4) {
        *game_over = true;
        *ball_placed = false;
        display_winner('L');
    }
}


int main(void)
{
    system_init();
    navswitch_init();
    led_init();
    led_set(0, 0);
    button_init();
    pacer_init(300);
    tinygl_init(300);
    ir_uart_init();
    graphics_init();
    uint8_t count = 0;
    Paddle paddle;
    Ball ball;
    bool game_over = true;
    bool ball_placed = false;
    uint16_t speed = 100;

    while (1) {
        pacer_wait();
        if (game_over) {
            speed = wait_for_start(&ball_placed, speed);
            if (!ball_placed) {
                ball_init(&ball);
                ir_uart_putc(BallPlaced);
            } else {
				move_ball_off_screen(&ball);
            }
            game_over = false;
            paddle_init(&paddle);
        } else {
            tinygl_clear();
            move_paddle_task(&paddle);
            receive_char(&ball, &game_over);
            display_task(paddle, ball);
            serve_ball(&ball);
            if (count % speed == 0) {
                move_ball(&ball, paddle);
                count = 0;
            }
            count++;
            check_for_loss(&ball, &game_over, &ball_placed);
        }
    }
}
