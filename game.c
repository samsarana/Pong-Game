#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"
#include "delay.h"

/* Define polling rates in Hz.  */
#define MOVE_PADDLE_RATE 2
#define DISPLAY_TASK_RATE 250
#define MESSAGE_RATE 10

typedef enum {F, B, FL, FR, BL, BR} Dir;

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

void move_ball(Ball* ball, Paddle paddle) {
	if (ball->x != 0 && ball->y != 0 && ball->y != 6 && ball->x != 3) {
		continue_straight(ball);
	} else if (ball->x == 0){
		if (ball->dir == F) {
			ball->x++;
			ball->dir = B;
		} else if (ball->dir == FL) {
			ball->x++;
			ball->y++;
			ball->dir = BL;
		} else if (ball->dir == FR) {
			ball->x++;
			ball->y--;
			ball->dir = BR;
		}
	} else if (ball->x == 3) {
		if (ball->dir == B) {
			if (ball->y == paddle.left) {
				ball->x--;
				ball->y++;
				ball->dir = FL;
			} else if (ball->y == paddle.mid) {
				ball->x--;
				ball->dir = F;
			} else if (ball->y == paddle.right) {
				ball->x--;
				ball->y--;
				ball->dir = FR;
			} else {
				continue_straight(ball);
			}
		} else if (ball->dir == BL) {
			if (ball->y == paddle.mid) {
				ball->x--;
				ball->y++;
				ball->dir = FL;
			} else if (ball->y == paddle.right) {
				ball->x--;
				ball->dir = F;
			} else if (ball->y == paddle.right - 1) {
				ball->x--;
				ball->y--;
				ball->dir = FR;
			} else {
				continue_straight(ball);
			}
		} else if (ball->dir == BR) {
			if (ball->y == paddle.left + 1) {
				ball->x--;
				ball->y++;
				ball->dir = FL;
			} else if (ball->y == paddle.left) {
				ball->x--;
				ball->dir = F;
			} else if (ball->y == paddle.mid) {
				ball->x--;
				ball->y--;
				ball->dir = FR;
			} else {
				continue_straight(ball);
			}
/*
		} else {
			continue_straight(ball);
*/
		}
	} else if (ball->y == 0){
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

void check_for_loss(Ball ball, bool* game_over)
{
	if (ball.x == 4) {
		*game_over = true;
		// increment other player's points
	}
}



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

void game_init(Ball* ball, Paddle* paddle)
{
	paddle->left = 4;
	paddle->mid = 3;
	paddle->right = 2;
	ball->x = 0;
	ball->y = 5;
	ball->dir = F;
}

void wait_for_start(void)
// Display welcome message and poll pushbutton to see if player is ready
// to start. Also sends ready character to other player
{
    tinygl_text("PONG");
    // send ready character to other player
    while (!navswitch_push_event_p(NAVSWITCH_PUSH)) {
        pacer_wait();
        tinygl_update();
        navswitch_update();
    }
}


void wait_for_player(void)
// Display waiting message and poll USART to see if other player has
// sent ready character
{

	
}


int main (void)
{
    system_init ();
    navswitch_init();
    pacer_init(300);
    tinygl_init(300);
    graphics_init();
    uint8_t count = 0;
    Paddle paddle = {4,3,2};
    Ball ball = {3,5,FR};
	bool game_over = true;

    while (1)
    {
		pacer_wait ();
		if (game_over) {
			tinygl_clear();
			wait_for_start();
			wait_for_player();
			game_over = false;
			game_init(&ball, &paddle);
		} else {
			tinygl_clear();
			move_paddle_task (&paddle);
			display_task(paddle, ball);
			if (count % 100 == 0 ) {
				move_ball(&ball, paddle);
				count = 0;
			}
			count++;
			check_for_loss(ball, &game_over);
		}
    }
}
