/*
 * ENCE260-19S2 UCKF4 Assignment
 *
 * File...................environment.c
 *
 * Contains structs, enums and functions related to the player environment
 *
 * Authors:
 *      Daniel Watson.....32227228 djw187
 *      James Kwok........63430622 jtk39
 *
 * Date...................16/10/2019
 *
 */

/* Initialises various game states. */
enum state {
    TITLE_SCREEN,
    ID_SELECT,
    READY,
    PLAYING,
    WON,
    LOST
};

/* Environment Struct:
 * Stores current state of the game,
 * The value of the attack meter,
 * The state of the game board,
 * and the player ID.*/
typedef struct {
    enum state current_state;
    int attack_meter;
    char board[7][5]; //1 if block in position, 0 otherwise
    char identifier;
} Environment;

/* Increases the players attack meter,
 * up to a maximum of 5. */
void increase_meter(Environment* player, int increase)
{
    player->attack_meter += increase;
    if (player->attack_meter > 5) {
        player->attack_meter = 5;
    }
}

/* Sets initial variables for the player environment:
 * The program starts in the TITLE_SCREEN state,
 * The attack_meter is set to 0
 * and the board is empty */
Environment player_init(void)
{
    Environment player;
    player.current_state = TITLE_SCREEN;
    player.attack_meter = 0;
    for(int i=0; i < 7; i++) {
        for(int j=0; j< 5; j++) {
            player.board[i][j] = 0;
        }
    }
    return player;
}