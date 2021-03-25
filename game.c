/*
 * ENCE260-19S2 UCKF4 Assignment
 *
 * File...................game.c
 *
 * Main program
 *
 * Authors:
 *      Daniel Watson.....32227228 djw187
 *      James Kwok........63430622 jtk39
 *
 * Date...................16/10/2019
 *
 */

#include "system.h"
#include "tinygl.h"
#include "pacer.h"
#include "led.h"
#include "pio.h"
#include "ir_uart.h"
#include "button.h"
#include "navswitch.h"
#include "ledmat.h"
#include "../fonts/font5x7_1.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define PACER_RATE 500
#define MESSAGE_RATE 10
#define ATTACK_SIGNAL 'A'
#define LOST_SIGNAL 'G'
#define ACKNOWLEDGEMENT 'Z'
#define COUNT_LIMIT 15000
#define BASE_FALL_SPEED 1500
#define FAST_FALL_SPEED 500


#include "environment.c"
#include "shape.c"
#include "display.c"

/** Starts the main game */
void game(Shape shapes[], Environment* player)
{
    int random_seed = 0;
    int ticks = 0;
    int count = 0;
    int rand_num = 0;
    int speed = BASE_FALL_SPEED;
    bool playing = true;
    char recv_char;
    while(playing) {
        srand(random_seed);
        rand_num = rand() % 3;
        //Generates and places a block at the top of the board.
        Shape new_shape = shapes[rand_num];
        new_shape.x_offset = 0;
        new_shape.y_offset = 0;
        count = 0;
        while (count < COUNT_LIMIT) {
            button_update();
            if (player->attack_meter == 5) {
                led_set(LED1, 1);
            } else {
                led_set(LED1, 0);
            }

            display_all(new_shape, *player);
            if (player->board[0][1] == 1 || player->board[0][2] == 1 || player->board[1][1] == 1 || player->board[1][2] == 1) {
                player->current_state = LOST;
                tinygl_text(" BETTER LUCK NEXT TIME \0");
                ir_uart_putc(LOST_SIGNAL);
                playing = false;
                count = COUNT_LIMIT;
                break;
            }

            navswitch_update();
            if (button_down_p(0) && player->attack_meter == 5) {
                ir_uart_putc(ATTACK_SIGNAL);
                player->attack_meter = 0;
                led_set(LED1, 0);
                player->current_state = PLAYING;
            }

            if (ir_uart_read_ready_p ()) {
                recv_char = ir_uart_getc();
                if (recv_char == ATTACK_SIGNAL) {
                    shift_row(player);
                    shift_block(&new_shape);
                    add_attack_dots(player, rand() % 5);
                } else if (recv_char == LOST_SIGNAL) {
                    player->current_state = WON;
                    count = COUNT_LIMIT;
                    tinygl_text(" WINNER! \0");
                    playing = false;
                }
            }

            //If enough ticks have passed, move the shape downwards by incrementing it's y offset.
            if (ticks >= speed) {
                if(block_bottom(&new_shape) == 1 ||
                    block_intersect(new_shape, *player) == 1) {
                    add_block_to_board(&new_shape, player);
                    break;
                }
                new_shape.y_offset += 1;
                ticks = 0;
            }

            //Rotate the block if navswitch pushed
            if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
                random_seed++;
                new_shape = rotate_shape(new_shape);
                display_all(new_shape, *player);
            }

            if(navswitch_push_event_p(NAVSWITCH_EAST)) {
                random_seed++;
                move_right(&new_shape, *player);
            }

            if(navswitch_push_event_p(NAVSWITCH_WEST)) {
                random_seed++;
                move_left(&new_shape, *player);
            }

            //Makes the block drop faster while navswitch south is held.
            //The lower the number the faster it falls.
            if(navswitch_down_p(NAVSWITCH_SOUTH)) {
                random_seed++;
                speed = FAST_FALL_SPEED;
            } else {
                speed = BASE_FALL_SPEED;
            }
            delete_row(player);
            ticks++;
            count++;
        }
    }

}

/** Initialisation and startup with other player */
int main (void)
{
    Environment player = player_init();
    Shape shapes[3];
    shape_init(shapes);
    system_init();
    led_init();
    led_set(LED1, 0);
    button_init();
    navswitch_init();
    ir_uart_init();
    start_screen_init();
    pacer_init (PACER_RATE);

    while (1)
    {
        pacer_wait();
        tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
        if (player.current_state == TITLE_SCREEN || player.current_state == READY || player.current_state == ID_SELECT) {
            button_update();
            tinygl_update();
            navswitch_update();
            if (button_down_p(0) && player.current_state == TITLE_SCREEN) {
                player.current_state = ID_SELECT;
                srand(time(NULL));
                player.identifier = (rand() % 2) + 1;
                tinygl_clear();
                tinygl_text_mode_set(TINYGL_TEXT_MODE_STEP);

                if (player.identifier == 1) {
                    tinygl_text("P1\0");
                } else {
                    tinygl_text("P2\0");
                }
            } else if (player.current_state == ID_SELECT) {
                if (player.identifier == 1 &&
                (navswitch_push_event_p(NAVSWITCH_NORTH) || navswitch_push_event_p(NAVSWITCH_SOUTH))) {
                    player.identifier = 2;
                    tinygl_text("P2\0");
                } else if (player.identifier == 2 &&
                (navswitch_push_event_p(NAVSWITCH_NORTH) || navswitch_push_event_p(NAVSWITCH_SOUTH))) {
                    player.identifier = 1;
                    tinygl_text("P1\0");
                }

                if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
                    player.current_state = READY;
                    tinygl_text(" READY \0");
                }
            } else if (player.current_state == READY) {
                if (player.identifier == 1) {
                    ir_uart_putc(ACKNOWLEDGEMENT);
                    player.current_state = PLAYING;
                    game(shapes, &player);
                } else {

                    if (ir_uart_read_ready_p()) {
                       if (ir_uart_getc() == ACKNOWLEDGEMENT) {
                           player.current_state = PLAYING;
                           game(shapes, &player);
                       }
                    }
                }
            }
        } else if (player.current_state == LOST) {
            led_set(LED1, 0);
            tinygl_update();
        } else if (player.current_state == WON) {
            led_set(LED1, 0);
            tinygl_update();
        }
    }
}
