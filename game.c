/** @file   game.c
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  This implements a wicked pong game
*/

#include "system.h"
#include "pacer.h"
#include "navswitch.h"
#include "tinygl.h"
#include "ir_uart.h"
#include "button.h"
#include "led.h"
#include "game.h"
#include "paddle.h"
#include "ball.h"
#include "graphics.h"
#include "startup.h"
#include "communications.h"

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


int main(void)
{
    system_init();
    navswitch_init();
    led_init();
    led_set(LED_BLUE, LED_OFF);
    button_init();
    pacer_init(LOOP_RATE);
    tinygl_init(DISPLAY_TASK_RATE);
    ir_uart_init();
    graphics_init();
    uint8_t count = 0;
    Paddle paddle;
    Ball ball;
    bool game_over = true;
    bool ball_placed = false;
    uint16_t speed = SLOW;

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
