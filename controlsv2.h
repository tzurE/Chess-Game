#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "SDL_video.h"
#include "SDL_image.h"
#include "Chess.h"

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

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 644
#define RIGHT_MARGIN 644
#define MAIN_MENU_B_H 53
#define MAIN_MENU_B_W 200
#define MAIN_MENU_B_X 300
#define MAIN_MENU_B_Y 250
#define BUTTON_SPACE 55
#define OPT_BUTTON_SPACE 75
#define DIFF_B_SPACE	85
#define SIDE_MENU_X_BUTTONS 659
#define SIDE_MENU_Y_BUTTONS 100
#define SIDE_MENU_B_H 44
#define SIDE_MENU_B_W 128
#define OPT_MENU_B_X 245
#define OPT_MENU_B_Y 200
#define OPT_MENU_B_H 63
#define OPT_MENU_B_W 312
#define COL_B_W 84
#define COL_B_H	83
#define DIFF_B_W 70
#define DIFF_B_H 74
#define RIGHT_WIDTH 156
#define LABEL_SIZE 10
#define CHILDREN_LIST_SIZE 35
#define MAIN_X 0
#define MAIN_Y 0
#define BOARD_SIZE 8
#define BOARD_X0 5
#define BOARD_Y0 569
#define DELTA_X 80
#define DELTA_Y 80

SDL_Rect main_offset;
int quit, cancel, main_game_alloc;

typedef struct Control{
	int flag;
	int curr_size;
	char label[LABEL_SIZE];
	SDL_Surface *surface;
	SDL_Rect rect;
	struct Control *children[CHILDREN_LIST_SIZE];
} Control;


/*controls functions*/
SDL_Surface *load_image(char* filename);
void create_image (SDL_Rect rect, char* image_path, Control *image);
void create_panel(SDL_Rect rect, Control* panel);
void create_button(SDL_Rect rect, char *image_path, Control *button);
void add_children_panel(Control* panel, Control* child);
void apply_surface(SDL_Rect rect,SDL_Surface* source, SDL_Surface* destination);
SDL_Rect create_rect(int x, int y, int w, int h);
void draw_panel(Control* panel, SDL_Surface* screen);
void free_panel(Control* panel);
void remove_last_child_panel(Control *panel);
int isClick(Control* button, SDL_Rect rect);


/*gui.c func*/
int main_gui(char ** board);
int mark_button(Control* button, char *path);
void draw_board(char** board);
int check_where_clicked(Control *Board, SDL_Rect click_loc, loc *location);
void mark_location(SDL_Rect click_loc, char *path);
int main_menu_loop(char** board);
int init_main_menu();
int new_game_options(char** board);
int main_game_loop_pvp(char** board, char* col_choose, char* starting_col);
int main_game_loop_pvc(char** board, char* user_color, char* starting_col, int depth);
int new_game(char** board, int diff_choose, char * col_choose, int game_mode, char* starting_col);
void end_prog();
void add_piece(Control *button, int i, int j,char * path);
void win_gui(char *curr_color);
void promote (char ** ,char* curr_color, int x, int y);
void load_and_save_game_gui(char** board, int mode, int game_mode,char* user_color,char *curr_color,int depth);
int choose_diff_color(char** board);


/*delete after use!!!!!!!!!*/
void init_board_(char** board);







