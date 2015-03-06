#ifndef CHESS_
#define CHESS_

#include <stdio.h>

#define PAWN_W 'm'
#define PAWN_B 'M'
#define KING_W 'k'
#define KING_B 'K'
#define KNIGHT_W 'n'
#define KNIGHT_B 'N'
#define BISHOP_W 'b'
#define BISHOP_B 'B'
#define ROOK_W 'r'
#define ROOK_B 'R'
#define QUEEN_W 'q'
#define QUEEN_B 'Q'
#define EMPTY ' '
#define DEFAULT_PROMOTION 'q'
#define EOL '\0'

#define BOARD_SIZE 8
#define MAX_DEPTH 4
#define MIN_DEPTH 1
#define TWO_PLAYER 1
#define PLAYER_VS_COMP 2
#define MAX_PLAYERS 12
#define MAX_CMD_LENGTH 51
#define MAX_STEPS 13
#define MAX_PIECE_LEN 10
#define NUM_NEW_PAWN_OPTIONS 4

#define PAWN_SCORE 1
#define KNIGHT_SCORE 3
#define BISHOP_SCORE 3
#define ROOK_SCORE 5
#define QUEEN_SCORE 9
#define KING_SCORE 400
#define WIN_SCORE 500

#define MATE_RET 2
#define CHECK_RET 3
#define RESTART_RET 5
#define INVALID_RET 0
#define OK_RET 1
#define ILLEGAL_RET -1
#define DEFAULT_DEPTH 1
#define DEFAULT_GAME_MODE 1
#define NUM_SAVE_SLOTS 5

typedef char** board_t;

typedef struct {
	int x;
	int y;
} loc;

typedef struct {
	loc from;
	loc to;
	char promotion;
} move;

#define WRONG_GAME_MODE "Wrong game mode\n"
#define WRONG_FILE_NAME "Wrong file name\n"
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. Value should between 1 to 4\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_DISCS "The specified position does not contain your piece\n"
#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"
#define SETTINGS_START "Enter game setting:\n"
#define USER_MOVE "enter your move:\n"

#define SPACE " "
#define KING "king"
#define PAWN "pawn"
#define BISHOP "bishop"
#define ROOK "rook"
#define KNIGHT "knight"
#define QUEEN "queen"
#define BLACK "Black"
#define WHITE "White"
#define BLACK_VAL 1
#define WHITE_VAL 0

#define perror_message(func_name) (perror("Error: standard function %s has failed", func_name))
#define print_message(message) (printf("%s", message));
#define GUI_CMD "gui"
#define CONSOLE_CMD "console"

extern char* new_pawn_options[NUM_NEW_PAWN_OPTIONS];
extern char new_pawn_keys[NUM_NEW_PAWN_OPTIONS];

/* Settings.C Functions */
void init_board_(char** board);
void copy_board(char*** demi_board,char** board);
void print_board_(char** board);
void print_line();
void free_board(char*** board);
int load_board(char** board, char* load_file, int* game_mode, char* user_color, char* curr_color, int* depth);
int save_board(char** board, char* save_file, int game_mode, char* curr_color, int depth);
char* opponent_color(char* color);
int opponent_piece(char piece, char* color);

/* Chess.C Functions */
int user_turn(char** board, char *player_color, int game_mode, int depth);
int computer_turn(char** board, char* computer_color, int depth, char* computer_best_move);
int user_move(char** board, char *player_color, move* my_move);
void init_pos(loc* my_move);
int init_move(move* my_move);
int is_valid_pos(loc location);
int is_legal_move(char** board, loc location, char* player_color);
int is_eat_move(char** board, loc location, char* player_color);
int validate_move(char** board, move* my_move, char* user_color);
void make_move(char** board, move* my_move);

/* GetMoves.c Functions */
int get_moves(char** board, move** all_moves, char *player_color, int check_risky);
int get_location_moves(char** board, loc pos, move** all_moves, int* num_moves, char *player_color, int check_risky);
int get_piece_moves(char** board, loc pos, int directions[8][2], int num_dirs, move** all_moves, int* num_moves, char *player_color, int is_recursive, int check_risky);
int is_player_threatening_opponent_king(char** board, move* all_moves, int num_moves, char* opp_player_color);
int is_risky_move(char** board, loc pos, loc next, char* player_color);
int is_player_checkmated_opponent(char** board, char *player_color);
int is_player_checked_opponent(char** board, char *player_color);
void print_moves(move* all_moves, int num_moves);

/* Minimax.c Functions */
int minimax(char** board, move** moves, int curr_depth, int max_depth, char* color, int max_val, int min_val, int is_max_turn);
int scoring_function_minimax(char** board, char* curr_color, char *player_color);

#endif 

