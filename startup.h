/** @file   startup.h
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions required for game setup
*/


#ifndef STARTUP_H
#define STARTUP_H

#include "system.h"

#define BallPlaced 'P'
#define LED_ON 1
#define LED_OFF 0
#define LED_BLUE 0
#define SLOW 100
#define NORMAL 50
#define INSANE 30
#define BUTTON 0


uint16_t wait_for_start(bool * ball_placed, uint16_t speed);
// Display welcome message and poll pushbutton to see if player is ready
// to start. Also polls usart to see if opponent has placed ball


#endif /* STARTUP_H  */
