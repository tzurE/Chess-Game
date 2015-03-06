#include "Chess.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Find all available move for a specific color player */
int get_moves(char** board, move** all_moves, char *player_color, int check_risky){

	int i, j;
	char piece;
	loc pos;
	int num_moves = 0;

	/* Find all pieces of my color */
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			
			piece = board[i][j];
			pos.x = i;
			pos.y = j;

			if (piece != EMPTY){
				if (!strcmp(player_color, WHITE)){
					if (piece == tolower(piece)){
						if (get_location_moves(board, pos, all_moves, &num_moves, player_color, check_risky) == ILLEGAL_RET)
							return ILLEGAL_RET;
					}
				}

				if (!strcmp(player_color, BLACK)){
					if (piece == toupper(piece)){
						if (get_location_moves(board, pos, all_moves, &num_moves, player_color, check_risky) == ILLEGAL_RET){
							return ILLEGAL_RET;
						}
					}
				}
			}
		}
	}

	return num_moves;
}

/* Find list of available moves for a specific location on board */ 
int get_location_moves(char** board, loc pos, move** all_moves, int* num_moves, char *player_color, int check_risky){

	char piece = board[pos.x][pos.y];
	char piece_kind = tolower(board[pos.x][pos.y]);

	/* lists of available moving direction for piece kind */
	int king_dirs[8][2] = {{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};
	int white_pawn_dirs[8][2] = {{0,1},{-1,1},{1,1}};
	int black_pawn_dirs[8][2] = {{0,-1},{-1,-1},{1,-1}};
	int knight_dirs[8][2] = {{-2,1},{-1,2},{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1}};
	int bishop_dirs[8][2] = {{-1,1},{1,1},{1,-1},{-1,-1}};
	int rook_dirs[8][2] = {{-1,0},{0,1},{1,0},{0,-1}};

	if (piece == EMPTY){
		return ILLEGAL_RET;
	}
	else if (!strcmp(player_color, WHITE)){
		if (piece == toupper(piece))
			return ILLEGAL_RET;
	}
	else if (!strcmp(player_color, BLACK)){
		if (piece == tolower(piece))
			return ILLEGAL_RET;
	}

	/* Sends the relevant directions list for the next function to simulate */
	if (piece == PAWN_W)
		return get_piece_moves(board, pos, white_pawn_dirs, 3, all_moves, num_moves, player_color, 0, check_risky);
	else if (piece == PAWN_B)
		return get_piece_moves(board, pos, black_pawn_dirs, 3, all_moves, num_moves, player_color, 0, check_risky);
	else if (piece_kind == KING_W)
		return get_piece_moves(board, pos, king_dirs, 8, all_moves, num_moves, player_color, 0, check_risky);
	else if (piece_kind == BISHOP_W)
		return get_piece_moves(board, pos, bishop_dirs, 4, all_moves, num_moves, player_color, 1, check_risky);
	else if (piece_kind == ROOK_W)
		return get_piece_moves(board, pos, rook_dirs, 4, all_moves, num_moves, player_color, 1, check_risky);
	else if (piece_kind == KNIGHT_W)
		return get_piece_moves(board, pos, knight_dirs, 8, all_moves, num_moves, player_color, 0, check_risky);
	else if (piece_kind == QUEEN_W)
		return get_piece_moves(board, pos, king_dirs, 8, all_moves, num_moves, player_color, 1, check_risky);
	else
		return ILLEGAL_RET;
}

/* Find list of available moves for a specific piece */
/* is_recursive = if a piece can make multiple steps */
/* Check risky = check if my move risks my king */
int get_piece_moves(char** board, loc pos, int directions[8][2], int num_dirs, move** all_moves, int* num_moves, 
					char *player_color, int is_recursive, int check_risky){

	int i, j, recurse;
	loc next;
	char piece = board[pos.x][pos.y];

	for (i = 0; i < num_dirs; i++){

		next.x = pos.x;
		next.y = pos.y;
		next.x += directions[i][0];
		next.y += directions[i][1];
		recurse = 1;

		/* While a move is valid, and can make multiple steps */
		while (is_valid_pos(next) && (is_legal_move(board, next, player_color)) && recurse){

			/* update wheter a move can be recursive */
			if (board[next.x][next.y] != EMPTY){
				recurse = 0;
			}
			else{
				recurse = is_recursive;
			}

			/* For PAWN only - Breaks if diagonal steps are not eating */
			if (tolower(piece) == PAWN_W){
				if (i && (!is_eat_move(board, next, player_color)))
					break;
				if (!i && (is_eat_move(board, next, player_color)))
					break;
			}

			/* Breaks if a steps risks my king */
			if (check_risky){

				if (is_risky_move(board, pos, next, player_color)){
					if (is_eat_move(board, next, player_color)){
						break;
					}
					else{
						next.x += directions[i][0];
						next.y += directions[i][1];
						continue;
					}
				}
			}
		
			/* A pawn promotion is needed */
			if (((piece == PAWN_W) && (next.y == BOARD_SIZE-1)) ||
				((piece == PAWN_B) && (next.y == 0))){

				/* Increases moves list and update it with the next available step */
				*all_moves = (move*)realloc(*all_moves,((*num_moves)+NUM_NEW_PAWN_OPTIONS)*sizeof(move));	
				if (*all_moves == NULL){
					print_message("failed to allocate memory");
					exit(0);
				}

				/* Inits all next steps with all promotion options */
				for (j = 0; j < NUM_NEW_PAWN_OPTIONS; j++){

					(*all_moves+(*num_moves))->from.x = pos.x;
					(*all_moves+(*num_moves))->from.y = pos.y;
					(*all_moves+(*num_moves))->to.x = next.x;
					(*all_moves+(*num_moves))->to.y = next.y;
					(*all_moves+(*num_moves))->promotion = new_pawn_keys[j];
					(*num_moves)++;
				}
			}
			/* Regular move */
			else{

				/* Increases moves list and update it with the next available step */
				*all_moves = (move*)realloc(*all_moves,((*num_moves)+1)*sizeof(move));
				if (*all_moves == NULL){
					print_message("failed to allocate memory");
					exit(0);
				}

				(*all_moves+(*num_moves))->from.x = pos.x;
				(*all_moves+(*num_moves))->from.y = pos.y;
				(*all_moves+(*num_moves))->to.x = next.x;
				(*all_moves+(*num_moves))->to.y = next.y;
				
				(*all_moves+(*num_moves))->promotion = EOL;
				(*num_moves)++;
			}
			next.x += directions[i][0];
			next.y += directions[i][1];
		}	
	}	

	return OK_RET;
}

/* Checks if current player is threatening opponent king, AKA chess */
int is_player_threatening_opponent_king(char** board, move* all_moves, int num_moves, char* opp_player_color){

	int i;
	char piece;

	/* Checks if the opposite player can eat my king */
	for (i = 0; i < num_moves; i++){
		
		piece = board[(all_moves+i)->to.x][(all_moves+i)->to.y];
		if (!strcmp(opp_player_color, WHITE)){
			if (piece == KING_W){
				return 1;
			}
		}
		else {
			if (piece == KING_B){
				return 1;
			}
		}
	}

	return 0;
}

/* Checks if my move risks my king */
int is_risky_move(char** board, loc pos, loc next, char* player_color){

	move* opp_moves;
	int num_opp_moves;
	char** demi_board;

	copy_board(&demi_board, board);

	/* Simulates the opposite player's move after my move */
	demi_board[next.x][next.y] = board[pos.x][pos.y];
	demi_board[pos.x][pos.y] = EMPTY;
	
	opp_moves = (move*)malloc(sizeof(move));
	if (opp_moves == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}

	num_opp_moves = get_moves(demi_board, &opp_moves, opponent_color(player_color), 0);
	
	/* Is my king is threatened by the opponent */
	if (is_player_threatening_opponent_king(demi_board, opp_moves, num_opp_moves, player_color)){
		free_board(&demi_board);
		free(opp_moves);
		return 1;
	} 

	free_board(&demi_board);
	free (opp_moves);
	return 0;
}

/* Check if player checkmated the opponent, AKA checkmate */
int is_player_checkmated_opponent(char** board, char *player_color){
	
	move* all_moves;
	int num_moves = 0;

	all_moves = (move*)malloc(sizeof(move));
	if (all_moves == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}

	num_moves = get_moves(board, &all_moves, opponent_color(player_color), 1);
	free(all_moves);

	/* If the opponent has no available moves => player checkmates */
	if(!num_moves)
		return 1;

	return 0;
}

/* Check is this color's player losed  */
int is_player_checked_opponent(char** board, char *player_color){
	
	move* all_moves;
	int num_moves = 0;

	all_moves = (move*)malloc(sizeof(move));
	if (all_moves == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}

	num_moves = get_moves(board, &all_moves, player_color, 1);

	if (is_player_threatening_opponent_king(board, all_moves, num_moves, opponent_color(player_color))){
		free(all_moves);
		return 1;
	}

	free(all_moves);
	return 0;
}

/* Print all moves to screen */
void print_moves(move* all_moves, int num_moves){

	int i, j;

	for(i = 0; i < num_moves; i++){
		printf("<%c,%d> to ",(char)((all_moves+i)->from.x+97) ,(all_moves+i)->from.y+1);
		printf("<%c,%d>", (char)((all_moves+i)->to.x+97) ,(all_moves+i)->to.y+1);
		
		if ((all_moves+i)->promotion != EOL){
			for (j = 0; j < NUM_NEW_PAWN_OPTIONS; j++){

				if ((all_moves+i)->promotion == new_pawn_keys[j])
					printf(" %s", new_pawn_options[j]);
			}
		}
		printf("\n");
	}
}