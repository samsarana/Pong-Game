/** @file   ball.c
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions for ball movement and interactions
*/

#include "ball.h"
#include "paddle.h"
#include "navswitch.h"
#include "graphics.h"
#include "communications.h"


void move_ball_off_screen(Ball * ball)
// Takes a pointer to a ball, moves it off the screen and makes it
// stationary
{
    ball->x = OUT_OF_PLAY;
    ball->y = OUT_OF_PLAY;
    ball->dir = S;
}


static void continue_straight(Ball * ball)
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


static void bounce_off_wall(Ball * ball)
// Takes a pointer to a ball struct which will represent a ball that is
// at a wall (i.e. y is 0 or 6) and updates the ball's position and
// direction according to our logic for wall bouncing
{
    if (ball->y == RIGHT_WALL) {
        if (ball->dir == BR) {
            ball->x++;
            ball->y++;
            ball->dir = BL;
        } else if (ball->dir == FR) {
            ball->x--;
            ball->y++;
            ball->dir = FL;
        }
    } else if (ball->y == LEFT_WALL) {
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


static void move_ball_top(Ball * ball)
// Takes a pointer to a ball struct which represents a ball at the top
// of the screen. The ball is either sent to the other player's screen
// if it is travelling forward, or it moves a square back, according to
// its current direction
{
    if (ball->dir == FL) {
        if (ball->y == LEFT_WALL) {
            ball->y--;
            ball->dir = FR;
        } else {
            ball->y++;
        }
        transmit_ball_info(ball);
    } else if (ball->dir == FR) {
        if (ball->y == RIGHT_WALL) {
            ball->y++;
            ball->dir = FL;
        } else {
            ball->y--;
        }
        transmit_ball_info(ball);
    } else if (ball->dir == F) {
        transmit_ball_info(ball);
    } else if (ball->y == RIGHT_WALL || ball->y == LEFT_WALL) {
        bounce_off_wall(ball);
    } else {
        continue_straight(ball);
    }
}


static void move_ball_bottom(Ball * ball, Paddle paddle)
// Takes a pointer to a ball struct which represents a ball at the bottom
// of the screen, and a paddle. If the ball should bounce off the paddle,
// its position and direction will be updated accordingly. Otherwise,
// ball will continue straight (and you will lose the game!)
{
    if (ball->y == paddle.left
        || (ball->y == paddle.right && ball->y == RIGHT_WALL)) {
        ball->x--;
        ball->y++;
        ball->dir = FL;
    } else if (ball->y == paddle.mid) {
        ball->x--;
        ball->dir = F;
    } else if (ball->y == paddle.right
               || (ball->y == paddle.left && ball->y == LEFT_WALL)) {
        ball->x--;
        ball->y--;
        ball->dir = FR;
    } else if (ball->y == RIGHT_WALL || ball->y == LEFT_WALL) {
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
    } else if (ball->x > TRANSMISSION_COL && ball->x < BOUNCE_COL && ball->y > RIGHT_WALL && ball->y < LEFT_WALL) {
        continue_straight(ball);
    } else if (ball->x == TRANSMISSION_COL) {
        move_ball_top(ball);
    } else if (ball->x == BOUNCE_COL) {
        move_ball_bottom(ball, paddle);
    } else {
        bounce_off_wall(ball);
    }
}


void ball_init(Ball * ball)
// Takes a pointer to a ball struct, moves it back to its starting
// position in the middle of the screen and makes it stationary
{
    ball->x = BALL_START;
    ball->y = BALL_START;
    ball->dir = S;
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
    if (ball->x == LOSE_COL) {
        *game_over = true;
        *ball_placed = false;
        display_winner('L');
    }
}
