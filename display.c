/*
 * ENCE260-19S2 UCKF4 Assignment
 *
 * File...................display.c
 *
 * Contains all functions related to the display
 *
 * Authors:
 *      Daniel Watson.....32227228 djw187
 *      James Kwok........63430622 jtk39
 *
 * Date...................16/10/2019
 *
 */

/** Initialises tinygl and sets the title. */
void start_screen_init(void)
{
    int LOOP_RATE = PACER_RATE;
    tinygl_init(LOOP_RATE);
    tinygl_font_set(&font5x7_1);
    tinygl_text_speed_set(MESSAGE_RATE);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text(" TROMTRIS \0");
}

/** Displays a given shape on the led matrix at the top middle position */
void display_shape(Shape shape)
{
    if(shape.size[0][0] == 1) {
        struct tinygl_point temp_point = {1 + shape.x_offset, 0 + shape.y_offset};
        tinygl_draw_point(temp_point, 1);
    }
    if(shape.size[1][0] == 1) {
        struct tinygl_point temp_point = {1 + shape.x_offset, 1 + shape.y_offset};
        tinygl_draw_point(temp_point, 1);
    }
    if(shape.size[0][1] == 1) {
        struct tinygl_point temp_point = {2 + shape.x_offset, 0 + shape.y_offset};
        tinygl_draw_point(temp_point, 1);
    }
    if(shape.size[1][1] == 1) {
        struct tinygl_point temp_point = {2 + shape.x_offset, 1 + shape.y_offset};
        tinygl_draw_point(temp_point, 1);
    }
}

/** Displays a point on the game board */
void display_board(Environment player)
{
        for (int i = 0; i <= 6; i++) {
            for (int j = 0; j <= 4; j++) {
                if (player.board[i][j] == 1) {
                    struct tinygl_point temp_point = {j, i};
                    tinygl_draw_point(temp_point, 1);
                }
            }
        }
        tinygl_update();
}

/** Displays a shape on the game board, and then populates the game board */
void display_all(Shape shape, Environment player)
{
    tinygl_clear();
    display_shape(shape);
    display_board(player);
    tinygl_update();
}

/* Removes a row from the game board only if it is full */
void delete_row(Environment* player)
{
    int row = 6;
    int deleted_rows = 0;
    while ((row >= 1)) {

        bool is_candidate = true;
        int col = 0;
        while ((col <= 4) && is_candidate) {
            // Do not remove if there is an empty point on that row
            if (player->board[row][col] == 0) {
                is_candidate = false;
            }
            col += 1;
        }
        if (is_candidate == true) {
            // Overwrite lower rows with above row
            int current_row = row;
            while (current_row - deleted_rows >= 1) {
                for (int i = 0; i <= 4; i++) {
                    player->board[current_row][i] = player->board[current_row - 1][i];
                }
                current_row -= 1;
            }
            deleted_rows += 1;
            row += 1;
        }
        row -= 1;
    }

    // Increase the attack meter depending on how many rows were removed
    switch(deleted_rows) {
        case 2:
            increase_meter(player, 3);
            break;
        case 1:
            increase_meter(player, 1);
            break;
    }
}

/* Shifts the entire board up by one and set the bottom row to empty.
 * Called during an attack. */
void shift_row(Environment* player)
{
   for (int row = 0; row <= 5; row++) {
        for (int col = 0; col <= 4; col++) {
            player->board[row][col] = player->board[row + 1][col];
        }
   }
   for (int col = 0; col <= 4; col++) {
        player->board[6][col] = 0;
   }
}

/* Shifts the block up by up to two if it is not already at the top.
 * Gets called when a player attacks another. */
void shift_block(Shape* shape) {
    switch (shape->y_offset) {
        case 0:
            break;
        case 1:
            shape->y_offset -= 1;
            break;
        default:
            shape->y_offset -= 2;
    }
}

/* Adds a set of dots to a row, except for the free_pos.
 * Row will have all dots filled but the free_pos.
 * Function called when a player is attacked */
void add_attack_dots(Environment* player, int free_pos)
{
    for (int i = 0; i <= 4; i++) {
        if (i != free_pos) {
            player->board[6][i] = 1;
        }
    }
}
