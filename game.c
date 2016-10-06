#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"

/* Define polling rates in Hz.  */
#define MOVE_PADDLE_RATE 2
#define DISPLAY_TASK_RATE 250

typedef struct paddle_s {
	uint8_t left;
	uint8_t mid;
	uint8_t right;
} Paddle;
	
	
void move_paddle(Paddle paddle, uint8_t dir)
{
	if (dir == 1 && paddle.right != 6) {
		paddle.left++;
		paddle.mid++;
		paddle.right++;
	} else if (dir == -1 && paddle.left != 0) {
		paddle.left--;
		paddle.mid--;
		paddle.right--;
	}
}


void move_paddle_task(Paddle paddle)
{
	navswitch_update ();

	if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
		move_paddle(paddle, 1);
	}
	if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
		move_paddle(paddle, -1);
	}
}


void display_task (Paddle paddle)
{
	tinygl_point_t pos1 = {1, paddle.left};
	tinygl_point_t pos2 = {1, paddle.right};
	tinygl_draw_line (pos1, pos2, 1);
	tinygl_update ();
}


int main (void)
{
    system_init ();
    navswitch_init();
    pacer_init(250);
    tinygl_init(250);
    //uint8_t count = 0;
    //Paddle paddle = {0, 1, 2};

	tinygl_point_t pos1 = {3, 6};
	tinygl_point_t pos2 = {3, 1};
	
	tinygl_draw_line (pos1, pos2, 1);
	

    while (1)
    {
		pacer_wait ();
		//move_paddle_task (paddle);
		//display_task(paddle);
		//tinygl_pixel_set(pos1, 1);
		//tinygl_pixel_set(pos2, 1);
		
		tinygl_update ();
		
		return 0;
    }
}
