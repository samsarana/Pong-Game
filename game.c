#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"

/* Define polling rates in Hz.  */
#define MOVE_PADDLE_RATE 2
#define DISPLAY_TASK_RATE 250

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

void move_ball(Ball* ball, Paddle paddle){
	if (ball->x != 0 && ball->y != 0 && ball->y != 6 && ball->x != 3) {
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
	} else if (ball->x == 3){
/*
		if (ball->dir == B) {
			ball->x--;
			ball->dir = F;
		} else if (ball->dir == BL) {
			ball->x--;
			ball->y++;
			ball->dir = FL;
		} else if (ball->dir == BR) {
			ball->x--;
			ball->y--;
			ball->dir = FR;
*/

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


int main (void)
{
    system_init ();
    navswitch_init();
    pacer_init(252);
    tinygl_init(252);
    uint8_t count = 0;
    Paddle paddle = {4,3,2};
    Ball ball = {3,4,FR};
    
	

    while (1)
    {
		pacer_wait ();
		move_paddle_task (&paddle);
		display_task(paddle, ball);
		if (count % 84 == 0 ) {
			move_ball(&ball, paddle);
			count = 0;
		}
		count++;
    }
}
