/** @file   paddle.h
    @author Angus Schuler and Sam Clarke
    @date   10 October 2016
    @brief  Functions for paddle movement
*/

#ifndef PADDLE_H
#define PADDLE_H


typedef enum { Left, Right } P_Dir;


typedef struct paddle_s {
    uint8_t left;
    uint8_t mid;
    uint8_t right;
} Paddle;


#endif /* PADDLE_H  */
