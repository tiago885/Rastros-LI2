#include <stdlib.h>
#include "../data/state.h"
#include "../linked_lists/linked.h"

/*
 * Checks if play is valid
 */

int check_move(const State* const state, const Position pos){
    const Position last_play = get_last_play(state);

    return pos.row >= 0 && pos.row < 8 && pos.column >= 0 && pos.column < 8 &&
        abs(last_play.row - pos.row) <= 1 && abs(last_play.column - pos.column) <= 1 &&
        get_position_space(state, pos) == Blank;

}


/*
 * Changes spaces from each move's position and appends move to moves' array
 */
void make_move(State* const state, const Position pos){
    unsigned int current_player = get_current_player(state);
    Position last_play = get_last_play(state);

    // Changes last move's position in the boarder to Black
    edit_position_space(state, last_play, Black);

    // Changes new move's position in the boarder to White
    edit_position_space(state, pos, White);

    if (current_player == 2) {
        Move move = {.player1 = last_play, .player2 = pos};
        append_move(state, move);
    }

    edit_last_play(state, pos);
}


/*
 * Changes state if played in a valid position
 */

int play(State* const state, const Position pos) {

    if (!check_move(state, pos))
        return 0;

    make_move(state, pos);

    return 1;
}

/*
 * Checks if game finished and returns the winner
 * Returns 0 in case game not finished yet
 */

unsigned int game_finished(const State* const state) {

    const int sum_pos[8][2] = {{1, 1}, {1, 0}, {1, -1}, {0, 1}, {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}};
    const Position last_play=get_last_play(state);
    Position pos;

    if (last_play.row - last_play.column == 7)
        return 1;
    else if (last_play.column - last_play.row == 7)
        return 2;

    for (int i=0; i < 8; ++i) {
        pos = (Position) {.row = last_play.row + sum_pos[i][0], .column = last_play.column + sum_pos[i][1]};

        if (pos.row >= 0 && pos.row < 8 && pos.column >= 0 && pos.column < 8 &&
            get_position_space(state, pos) == Blank)
            return 0; // Game not finished yet
    }

    return get_current_player(state);
}


/*
 * Gives a random position
 */

Position randomJog (State* state){
    const int sum_pos[8][2] = {{1, 1}, {1, 0}, {1, -1}, {0, 1}, {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}};
    const Position last_play=get_last_play(state);
    Position positions[8],pos;
    int length = 0;

    List* position_list = create_list();

    for (int i=0; i < 8; ++i) {
        pos = (Position) {.row = last_play.row + sum_pos[i][0], .column = last_play.column + sum_pos[i][1]};


        if (pos.row >= 0 && pos.row < 8 && pos.column >= 0 && pos.column < 8 &&
            get_position_space(state, pos) == Blank) {

            positions[length] = pos;
            position_list = head_insert(position_list, positions + length);
            length++;
        }

    }
    int random = rand() % length;
    Position* jogpos = get_head(position_list);

    for (int j = 0 ; j< random ; j++) {
        jogpos = get_head(position_list);
        position_list = remove_head(position_list);
    }

    position_list = clear_list(position_list);

    return *jogpos;

}


/*
 * Does a random computer move
 */

void computer_move2 (State* const state){
    Position pos = randomJog(state);
    make_move(state,pos);
}

/*
 * Choses the adjacent position that is near from the objetive
 */
Position arithmeticJog (State* state, unsigned int player){
    int d, d1;
    Position* jogpos;
    Position* jogpos1;
    const int sum_pos[8][2] = {{1, 1}, {1, 0}, {1, -1}, {0, 1}, {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}};
    const Position last_play=get_last_play(state);
    Position positions[8],pos;
    int length = 0;

    List* position_list = create_list();

    for (int i=0; i < 8; ++i) {
        pos = (Position) {.row = last_play.row + sum_pos[i][0], .column = last_play.column + sum_pos[i][1]};


        if (pos.row >= 0 && pos.row < 8 && pos.column >= 0 && pos.column < 8 &&
            get_position_space(state, pos) == Blank) {

            positions[length] = pos;
            position_list = head_insert(position_list, positions + length);
            length++;
        }

    }

    jogpos = get_head(position_list);

    d = ((7 * ((int) player - 1)) - jogpos->column) * ((7 * ((int) player - 1)) - jogpos->column)
        + (7 - (7 * ((int) player - 1)) - jogpos->row) * (7 - (7 * ((int) player - 1)) - jogpos->row);

    position_list = remove_head(position_list);

    while (position_list != NULL) {
        jogpos1 = get_head(position_list);
        d1 = ((7 * ((int) player - 1)) - jogpos1->column) * ((7 * ((int) player - 1)) - jogpos1->column)
            + (7 - (7 * ((int) player - 1)) - jogpos1->row) * (7 - (7 * ((int) player - 1)) - jogpos1->row);

        if (d1 < d) {
            d = d1;
            jogpos = jogpos1;
        }
        position_list = remove_head(position_list);
    }

    return *jogpos;

}


/*
 * Reads the flood fill matrix, giving a position where to play in order to reach the objetive faster.
 */

Position reads_flood_fill_matrix(int flood_fill_matrix[][8], Position pos, int count) {
    const int sum_pos[8][2] = {{1, 1}, {1, 0}, {1, -1}, {0, 1}, {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}};
    Position play;

    while(count != 1) {
        for(int i = 0; i < 8; i++) {
            play = (Position) {.row = pos.row + sum_pos[i][0], .column = pos.column + sum_pos[i][1]};

            if (play.row >= 0 && play.row < 8 && play.column >= 0 && play.column < 8 &&
                flood_fill_matrix[play.row][play.column] == count - 1) {

                    pos = (Position) {.row = play.row, .column = play.column};
                    --count;
                    break;
            }
        }
    }
    return pos;
}


/*
 * Creates the flood fill matrix on a given state of the game, returning to best position to reach the objetive,.
 */

Position flood_fill(State* state, int flood_fill_matrix[][8] ,unsigned int player) {
    int count = 1;
    const int sum_pos[8][2] = {{1, 1}, {1, 0}, {1, -1}, {0, 1}, {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}};
    Position last_play = get_last_play(state);
    Position positions[64], pos;
    Position* play;
    int length = 0;
    List* position_list1 = create_list();
    List* position_list2 = create_list();

    position_list1 = head_insert (position_list1, &last_play);

    while (position_list1 != NULL) {
        while (position_list1 != NULL) {
            play = get_head(position_list1);

            for (int i = 0; i < 8; ++i) {
                pos = (Position) {.row = play->row + sum_pos[i][0], .column = play->column + sum_pos[i][1]};

                if ((unsigned) pos.column == (7 * (player - 1)) && (unsigned) pos.row == 7 - (7 * (player - 1))) {
                    flood_fill_matrix[pos.row][pos.column] = count;

                    position_list1 = clear_list(position_list1);
                    position_list2 = clear_list(position_list2);

                    return reads_flood_fill_matrix(flood_fill_matrix, pos, count);
                }

                else if (pos.row >= 0 && pos.row < 8 && pos.column >= 0 && pos.column < 8 &&
                    flood_fill_matrix[pos.row][pos.column] == 0) {

                    positions[length] = pos;
                    position_list2 = head_insert(position_list2, positions + length);
                    flood_fill_matrix[pos.row][pos.column] = count;
                    length++;
                }
            }

            position_list1 = remove_head(position_list1);
        }

        position_list1 = position_list2;
        position_list2 = create_list();
        ++count;
    }

    return arithmeticJog(state, player); //in case of being impossible to reach the objetive, does an arithmetic calculation
}


/*
 * Does a computer move using the flood fill method
 */

void computer_move(State* const state){
    int flood_fill_matrix[8][8];
    int player = get_current_player(state);

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++){
            if (state->board[i][j] == Blank)
                flood_fill_matrix[i][j] = 0;

            else flood_fill_matrix[i][j] = -1;
        }
    }

    flood_fill_matrix[7 * (player - 1)][7 - (7 * (player - 1))] = -1;

    Position pos = flood_fill(state, flood_fill_matrix, player);
    make_move(state,pos);
}


