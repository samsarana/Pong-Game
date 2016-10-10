/** @file   paddle.c
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions for paddle movement
*/

#define LEFT_WALL 6
#define RIGHT_WALL 0
#define PADDLE_START_LEFT 4
#define PADDLE_START_MID 3
#define PADDLE_START_RIGHT 2


void move_paddle(Paddle * paddle, P_Dir dir)
// Takes a pointer to a paddle struct and moves the entire paddle one
// square up or one square down in the given direction. The paddle will
// not move if you try to push it over the edge of the screen
{
    if (dir == Left && paddle->left != LEFT_WALL) {
        paddle->left++;
        paddle->mid++;
        paddle->right++;
    } else if (dir == Right && paddle->right != RIGHT_WALL) {
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


void paddle_init(Paddle * paddle)
// Takes a pointer to a paddle struct and moves it back to its starting
// position in the middle of the screen
{
    paddle->left = PADDLE_START_LEFT;
    paddle->mid = PADDLE_START_MID;
    paddle->right = PADDLE_START_RIGHT;
}
