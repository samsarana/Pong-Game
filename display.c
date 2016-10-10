/** @file   display.c
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions for LED matrix manipulation
*/

#define MESSAGE_RATE 10
#define WAIT_TIME 1000
#define MIDDLE_SQUARE 0, 4
#define WIN 'W'
#define LOSE 'L'
#define PADDLE_COL 4
#define LED_ON 1


void wait(void)
// Waits for a brief time while win/lose message is displayed
{
    uint16_t i = 0;
    while (i <= WAIT_TIME) {
        pacer_wait();
        tinygl_update();
        i++;
    }
}


void display_char(char ch)
// Takes a W or L, displays that character on the screen and sends 'L'
// over IR if player has lost
{
    tinygl_point_t display_square = { MIDDLE_SQUARE };
    if (ch == WIN) {
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


void display_task(Paddle paddle, Ball ball)
// Takes a paddle and a ball, and displays them on the screen
{
    tinygl_point_t paddle_left = { PADDLE_COL, paddle.left };
    tinygl_point_t paddle_right = { PADDLE_COL, paddle.right };
    tinygl_point_t ball_pos = { ball.x, ball.y };

    tinygl_clear();
    tinygl_draw_line(paddle_left, paddle_right, LED_ON);
    tinygl_draw_point(ball_pos, LED_ON);
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
