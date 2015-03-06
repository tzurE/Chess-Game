#include "Chess.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "controlsv2.h"
#include <limits.h>

/* Pawn promotion options */
char* new_pawn_options[NUM_NEW_PAWN_OPTIONS] = {QUEEN, BISHOP, ROOK, KNIGHT};
char new_pawn_keys[NUM_NEW_PAWN_OPTIONS] = {QUEEN_W, BISHOP_W, ROOK_W, KNIGHT_W};

/*SDL main items*/
int quit, cancel;

int main(int argc, char* args[]){

	char set_cmd[MAX_CMD_LENGTH], color_param[6], user_color[6] = WHITE, curr_color[6] = WHITE;
	char load_file[MAX_CMD_LENGTH], computer_best_move[MAX_CMD_LENGTH];
	int depth_param, param, move_ret_val, depth = DEFAULT_DEPTH;
	int game_mode = DEFAULT_GAME_MODE, start = 0, restart = 1, i;
	char** board;
	
	/* Allocate game board */
	board = (char**) malloc(sizeof(char*)*BOARD_SIZE);
	if (board == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}

	for (i = 0; i < BOARD_SIZE; i++){
		board[i] = (char*) malloc(sizeof(char)*BOARD_SIZE);
		if (board[i] == NULL){
			perror("Error: standard function malloc has failed");
			exit(0);
		}
	}
	
	/* Initiate the gui component */
	if ((argc == 2) && (!strcmp(args[1], GUI_CMD))){
		
		/* Place all pieces on the board */
		init_board_(board);
		main_gui(board);
		free_board(&board);
		return 0;
	}
	
	/* Main loop */
	if ((argc == 1) || (!strcmp(args[1], CONSOLE_CMD))){
		
		while(restart){

			restart = 0;
			start = 0;
			depth = DEFAULT_DEPTH;
			game_mode = DEFAULT_GAME_MODE;
			strcpy(user_color, WHITE);
			strcpy(curr_color, WHITE);
			init_board_(board);
			print_board_(board);
			print_message(SETTINGS_START);
			scanf("%s",set_cmd);

			/****************************settings state******************************/
			while(!start){
		
				/* Choosing Game Mode */
				if(!strcmp(set_cmd,"game_mode")){

					scanf("%d", &param);
					if (param == TWO_PLAYER){
						game_mode = param;
						printf("Running game in 2 players mode\n");
					}
					else if (param == PLAYER_VS_COMP){
						game_mode = param;
						printf("Running game in player vs. AI mode\n");
					}
					else
						print_message(WRONG_GAME_MODE);
				}
		
				/* Setting MINIMAX depth */
				else if(!strcmp(set_cmd,"difficulty")){
				
					if (game_mode == TWO_PLAYER){
							fgets(set_cmd, MAX_CMD_LENGTH, stdin);
							print_message(ILLEGAL_COMMAND);
					}
					else{
						scanf("%s", set_cmd);
						if (!strcmp(set_cmd, "depth")){
							scanf("%d", &depth_param);
							if ((depth_param < MIN_DEPTH) || (depth_param > MAX_DEPTH)){
								print_message(WRONG_MINIMAX_DEPTH);
							}
							else{
								depth = depth_param;
							}
						}
						else if (!strcmp(set_cmd, "best")){
							depth = MAX_DEPTH+1;
						}
						else{
								fgets(set_cmd, MAX_CMD_LENGTH, stdin);
								print_message(ILLEGAL_COMMAND);
						}
					}
				}
				/* Set player's color */
				else if(!strcmp(set_cmd,"user_color")){
					scanf("%s", user_color);
					if (game_mode == TWO_PLAYER){
						print_message(ILLEGAL_COMMAND);
					}
					else if ((!strcmp(color_param, BLACK)) || (!strcmp(color_param, WHITE))){
						strcpy(user_color, color_param);
						strcpy(curr_color, color_param);
					}
					else{
						print_message(ILLEGAL_COMMAND);
					}
				}
				else if(!strcmp(set_cmd, "load")){
					scanf("%s", load_file);
					if (!load_board(board, load_file, &game_mode, user_color, curr_color, &depth)){
						print_message(WRONG_FILE_NAME);
					}
					else{
						print_board_(board);
					}
				}
				else if(!strcmp(set_cmd, "print")){
					print_board_(board);
				}
				else if(!strcmp(set_cmd,"start")){
					start = 1;
					break;
				}
				else if(!strcmp(set_cmd,"quit")){
					free_board(&board);
					return 0;
				}
				else{
					fgets(set_cmd, MAX_CMD_LENGTH, stdin);
					print_message(ILLEGAL_COMMAND);
				}
				scanf("%s",set_cmd);
			}

			/****************************game state***************************/
			start = 0;
			restart = 0;

			/* Player VS Player */
			if (game_mode == TWO_PLAYER){

				while(!restart){

					move_ret_val = user_turn(board, curr_color, game_mode, depth);

					/* Restart game */
					if (move_ret_val == RESTART_RET){
						restart = RESTART_RET;
						start = 0;
					}
					/* Player moved */
					else{
						print_board_(board);

						/* Player checkmates opponent (wins)*/
						if (move_ret_val == MATE_RET){
							printf("Mate! %s player wins the game\n", curr_color);
							break;
						}
						/* Player checks opponent*/
						else if (move_ret_val == CHECK_RET){
							printf("Check!\n");				
						}
						/* Change to opponent player */
						strcpy(curr_color, opponent_color(curr_color));	
					}
				}
			}
			else if (game_mode == PLAYER_VS_COMP){
			
				while(!restart){
				
					/* User's move */
					if (!strcmp(user_color, curr_color)){
						move_ret_val = user_turn(board, user_color, game_mode, depth);
					}
					/* Computer's move */
					else{
						move_ret_val = computer_turn(board, curr_color, depth, computer_best_move);
						puts(computer_best_move);
					}

					if (move_ret_val == RESTART_RET){
						restart = RESTART_RET;
						start = 0;
					}
					else{
						print_board_(board);

						if (move_ret_val == MATE_RET){
							printf("Mate! %s player wins the game!\n", curr_color);
							break;
						}
						else if (move_ret_val == CHECK_RET){
							printf("Check!\n");
						}

						strcpy(curr_color, opponent_color(curr_color));
					}
				}
			}	
		}
	}
	free_board(&board);
	return 0;
}

/* User's turn and its options */
int user_turn(char** board, char *player_color, int game_mode, int depth){
	
	move* all_moves, *my_move;
	loc pos;
	char user_cmd[MAX_CMD_LENGTH], save_file[MAX_CMD_LENGTH];
	int num_moves = 0, user_move_val = ILLEGAL_RET;
	
	while(user_move_val == ILLEGAL_RET){
		
		printf("%s player - ", player_color);
		print_message(USER_MOVE);
		scanf("%s",user_cmd);

		if(!strcmp(user_cmd,"quit")){
			free_board(&board);
			exit(0);
		}
		else if(!strcmp(user_cmd,"restart")){
			user_move_val = RESTART_RET;
		}
		/* Simulate user's move */
		else if(!strcmp(user_cmd, "move")){
			my_move = (move*)malloc(sizeof(move));
			if (my_move == NULL){
				perror("Error: standard function malloc has failed");
				exit(0);
			}
	
			if (init_move(my_move) == ILLEGAL_RET){
				free(my_move);
				print_message(ILLEGAL_COMMAND);
			}
			else{
				user_move_val = user_move(board, player_color, my_move);
				free(my_move);
				
				if (user_move_val == ILLEGAL_RET){
					print_message(ILLEGAL_COMMAND);
				}
			}
		}
		/* Get moves for location specified by user */
		else if(!strcmp(user_cmd, "get_moves")){

			init_pos(&pos);
			num_moves = 0;
			all_moves = (move*)malloc(sizeof(move));
			if (all_moves == NULL){
				perror("Error: standard function malloc has failed");
				exit(0);
			}

			/* Location is invalid - not my color, or empty square */
			if ((!is_valid_pos(pos)) || (get_location_moves(board, pos, &all_moves, &num_moves, player_color, 1) == ILLEGAL_RET)){
				free(all_moves);
				print_message(ILLEGAL_COMMAND);
			}
			else{
				print_moves(all_moves, num_moves);
				free(all_moves);
			}
		}
		else if(!strcmp(user_cmd, "save")){
			scanf("%s", save_file);
			if (!save_board(board, save_file, game_mode, player_color, depth))
				print_message(WRONG_FILE_NAME);
		}
		else{
			fgets(user_cmd, MAX_CMD_LENGTH, stdin);
			print_message(ILLEGAL_COMMAND);
		}
	}
	return user_move_val;
}

/* Computer's turn, makes best move */
int computer_turn(char** board, char *computer_color, int depth, char* computer_best_move){

	int best_move_index;
	move* all_moves;	

	all_moves = (move*)malloc(sizeof(move));
	if (all_moves == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}

	/* Find best move */
	best_move_index = minimax(board, &all_moves, depth, depth, computer_color, INT_MIN, INT_MAX, 1);
	make_move(board, &(all_moves[best_move_index]));
	sprintf(computer_best_move, "Computer: move <%c,%d> to <%c,%d>\n",(char)all_moves[best_move_index].from.x+97 , all_moves[best_move_index].from.y+1,
															(char)all_moves[best_move_index].to.x+97 , all_moves[best_move_index].to.y+1);
	free(all_moves);

	/* Computer checkmates opponent */
	if (is_player_checkmated_opponent(board, computer_color))
		return MATE_RET;

	/* Computer checks opponent */
	else if (is_player_checked_opponent(board, computer_color))
		return CHECK_RET;

	return OK_RET;
}

/* User's turn, makes a move */
int user_move(char** board, char *player_color, move* my_move){

	/* Check if move is valid */
	if (validate_move(board, my_move, player_color) == ILLEGAL_RET){
		return ILLEGAL_RET;
	}

	/* Make the move */
	make_move(board, my_move);

	/* Player checkmates opponent */
	if (is_player_checkmated_opponent(board, player_color))
		return MATE_RET;
	
	/* Player checks opponent */
	else if (is_player_checked_opponent(board, player_color))
		return CHECK_RET;

	return OK_RET;
}

/* Init position variable from user's input */
void init_pos(loc* my_move){

	char* tok, start_param[6];

	/* getting the user input */
	scanf("%s", start_param);
    tok = strtok (start_param," <,>");
	(*my_move).x = ((int)(tok[0])-97);
	tok = strtok (NULL, " <,>");
	(*my_move).y = atoi(tok)-1;
}

/* Init move variable from user's input */
int init_move(move* my_move){

	char* tok, start_param[6], next_param[MAX_CMD_LENGTH], to[4], new_pawn[MAX_CMD_LENGTH];
	int i = 0;

	/* getting the user input */
	scanf("%s", start_param);
    tok = strtok (start_param," <,>");
	my_move->from.x = ((int)(tok[0])-97);
	tok = strtok (NULL, " <,>");
	my_move->from.y = atoi(tok)-1;
	
	/* ignore the "to" part */
	scanf("%s", to);
	scanf("%s", next_param);
	tok = strtok (next_param,"<,>");

	my_move->to.x = ((int)(tok[0])-97);
	tok = strtok (NULL,"<,>");
	my_move->to.y = atoi(tok)-1;
	tok = strtok (NULL, "<,>");

	fgets(new_pawn, MAX_CMD_LENGTH, stdin);
	my_move->promotion = EOL;
	new_pawn[strlen(new_pawn)-1] = EOL;

	if (strlen(new_pawn)){
		for (i = 0; i < NUM_NEW_PAWN_OPTIONS; i++){
			if (!strcmp(new_pawn+1, new_pawn_options[i])){
				my_move->promotion = new_pawn_keys[i];
				return OK_RET;
			}
		}
	}

	/* new_pawn entered isn't a valid option */
	if (strlen(new_pawn)){
		return ILLEGAL_RET;
	}
	return OK_RET;
}

/* Check if position is within board range */
int is_valid_pos(loc location){

	if((location.x < 0) || (location.x > (BOARD_SIZE-1)) || (location.y < 0) || (location.y > (BOARD_SIZE-1)))
		return 0;

	return 1;
}

/* Validates if a move is legal, doesn't eat piece of same color */
int is_legal_move(char** board, loc location, char* color){

	char piece = board[location.x][location.y];

	if (piece == EMPTY)
		return 1;

	if (!strcmp(color, BLACK)){
		
		/* White eats black */
		if(piece == tolower(piece))
			return 1;
	}
	if (!strcmp(color, WHITE)){
		
		/* Black eats white */
		if(piece == toupper(piece))
			return 1;
	}
	return 0;
}

/* Checks if a move eats a piece */
int is_eat_move(char** board, loc location, char* color){

	char piece = board[location.x][location.y];

	if (piece == EMPTY)
		return 0;

	if (!strcmp(color, BLACK)){
		if(piece == tolower(piece))
			return 1;
	}
	if (!strcmp(color, WHITE)){
		if(piece == toupper(piece))
			return 1;
	}
	return 0;
}

/* Check if a user's move is available */
int validate_move(char** board, move* my_move, char* player_color){

	move* all_moves;
	int num_moves = 0, i = 0;

	all_moves = (move*)malloc(sizeof(move));
	if (all_moves == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}

	if (!(is_valid_pos(my_move->from)) || !(is_valid_pos(my_move->to))){
		free(all_moves);
		return ILLEGAL_RET;
	}
	
	/* If promotion needed but not chosen - inits promotion to default option */
	if (my_move->promotion == EOL){
		if (tolower(board[my_move->from.x][my_move->from.y]) == PAWN_W){
			if ((my_move->to.y == 0) || (my_move->to.y == BOARD_SIZE-1)){
				if (!strcmp(player_color, WHITE)){
					my_move->promotion = DEFAULT_PROMOTION;
				}
				else
					my_move->promotion = toupper(DEFAULT_PROMOTION);
			}
		}
	}

	/* Calculate all of user's possible moves */
	if (get_location_moves(board, my_move->from, &all_moves, &num_moves, player_color, 1) == ILLEGAL_RET){
		free(all_moves);
		return ILLEGAL_RET;
	}

	/* Check if move exists in all possible moves */
	for (i = 0; i < num_moves; i++){

		/* Move exists */
		if (((all_moves+i)->to.x == my_move->to.x) && ((all_moves+i)->to.y == my_move->to.y) &&
			((all_moves+i)->promotion == tolower(my_move->promotion))){
			free(all_moves);
			return OK_RET;
		}
	}
	free(all_moves);
	return ILLEGAL_RET;
}

/* Make move on board*/
void make_move(char** board, move* my_move){

	/* This is a pawn promotion move*/
	if (my_move->promotion != EOL){
		
		/* Black pawn promoted*/
		if (my_move->to.y == 0){
			board[my_move->to.x][my_move->to.y] = toupper(my_move->promotion);
		}

		/* White pawn promoted*/
		else{
			board[my_move->to.x][my_move->to.y] = tolower(my_move->promotion);
		}
	}
	
	/* Regular move*/
	else
		board[my_move->to.x][my_move->to.y] = board[my_move->from.x][my_move->from.y];

	board[my_move->from.x][my_move->from.y] = EMPTY;
}
