/** @file   startup.c
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions required for game setup
*/

#include "startup.h"
#include "led.h"
#include "tinygl.h"
#include "button.h"
#include "navswitch.h"
#include "pacer.h"
#include "ir_uart.h"


static void dim_led(uint16_t speed)
// If speed is medium, toggles the led on successive iterations through
// the while loop such that led appears dimmed
{
    static uint8_t toggle = LED_OFF;
    if (toggle == LED_OFF) {
        led_set(LED_BLUE, toggle);
        toggle = LED_ON;
    } else {
        led_set(LED_BLUE, toggle);
        toggle = LED_OFF;
    }
}


static uint16_t update_speed(uint16_t speed)
// Takes the current speed selection and toggles it to the next speed
// selection. Returns the new speed
{
    if (speed == SLOW) {
        speed = NORMAL;
        led_set(LED_BLUE, LED_ON);
    } else if (speed == NORMAL) {
        speed = INSANE;
        led_set(LED_BLUE, LED_ON);
    } else if (speed == INSANE) {
        speed = SLOW;
        led_set(LED_BLUE, LED_OFF);
    }
    return speed;
}


uint16_t wait_for_start(bool * ball_placed, uint16_t speed)
// Display welcome message and poll pushbutton to see if player is ready
// to start. Also polls usart to see if opponent has placed ball
{
    // Clear screen and display PONG start screen
    tinygl_clear();
    tinygl_text("PONG");
    char ch = '\0';

    while (!navswitch_push_event_p(NAVSWITCH_PUSH)) {
        pacer_wait();
        tinygl_update();
        navswitch_update();
        button_update();
        // Check if other player has placed ball
        if (ir_uart_read_ready_p()) {
            ch = ir_uart_getc();
            if (ch == BallPlaced) {
                *ball_placed = true;
            }
        }
        // Cycle through ball speed on button click
        if (button_push_event_p(BUTTON)) {
            speed = update_speed(speed);
        }
        // Dim blue speed indicator LED on normal speed mode
        if (speed == NORMAL) {
            dim_led(speed);
        }
    }
    return speed;
}
