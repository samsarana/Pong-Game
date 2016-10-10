#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "delay.h"
#include "ir_uart.h"
#include "button.h"
#include "led.h"


/* Define polling rates in Hz.  */
#define MOVE_PADDLE_RATE 2
#define DISPLAY_TASK_RATE 250
#define MESSAGE_RATE 10

#define BallPlaced 'P'

typedef enum {F, B, FL, FR, BL, BR, S} Dir;

typedef enum {Left, Right} P_Dir;

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
	
	
void move_paddle(Paddle* paddle, P_Dir dir)
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


void move_paddle_task(Paddle* paddle)
{
	navswitch_update ();

	if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
		move_paddle(paddle, Left);
	}
	if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
		move_paddle(paddle, Right);
	}
}

void continue_straight(Ball* ball)
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

void finish(char ch)
{
	tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
	tinygl_clear();
	tinygl_point_t display_square = {0,4};
	if (ch == 'W') {
		tinygl_draw_char (ch, display_square);
	} else {
		ir_uart_putc(ch);
		tinygl_draw_char (ch, display_square);
	}
	int i = 0;
	while (i <= 1000){
		pacer_wait();
		tinygl_update();
		i++;
	}
	tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
}

char encode_ball_info(Ball* ball)
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

bool decode_ball_info(char ch, Ball* ball)
// Decodes character transmitted over IR and updates fields of ball
// struct accordingly
{
	if (ch == 'L') {
		finish('W');
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

void transmit_ball_info(Ball * ball)
// Sends encoded ball struct over IR and moves the ball off the screen
{
    char ch_transmit = encode_ball_info(ball);
    ir_uart_putc(ch_transmit);
    ball->x = 10;
    ball->y = 10;
    ball->dir = S;
}

void edge_case_y(Ball* ball)
{
	if (ball->y == 0){
		if (ball->dir == BR) {
			ball->x++;
			ball->y++;
			ball->dir = BL;
		} else if (ball->dir == FR) {
			ball->x--;
			ball->y++;
			ball->dir = FL;
		}
	} else if (ball->y == 6){
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

void move_ball(Ball* ball, Paddle paddle)
{
	if (ball->dir == S) {
		// Stationary, don't move
	} else if (ball->x > 0 && ball->x < 3 && ball->y > 0 && ball->y < 6) {
		continue_straight(ball);
	} else if (ball->x == 0) {
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
		} else if (ball->y == 0 || ball->y == 6){
			edge_case_y(ball);
		} else {
			continue_straight(ball);
		}
	} else if (ball->x == 3) {
		if (ball->y == paddle.left || (ball->y == paddle.right && ball->y == 0)) {
			ball->x--;
			ball->y++;
			ball->dir = FL;
		} else if (ball->y == paddle.mid) {
			ball->x--;
			ball->dir = F;
		} else if (ball->y == paddle.right || (ball->y == paddle.left && ball->y == 6)) {
			ball->x--;
			ball->y--;
			ball->dir = FR;
		} else {
			continue_straight(ball);
		}
	} else {
		edge_case_y(ball);
	}
}
/*

void check_for_loss(Ball ball, bool * game_over)
// increment other player's points instead of setting game_over
// and then check if points == 3. If so, game_over = true.
{
    if (ball.x == 4) {
        game_over = true;
        ball_placed = false;
    }
}

*/


void display_task (Paddle paddle, Ball ball)
{
	tinygl_point_t pos1 = {4, paddle.left};
	tinygl_point_t pos2 = {4, paddle.right};
	tinygl_point_t ball_pos = {ball.x, ball.y};
	
	tinygl_clear();
	tinygl_draw_line (pos1, pos2, 1);
	tinygl_draw_point (ball_pos, 1);
	tinygl_update ();
}

void graphics_init(void)
{
	tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
}

void ball_init(Ball* ball)
{
	ball->x = 3;
	ball->y = 3;
	ball->dir = S;
}

void paddle_init(Paddle* paddle)
{
	paddle->left = 4;
	paddle->mid = 3;
	paddle->right = 2;
}

uint16_t update_speed(uint16_t speed)
// Takes the current speed selection and toggles it to the next speed
// selection. Returns new speed
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

uint16_t wait_for_start(bool* ball_placed, uint16_t speed)
// Display welcome message and poll pushbutton to see if player is ready
// to start. Also poll usart to see if opponent has placed ball
{
    tinygl_clear();
    tinygl_text("PONG");
    char ch;
    int i = 0;
    
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
        if (button_push_event_p (0)) {
			speed = update_speed(speed);
		}
		if (speed == 50) {
			if (i == 0){
				led_set(0, i);
				i = 1;
			} else {
				led_set(0, i);
				i = 0;
			}
		}
    }
    return speed;
}


int main (void)
{
    system_init ();
    navswitch_init();
    led_init();
    led_set(0, 0);
    button_init();
    pacer_init(300);
    tinygl_init(300);
    ir_uart_init ();
    graphics_init();
    uint8_t count = 0;
    Paddle paddle;
    Ball ball;
	bool game_over = true;
	bool ball_placed = false;
	char ch_received;
	uint16_t speed = 100;

    while (1)
    {
		pacer_wait ();
		if (game_over) {
			speed = wait_for_start(&ball_placed, speed);
			if (!ball_placed) {
				ball_init(&ball);
				ir_uart_putc (BallPlaced);
			} else {
				ball.x = 10;
				ball.y = 10;
				ball.dir = S;
			}
			game_over = false;
			paddle_init(&paddle);
		} else {
			tinygl_clear();
			move_paddle_task (&paddle);
			if (ball.x == 10 && ball.y == 10 && ir_uart_read_ready_p ()) {
				ch_received = ir_uart_getc ();
				game_over = decode_ball_info(ch_received, &ball);
			}
			display_task(paddle, ball);
			if (ball.dir == S && navswitch_push_event_p(NAVSWITCH_PUSH)) {
				ball.dir = F;
			}
			if (count % speed == 0) {
				move_ball(&ball, paddle);
				count = 0;
			}
			count++;
			//check_for_loss(ball, &game_over);
			
			if (ball.x == 4) {
				game_over = true;
				ball_placed = false;
				finish('L');
			}
		}
	}
}

/* Need proper win/lose conditions. At the moment, winner can just keep
 * moving paddle, but can't receive a new ball or start a new game without
 * resetting. Loser returns to PONG screen.
 * 
 * Also, the ball occassionally doubles up still. Don't have the brainpower
 * to work out when.
 */
