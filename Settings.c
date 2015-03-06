#include "Chess.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Initialize standard chess board */
void init_board_(char** board){
	
	int i,j;

	/* Empty slots */
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			board[i][j] = EMPTY;
		}
	}

	/* Pawn slots */
	for (i = 0; i < BOARD_SIZE; i++){
		board[i][1] = PAWN_W;
		board[i][6] = PAWN_B;
	}

	board[0][0] = QUEEN_W;
	board[0][0] = ROOK_W;
	board[0][7] = ROOK_B;
	board[1][0] = KNIGHT_W;
	board[1][7] = KNIGHT_B;
	board[2][0] = BISHOP_W;
	board[2][7] = BISHOP_B;
	board[3][0] = QUEEN_W;
	board[3][7] = QUEEN_B;
	board[4][0] = KING_W;
	board[4][7] = KING_B;
	board[5][0] = BISHOP_W;
	board[5][7] = BISHOP_B;
	board[6][0] = KNIGHT_W;
	board[6][7] = KNIGHT_B;
	board[7][0] = ROOK_W;
	board[7][7] = ROOK_B;
}

/* Dynamically allocate demi-board and copy the original board data */
void copy_board(char*** demi_board, char** board){
	
	int i,j;

	*demi_board = (char**)malloc(sizeof(char*)*BOARD_SIZE);
	if (*demi_board == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}

	for (i = 0; i < BOARD_SIZE; i++){
		(*demi_board)[i] = (char*)malloc(sizeof(char)*BOARD_SIZE);
		if ((*demi_board)[i] == NULL){
			perror("Error: standard function malloc has failed");
			exit(0);
		}
	}

	/* Copy board to demi-board */
	for(i = 0; i < BOARD_SIZE; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			(*demi_board)[i][j] = board[i][j];
		}
	}
}

/* Prints board to console */
void print_board_(char** board)
{
	int i,j;
		print_line();
		for (j = BOARD_SIZE-1; j >= 0 ; j--)
		{
			printf("%d", j+1);
			for (i = 0; i < BOARD_SIZE; i++){
				printf("| %c ", board[i][j]);
			}
			printf("|\n");
			print_line();
		}
		printf("  ");
		for (j = 0; j < BOARD_SIZE; j++){
			printf(" %c  ", (char)('a' + j));
		}
		printf("\n");
}

void print_line(){
	int i;
	printf(" |");
	for (i = 1; i < BOARD_SIZE*4; i++){
		printf("-");
	}
	printf("|\n");
}

/* Free allocation for 2 dimentional board */
void free_board(char*** board){

	int i;

	for (i = 0; i < BOARD_SIZE; i++)
		free((*board)[i]);
	free(*board);
}

/* Load board from file and set relevant arguments */
int load_board(char** board, char* load_file, int* game_mode, char* user_color, char* curr_color, int* depth){

	
	FILE* input;
	char text[10];
	int i, j, row_num;
	
	text[5] = EOL;
	input = fopen(load_file, "r");

	if (input == NULL) {
		return 0;
	}
	
	/* Read file in specific format */
    fscanf(input, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<game>\n");
    fscanf(input, "\t<type>%d</type>\n", game_mode);

	/* If we're in player vs AI - Read depth and user color */
	if (*game_mode == PLAYER_VS_COMP){

		fscanf(input, "\t<difficulty>%c", text);
		if (isdigit(text[0])){
			*depth = text[0] - '0';
			fscanf(input, "</difficulty>\n");
		}
		else{
			*depth = MAX_DEPTH+1;
			fscanf(input, "%c%c%c</difficulty>\n", text, text+1, text+2);
		}

		fscanf(input, "\t<user_color>%c%c%c%c%c</user_color>\n", text, text+1, text+2, text+3, text+4);
		
		for (i = 1; i < 5; i++)
			text[i] = tolower(text[i]);

		if (!strcmp(text, BLACK))  
			strcpy(user_color, BLACK);
		else if (!strcmp(text, WHITE))
			strcpy(user_color, WHITE);
	}
    
	fscanf(input, "\t<next_turn>%c%c%c%c%c</next_turn>\n", text, text+1, text+2, text+3, text+4);
    
	for (i = 1; i < 5; i++)
		text[i] = tolower(text[i]);
	
	if (!strcmp(text, BLACK))  
		strcpy(curr_color, BLACK);
	else if (!strcmp(text, WHITE))
		strcpy(curr_color, WHITE);
	
	text[BOARD_SIZE] = EOL;
    fscanf(input, "\t<board>\n");
	
	/* Read board state */
    for (i = 0; i < BOARD_SIZE; ++i)
    {
        fscanf(input, "\t\t<row_%d>%c%c%c%c%c%c%c%c</row_%d>\n", &row_num, text, text+1, text+2, text+3, text+4, text+5, text+6, text+7, &row_num);
        for (j = 0; j < BOARD_SIZE; j++)
        {
            if (text[j] == '_')
                board[j][row_num-1] = EMPTY;
			else
				board[j][row_num-1] = text[j];
        }
    }

	fclose(input);
    return 1;
}

/* Save board and game state to file */
int save_board(char** board, char* save_file, int game_mode, char* curr_color, int depth){

	
	FILE* output;
	int i, j;
	
	output = fopen(save_file, "w");

	if (output == NULL) {
		return 0;
	}

	fseek(output, 0, SEEK_SET);

	/* Read file in specific format */
    fprintf(output, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<game>\n");
    fprintf(output, "\t<type>%d</type>\n", game_mode);

	/* If we're in player vs AI - Read depth and user color */
	if (game_mode == PLAYER_VS_COMP){

		if (depth == (MAX_DEPTH+1)){
			fprintf(output, "\t<difficulty>Best</difficulty>\n");
		}
		else{
			fprintf(output, "\t<difficulty>%d</difficulty>\n", depth);
		}
		
		if (!strcmp(curr_color, BLACK))  
			fprintf(output, "\t<user_color>%s</user_color>\n", BLACK);
		else if (!strcmp(curr_color, WHITE))
			fprintf(output, "\t<user_color>%s</user_color>\n", WHITE);
	}

	if (!strcmp(curr_color, BLACK))  
		fprintf(output, "\t<next_turn>%s</next_turn>\n", BLACK);
	else if (!strcmp(curr_color, WHITE))
		fprintf(output, "\t<next_turn>%s</next_turn>\n", WHITE);

    fprintf(output, "\t<board>\n");

	/* Write board state */
    for (i = BOARD_SIZE - 1; i >= 0; --i)
    {
        fprintf(output, "\t\t<row_%d>", i+1);
        for (j = 0; j < BOARD_SIZE; j++)
        {
            if (board[j][i] == EMPTY)
				fprintf(output, "%c", '_');
			else
				fprintf(output, "%c", board[j][i]);
        }
		fprintf(output, "</row_%d>\n", i+1);
    }

    fprintf(output, "\t</board>\n</game>\n");
	fclose(output);
    return 1;
}

/* Return the opposite color of the parameter */
char* opponent_color(char* color) {

	if(!strcmp(color,WHITE)){
		return BLACK;
	}
	else {
		return WHITE;
	}
}

/* Checks if a piece is in the opposite color */
int opponent_piece(char piece, char* color){

	if (!strcmp(color, WHITE)){
		if (piece == toupper(piece)){
			return 1;
		}
	}
	else{
		if (piece == tolower(piece)){
			return 1;
		}
	}	
	
	return 0;
}

