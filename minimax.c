#include "Chess.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Minimax Function, Calculates next best move according to Depth parameter and scoring function */
int minimax(char** board, move** all_moves, int curr_depth, int max_depth, char* player_color,
			int max_val, int min_val, int is_max_turn){
	
	int i, num_moves = 0;
	int score, best_move_index = -1;
	char** demi_board;
	move* opp_moves;

	/* Stopping condition */
	if (!curr_depth){
		if (is_max_turn)
			return scoring_function_minimax(board, opponent_color(player_color), player_color);
		else
			return scoring_function_minimax(board, opponent_color(player_color), opponent_color(player_color));
	}

	else {
		/* Calculate maximum score */
		if (is_max_turn){
			num_moves = get_moves(board, all_moves, player_color, 1);
			
			for (i = 0; i < num_moves; i++){
				copy_board(&demi_board, board);
				make_move(demi_board, (*all_moves+i));
	
				opp_moves = (move*)malloc(sizeof(move));
				if (opp_moves == NULL){
					perror("Error: standard function malloc has failed");
					exit(0);
				}
								
				score = minimax(demi_board, &opp_moves, curr_depth-1, max_depth, opponent_color(player_color), max_val, min_val, 0);
				free(opp_moves);
				free_board(&demi_board);
				
				if (score > max_val){
					max_val = score;
					best_move_index = i;
				}

				if (score >= min_val){
					break;
				}
			}
		}

		/* Calculate minimum score */
		else {
			
			num_moves = get_moves(board, all_moves, player_color, 1);

			for (i = 0; i < num_moves; i++){
				copy_board(&demi_board, board);
				make_move(demi_board, (*all_moves+i));

				opp_moves = (move*)malloc(sizeof(move));
				if (opp_moves == NULL){
					perror("Error: standard function malloc has failed");
					exit(0);
				}

				score = minimax(demi_board, &opp_moves, curr_depth-1, max_depth, opponent_color(player_color), max_val, min_val, 1);
				free(opp_moves);				
				free_board(&demi_board);

				if (score < min_val){
					min_val = score;
					best_move_index = i;
				}
				if (score <= max_val){
					break;
				}
			}
		}
	}

	if (curr_depth == max_depth)
		return best_move_index;
	
	if (is_max_turn)
		return max_val;

	return min_val;
}

/* Calculates board score, according to the pieces left on board */
int scoring_function_minimax(char** board, char* curr_color, char *player_color){
	
	int my_score = 0, opponent_score = 0, i, j;
	char piece;
	int is_player_win = 0;
	
	is_player_win = is_player_checkmated_opponent(board, curr_color);

	if (!strcmp(curr_color, player_color)){
		
		/* I win */
		if (is_player_win){
			return WIN_SCORE;
		}
	}
	else{
		
		/* I lose */
		if (is_player_win){
			return -WIN_SCORE;
		}
	}
	
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
				
			piece = board[i][j];
				
			if (piece != EMPTY){
				if (opponent_piece(piece, player_color)){

					piece = tolower(piece);
					
					if (piece == PAWN_W)
						opponent_score += PAWN_SCORE;
					if (piece == KNIGHT_W)
						opponent_score += KNIGHT_SCORE;
					if (piece == BISHOP_W)
						opponent_score += BISHOP_SCORE;
					if (piece == ROOK_W)
						opponent_score += ROOK_SCORE;
					if (piece == QUEEN_W)
						opponent_score += QUEEN_SCORE;
					if (piece == KING_W)
						opponent_score += KING_SCORE;
				}
				else{
					piece = tolower(piece);
					
					if (piece == PAWN_W)
						my_score += PAWN_SCORE;
					if (piece == KNIGHT_W)
						my_score += KNIGHT_SCORE;
					if (piece == BISHOP_W)
						my_score += BISHOP_SCORE;
					if (piece == ROOK_W)
						my_score += ROOK_SCORE;
					if (piece == QUEEN_W)
						my_score += QUEEN_SCORE;
					if (piece == KING_W)
						my_score += KING_SCORE;
				}
			}
		}
	}

	return my_score - opponent_score;
}

