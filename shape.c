/*
 * ENCE260-19S2 UCKF4 Assignment
 *
 * File...................shape.c
 *
 * Contains structs, functions related to shapes/blocks
 *
 * Authors:
 *      Daniel Watson.....32227228 djw187
 *      James Kwok........63430622 jtk39
 *
 * Date...................16/10/2019
 *
 */


/** Defines the type of a shape, used to identify the type of shape.*/
enum type {
    SHAPE_CHAIR,
    SHAPE_STRAIGHT,
    SHAPE_BLOCK
};

/** Shape Struct:
 * Defines the shape of the Shape in a 2x2 grid
 * Contains the shapes current offset from the starting position */
typedef struct {
    //initial position of a block is equal to 0
    //a maximum of -2 - +2 can be applied to a block given it's orientation.
    char size[2][2];
    int type;
    int orientation; //0-3 in 90 degree changes
    int x_offset;
    int y_offset;
} Shape;

/** Initialises variables for the different kinds of shapes */
Shape* shape_init(Shape* shapes)
{
    Shape chair;
    Shape straight;
    Shape block;

    chair.size[0][0] = 0;
    chair.size[0][1] = 1;
    chair.size[1][0] = 1;
    chair.size[1][1] = 1;
    chair.type = SHAPE_CHAIR;
    chair.orientation = 0;
    chair.x_offset = 0;
    chair.y_offset = 0;

    straight.size[0][0] = 1;
    straight.size[0][1] = 0;
    straight.size[1][0] = 1;
    straight.size[1][1] = 0;
    straight.type = SHAPE_STRAIGHT;
    straight.orientation = 0;
    straight.x_offset = 0;
    straight.x_offset = 0;

    block.size[0][0] = 1;
    block.size[0][1] = 1;
    block.size[1][0] = 1;
    block.size[1][1] = 1;
    block.type = SHAPE_BLOCK;
    block.orientation = 0;
    block.x_offset = 0;
    block.y_offset = 0;

    shapes[0] = chair;
    shapes[1] = straight;
    shapes[2] = block;

    return shapes;
}



/** Checks if the given shape is able to rotate. */
int shape_can_rotate(Shape shape)
{
    if(shape.type == SHAPE_STRAIGHT && (shape.orientation == 0 || shape.orientation == 2)) {
        if(shape.x_offset <= 2 && shape.x_offset >= -1) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 1;
    }

}

/** Rotates the shape 90 degrees by switching the elements of the 2x2 grid,
 * only if the shape can still fit on the grid*/
Shape rotate_shape(Shape shape)
{
    if(shape_can_rotate(shape) == 1) {
        char holding = shape.size[0][0];
        shape.size[0][0] = shape.size[1][0];
        shape.size[1][0] = shape.size[1][1];
        shape.size[1][1] = shape.size[0][1];
        shape.size[0][1] = holding;
        shape.orientation = (shape.orientation + 1) % 4;
    }
    return shape;
}

/* Returns 1 if the shape can move right without intersecting another block, returns 0 otherwise. */
int shape_can_move_right(Shape shape, Environment player)
{
    if(shape.type == SHAPE_CHAIR) {
        if(shape.orientation == 0 || shape.orientation == 3) {
            if(player.board[shape.y_offset][shape.x_offset+3] == 1 ||
               player.board[shape.y_offset+1][shape.x_offset+3] == 1) {
                return 0;
            }
        } else if(shape.orientation == 1) {
            if(player.board[shape.y_offset][shape.x_offset + 2] == 1 ||
               player.board[shape.y_offset + 1][shape.x_offset + 3] == 1) {
                return 0;
            }
        } else if(shape.orientation == 2) {
            if(player.board[shape.y_offset][shape.x_offset+3] == 1 ||
               player.board[shape.y_offset+1][shape.x_offset+2] == 1) {
                return 0;
            }
        }
    } else if(shape.type == SHAPE_STRAIGHT) {
        if(shape.orientation == 0) {
            if(player.board[shape.y_offset][shape.x_offset+2] == 1 ||
               player.board[shape.y_offset+1][shape.x_offset+2] == 1) {
                return 0;
            }
        } else if(shape.orientation == 1) {
            if(player.board[shape.y_offset][shape.x_offset+3] == 1) {
                return 0;
            }
        } else if(shape.orientation == 2) {
            if(player.board[shape.y_offset][shape.x_offset+3] == 1 ||
               player.board[shape.y_offset+1][shape.x_offset+3] == 1) {
                return 0;
            }
        } else {
            if(player.board[shape.y_offset+1][shape.x_offset+3] == 1) {
                return 0;
            }
        }
    } else {
        if(player.board[shape.y_offset][shape.x_offset+3] == 1 ||
           player.board[shape.y_offset+1][shape.x_offset+3] == 1) {
            return 0;
        }
    }
    return 1;
}

/** Moves the block right by increasing its x_offset value by one if valid */
void move_right(Shape* new_shape, Environment player)
{
    if(shape_can_move_right(*new_shape, player) == 1) {
        int move=0;
        if (new_shape->type == 0 || new_shape->type == 2) {
            if(new_shape->x_offset <= 1) {
                move=1;
            }
        } else if (new_shape->type == 1) {
            if((new_shape->orientation == 0 ) &&
                new_shape->x_offset <= 2) {
                move=1;
            }
            else if(new_shape->orientation == 2 && new_shape->x_offset <= 3) {
                move=1;
            }
            else if((new_shape->orientation == 1 || new_shape->orientation == 3) &&
                     new_shape->x_offset <= 1) {
                move=1;
            }
        }

        if(move==1) {
            new_shape->x_offset += 1;
        }
    }
}

/** Returns 1 if the block can move left without intersecting another block */
int block_can_move_left(Shape shape, Environment player)
{
    if(shape.type == SHAPE_CHAIR) {
        if(shape.orientation == 1 || shape.orientation == 2) {
            if(player.board[shape.y_offset][shape.x_offset] == 1 ||
               player.board[shape.y_offset+1][shape.x_offset] == 1) {
                return 0;
            }
        } else if(shape.orientation == 0) {
            if(player.board[shape.y_offset][shape.x_offset+1] == 1 ||
               player.board[shape.y_offset+1][shape.x_offset] == 1) {
                return 0;
            }
        } else if(shape.orientation == 3) {
            if(player.board[shape.y_offset][shape.x_offset] == 1 ||
               player.board[shape.y_offset+1][shape.x_offset+1] == 1) {
                return 0;
            }
        }
    } else if(shape.type== SHAPE_STRAIGHT) {
        if(shape.orientation == 0) {
            if(player.board[shape.y_offset][shape.x_offset] == 1 ||
               player.board[shape.y_offset+1][shape.x_offset] == 1) {
                return 0;
            }
        } else if(shape.orientation == 1) {
            if(player.board[shape.y_offset][shape.x_offset] == 1) {
                return 0;
            }
        } else if(shape.orientation == 2) {
            if(player.board[shape.y_offset][shape.x_offset+1] == 1 ||
               player.board[shape.y_offset+1][shape.x_offset+1] == 1) {
                return 0;
            }
        } else {
            if(player.board[shape.y_offset+1][shape.x_offset] == 1) {
                return 0;
            }
        }
    } else {
        if(player.board[shape.y_offset][shape.x_offset] == 1 ||
           player.board[shape.y_offset+1][shape.x_offset] == 1) {
            return 0;
        }
    }
    return 1;
}

/** Moves the block left by decreasing its x_offset by one if valid */
void move_left(Shape* new_shape, Environment player)
{
    if(block_can_move_left(*new_shape, player) == 1) {

        if (new_shape->x_offset >= 0) {
            new_shape->x_offset -= 1;
        } else if(new_shape->orientation == 2 && new_shape->x_offset >= -1
                  && new_shape->type == 1) {
            new_shape->x_offset -= 1;
        }
    }
}

/** Adds a block to the board by setting the corresponding points to 1 */
void add_block_to_board(Shape* shape, Environment* player)
{
    if(shape->size[0][0] == 1) {
        player->board[shape->y_offset][1 + shape->x_offset] = 1;
    }
    if(shape->size[1][0] == 1) {
        player->board[1 + shape->y_offset][1 + shape->x_offset] = 1;
    }
    if(shape->size[0][1] == 1) {
        player->board[shape->y_offset][2 + shape->x_offset] = 1;
    }
    if(shape->size[1][1] == 1) {
        player->board[1 + shape->y_offset][2 + shape->x_offset] = 1;
    }
}
int block_intersect(Shape shape, Environment player)
{
    if(shape.type == SHAPE_CHAIR) {
        if(shape.orientation == 0 || shape.orientation == 1) {
            if((player.board[shape.y_offset+2][shape.x_offset+2] == 1) ||
               (player.board[shape.y_offset+2][shape.x_offset+1] == 1))  {
                return 1;
            }
        } else if(shape.orientation == 2) {
            if((player.board[shape.y_offset+2][shape.x_offset+1] == 1) ||
               (player.board[shape.y_offset+1][shape.x_offset+2] == 1))  {
                return 1;
            }
        } else {
            if((player.board[shape.y_offset+2][shape.x_offset+2] == 1) ||
               (player.board[shape.y_offset+1][shape.x_offset+1] == 1))  {
                return 1;
            }
        }

    } else if(shape.type == SHAPE_STRAIGHT) {
        if(shape.orientation == 0) {
            if(player.board[shape.y_offset + 2][shape.x_offset + 1] == 1) {
                return 1;
            }
        } else if(shape.orientation == 1) {
            if((player.board[shape.y_offset+1][shape.x_offset+1] == 1) ||
               (player.board[shape.y_offset+1][shape.x_offset+2] == 1)) {
                return 1;
            }
        } else if(shape.orientation == 2) {
            if(player.board[shape.y_offset + 2][shape.x_offset+2] == 1) {
                return 1;
            }
        } else {
            if((player.board[shape.y_offset+2][shape.x_offset+1] == 1 ||
                player.board[shape.y_offset+2][shape.x_offset+2])) {
                return 1;
            }
        }
    } else {
        if(player.board[shape.y_offset + 2][shape.x_offset + 2] == 1 ||
           player.board[shape.y_offset + 2][shape.x_offset + 1] == 1) {
            return 1;
        }
    }
    return 0;
}

/* Checks if the block is touching the bottom
 * a particular orientation of the straight shape has
 * a slightly different implementation */
int block_bottom(Shape* shape)
{
    if(shape->type == SHAPE_CHAIR) {
        if(shape->orientation == 1) {
            if(shape->y_offset == 6) {
                return 1;
            }
            return 0;
        }
    } else if(shape->type == SHAPE_STRAIGHT) {
        if(shape->orientation == 1) {
            if(shape->y_offset == 7) {
                shape->y_offset = 6;
                return 1;
            }
            return 0;
        }
    }

    if(shape->y_offset == 5) {
        return 1;
    } else {
        return 0;
    }
}
