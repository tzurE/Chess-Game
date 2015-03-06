#include "SDL.h" 
#include "SDL_video.h"
#include "SDL_image.h"
#include <string.h>
#include "controlsv2.h"
#include "Chess.h"




/*The main images */
SDL_Surface* screen = NULL;
Control *main_menu_panel;
Control *main_game_window;
SDL_Event e; 

/*offset*/
extern SDL_Rect main_offset;
extern int quit, cancel, main_game_alloc;
int x_off[BOARD_SIZE] = {BOARD_X0 , BOARD_X0 + DELTA_X,BOARD_X0 + 2*DELTA_X, BOARD_X0 + 3*DELTA_X,BOARD_X0 + 4*DELTA_X,BOARD_X0 + 5*DELTA_X,BOARD_X0 + 6*DELTA_X,BOARD_X0 + 7*DELTA_X};
int y_off[BOARD_SIZE] = {BOARD_Y0 , BOARD_Y0 - DELTA_Y,BOARD_Y0 - 2*DELTA_Y, BOARD_Y0 - 3*DELTA_Y,BOARD_Y0 - 4*DELTA_Y,BOARD_Y0 - 5*DELTA_Y,BOARD_Y0 - 6*DELTA_Y,BOARD_Y0 - 7*DELTA_Y};



/*The main gui function. inits the UITREE, sets everthing and getting ready to recieve commands from the user*/
int main_gui(char **board1) { 
	char ** board;
	
	/*The board from the console mode, after the "init board" function.*/
	board = board1;
	quit = 0;
	main_game_alloc = 0;
	/*dimensions of the screen*/
	main_offset.x = 0;
	main_offset.y = 0;
	main_offset.h = SCREEN_HEIGHT;
	main_offset.w = SCREEN_WIDTH;

	
	/*load the main menu*/
	if (init_main_menu() == 0){
		printf("error: unable to init SDL");
		exit (0);
	}
	
	/*draw the main menu panel. the drawing is recursive, and in DFS. first the base, than the image than the buttons etc...*/
	draw_panel(main_menu_panel, screen);
	/*this starts the sequance of the game. the main loop in the game. exists when "quit" is issued via the quit button*/
	while (quit == 0){
			main_menu_loop(board);
	}
	end_prog();
	SDL_Quit();
	return 0;
	
	
}
/*main menu builder*/
int init_main_menu(){
	/*A control is a GUI components which is what the view is composed of. A control can be a button, an image, a game window - everything.*/
	/*main menu back is is the background image, the rest are buttons. RECT is a location on the window.*/
	Control *main_menu_back;
	Control *new_game_button, *load_game_button, *quit_button;
	SDL_Rect new_game_pos,load_game_pos,quit_pos;

	/*mem allocating for graphical components*/
	main_menu_panel = (Control*) malloc(sizeof(Control));
	if(main_menu_panel == NULL){
		printf("Error: standard function malloc has failed");
		exit (0);
	}
	
	main_menu_back = (Control*) malloc(sizeof(Control));
	if (main_menu_back == NULL){
		printf("Error: standard function malloc has failed");
		exit(0);
	}
	/*Start SDL - A MUST if we want to use SDL */
	if (SDL_Init( SDL_INIT_VIDEO ) < 0){
		printf("Error: unable to init SDL");
		exit (0);
	}
	
	/*setup screen*/
	 screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0 );
	 if(screen == NULL) {
		printf("error: unable to init screen");
		exit (0);
	 }
	 
	 /*window label*/
	 SDL_WM_SetCaption( "Chess v0.89", NULL );
	 /*create the main menu panel(which holdes all the buttons)*/
	 create_panel(main_offset, main_menu_panel);
	 
	 /*create main menu backgroud image*/
	 create_image(main_offset,"Graphic/main_menu_back3.png", main_menu_back);
	 
	 /*add the background as a child to the main panel - this is important for the recursive printing process*/
	 add_children_panel(main_menu_panel, main_menu_back);
	 /*create buttons as the main menu panel children - a contructor for each button.*/
	 new_game_button = (Control*) malloc(sizeof(Control));
	 if (new_game_button == NULL){
		printf("Error: standard function malloc has failed");
		exit(0);
	}
	 new_game_pos = create_rect(MAIN_MENU_B_X, MAIN_MENU_B_Y,MAIN_MENU_B_W,MAIN_MENU_B_H);
	 create_button(new_game_pos,"Graphic/new_game1.png",new_game_button);
	 add_children_panel(main_menu_panel,new_game_button);

	 load_game_button = (Control*) malloc(sizeof(Control));
	 if (load_game_button == NULL){
		printf("Error: standard function malloc has failed");
		exit(0);
	}
	 load_game_pos = create_rect(MAIN_MENU_B_X, MAIN_MENU_B_Y + BUTTON_SPACE,MAIN_MENU_B_W,MAIN_MENU_B_H);
	 create_button(load_game_pos,"Graphic/load_game1.png",load_game_button);
	 add_children_panel(main_menu_panel,load_game_button);
	 
	 quit_button = (Control*) malloc(sizeof(Control));
	 if (quit_button == NULL){
		printf("Error: standard function malloc has failed");
		exit(0);
	}
	 quit_pos = create_rect(MAIN_MENU_B_X, MAIN_MENU_B_Y + 2*BUTTON_SPACE ,MAIN_MENU_B_W,MAIN_MENU_B_H);
	 create_button(quit_pos,"Graphic/Quit.png",quit_button);
	 add_children_panel(main_menu_panel,quit_button);


	return 1;
}

void end_prog(){
	/*free the main & global surfaces including all the children*/
	free_panel(main_menu_panel);
	if(main_game_alloc == 1){
		free_panel(main_game_window);
		main_game_alloc = 0;
	}
	SDL_FreeSurface(screen);
	SDL_Quit();
	
}

int main_menu_loop(char ** board){
	int depth = 1, mode = 0, game_mode = 1;
	SDL_Rect recti;
	char curr_color[6], user_color[6];

	strcpy(curr_color,WHITE);
	strcpy(user_color,WHITE);
	draw_panel(main_menu_panel, screen); 
	SDL_Flip(screen);
	while (SDL_PollEvent( &e) != 0){
		cancel = 0;
		if(e.type == SDL_MOUSEBUTTONDOWN){
			/*get even details*/
				if(quit == 0){
					/*getting the cords for the click*/
					recti = create_rect(e.button.x,e.button.y, 0,0);
					/*new game?*/
					if( isClick(main_menu_panel->children[1], recti)){
						new_game_options(board);
						draw_panel(main_menu_panel, screen);	
					}
					/*load game?*/
					if(isClick(main_menu_panel->children[2], recti) ){
						load_and_save_game_gui(board,mode,game_mode,user_color,curr_color,depth);
						draw_panel(main_menu_panel, screen);
						
					}					
					/*quit game?*/
					if( isClick(main_menu_panel->children[3], recti)){
						quit = 1;
						SDL_Flip(screen);
					}
				}
			}
		
		if(e.type == SDL_QUIT){
			quit = 1;
		}
	}
	return 1;
	}

void load_and_save_game_gui(char** board, int mode, int game_mode,char* user_color,char *curr_color,int depth){
	Control* load_dialog, *back, *save_slot, *ok, *Cancel;
	SDL_Rect click_loc;
	/*mode == 1 means save, mode == 0 means load*/
	int i, choose = 0, close = 0, slot = 0;
	char** board1;
	char text[MAX_CMD_LENGTH];
	FILE *input;

	load_dialog = (Control*) malloc(sizeof(Control));
	if (load_dialog == NULL){
		printf("Error: standard function malloc has failed");
		exit(0);
	}
	create_panel(create_rect(0, 140, 800,153),load_dialog);
	back = (Control*) malloc(sizeof(Control));
	if (back == NULL){
		printf("Error: standard function malloc has failed");
		exit(0);
	}
	if(mode == 0)
		create_image(load_dialog->rect,"Graphic/load_game_back1.png", back);
	else 
		create_image(load_dialog->rect,"Graphic/save_game_back1.png", back);
	add_children_panel(load_dialog,back);
	
	for(i = 0; i < NUM_SAVE_SLOTS; i++){
		save_slot = (Control*) malloc(sizeof(Control));
		if (save_slot == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
		}
		
		sprintf(text, "%d.xml",i);
		input = fopen(text,"r");
		if (input == NULL){
			create_button(create_rect(50 + i*2*OPT_BUTTON_SPACE, 170,100,65),"Graphic/save_empty.png",save_slot);
			}
		else {
			create_button(create_rect(50 + i*2*OPT_BUTTON_SPACE, 170,100,65),"Graphic/save_full.png",save_slot);
			fclose(input);
		}
		add_children_panel(load_dialog,save_slot);
		input = NULL;

	}
	
	ok = (Control*) malloc(sizeof(Control));
	if (ok == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	create_button(create_rect(275 - 2, 250, 75,40),"Graphic/ok1.png", ok);
	add_children_panel(load_dialog,ok);

	Cancel = (Control*) malloc(sizeof(Control));
	if (Cancel == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	create_button(create_rect(450 - 2, 250, 75,40),"Graphic/cancel_s1.png", Cancel);
	add_children_panel(load_dialog,Cancel);
	draw_panel(load_dialog, screen);
	SDL_Flip(screen);

	copy_board(&board1, board);
	while(close == 0){
		while (SDL_PollEvent( &e) != 0)	{
			if(e.type == SDL_MOUSEBUTTONDOWN){
				click_loc = create_rect(e.button.x, e.button.y, 0,0);
				if(isClick(ok,click_loc) && choose == 1){
					if(mode == 0){
						new_game(board1,depth,user_color,game_mode,curr_color);
						close = 1;
						cancel = 1;
						break;
					}
					else if(mode == 1){
						sprintf(text,"%d.xml", slot);
						save_board(board1,text, game_mode,curr_color,depth);
						close = 1;
					}
				}
				if(isClick(Cancel,click_loc)){
					close = 1;
					break;
				}
				if(isClick(load_dialog->children[1], click_loc)){
					/*is the save empty?*/
					input = fopen("0.xml","r");
					if(mode == 1){
						if(input == NULL){	
							mark_button(load_dialog->children[1],"Graphic/save_empty_c.png");
							slot = 0;
							choose = 1;
						}	
						else{
							mark_button(load_dialog->children[1],"Graphic/save_full_c.png");
							slot = 0;
							choose = 1;
						}
					}
					if(mode == 0) {
						if(input != NULL){
							mark_button(load_dialog->children[1],"Graphic/save_full_c.png");
							load_board(board1,"0.xml",&game_mode,user_color,curr_color,&depth);
							choose = 1;
						}
					}
					break;
				}
				if(isClick(load_dialog->children[2], click_loc) && choose == 0){
					/*is the save empty?*/
					input = fopen("1.xml","r");
					if(mode == 1){
						if(input == NULL){	
							mark_button(load_dialog->children[2],"Graphic/save_empty_c.png");
							slot = 1;
							choose = 1;
						}	
						else{
							mark_button(load_dialog->children[2],"Graphic/save_full_c.png");
							slot = 1;
							choose = 1;
						}
					}
					if(mode == 0) {
						if(input != NULL){
							mark_button(load_dialog->children[2],"Graphic/save_full_c.png");
							load_board(board1,"1.xml",&game_mode,user_color,curr_color,&depth);
							choose = 1;
						}
					}
					break;
				}
				if(isClick(load_dialog->children[3], click_loc) && choose == 0){
					/*is the save empty?*/
					input = fopen("2.xml","r");
					if(mode == 1){
						if(input == NULL){	
							mark_button(load_dialog->children[3],"Graphic/save_empty_c.png");
							slot = 2;
							choose = 1;
						}	
						else{
							mark_button(load_dialog->children[3],"Graphic/save_full_c.png");
							slot = 2;
							choose = 1;
						}
					}
					if(mode == 0) {
						if(input != NULL){
							mark_button(load_dialog->children[3],"Graphic/save_full_c.png");
							load_board(board1,"2.xml",&game_mode,user_color,curr_color,&depth);
							choose = 1;
						}
					}
					break;
				}
				if(isClick(load_dialog->children[4], click_loc)&& choose == 0){
					/*is the save empty?*/
					input = fopen("3.xml","r");
					if(mode == 1){
						if(input == NULL){	
							mark_button(load_dialog->children[4],"Graphic/save_empty_c.png");
							slot = 3;
							choose = 1;
						}	
						else{
							mark_button(load_dialog->children[4],"Graphic/save_full_c.png");
							slot = 3;
							choose = 1;
						}
					}
					if(mode == 0) {
						if(input != NULL){
							mark_button(load_dialog->children[4],"Graphic/save_full_c.png");
							load_board(board1,"3.xml",&game_mode,user_color,curr_color,&depth);
							choose = 1;
						}
					}
					break;
				}
				if(isClick(load_dialog->children[5], click_loc) && choose == 0){
					/*is the save empty?*/
					input = fopen("4.xml","r");
					if(mode == 1){
						if(input == NULL){	
							mark_button(load_dialog->children[5],"Graphic/save_empty_c.png");
							slot = 4;
							choose = 1;
						}	
						else{
							mark_button(load_dialog->children[5],"Graphic/save_full_c.png");
							slot = 4;
							choose = 1;
						}
					}
					if(mode == 0) {
						if(input != NULL){
							mark_button(load_dialog->children[5],"Graphic/save_full_c.png");
							load_board(board1,"4.xml",&game_mode,user_color,curr_color,&depth);
							choose = 1;
						}
					}
					break;
				}
				
			}
		}
	}
	if(input != NULL)
		fclose(input);
	free_board(&board1);
	free_panel(load_dialog);
	return;
}


int new_game_options(char** board){
	/*Options for the new game - PVP, PVC or cancel*/
	int cancel = 0;
	Control *options, *options_back;
	Control *pvp, *pvc, *cancel_b;
	SDL_Rect options_pos, pvp_pos, pvc_pos, cancel_pos, recti;

	options_pos = create_rect(200, 140,400, 350);
	options = (Control*) malloc(sizeof(Control));
	if (options == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	options_back = (Control*) malloc(sizeof(Control));
	if (options_back == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	create_panel(options_pos,options);
	create_image(options_pos,"Graphic/new_game_options.png",options_back);
	
	/*adding the children to the options panel*/

	add_children_panel(options,options_back);

	pvp = (Control*) malloc(sizeof(Control));
	if (pvp == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	pvp_pos = create_rect(OPT_MENU_B_X,OPT_MENU_B_Y,OPT_MENU_B_W,OPT_MENU_B_H);
	create_button(pvp_pos,"Graphic/PVP1.png",pvp);

	pvc = (Control*) malloc(sizeof(Control));
	if (pvc == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	pvc_pos = create_rect(OPT_MENU_B_X,OPT_MENU_B_Y + OPT_BUTTON_SPACE,OPT_MENU_B_W,OPT_MENU_B_H);
	create_button(pvc_pos,"Graphic/PVC1.png",pvc);

	cancel_b = (Control*) malloc(sizeof(Control));
	if (cancel_b == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	cancel_pos = create_rect(OPT_MENU_B_X,OPT_MENU_B_Y + 2*OPT_BUTTON_SPACE,OPT_MENU_B_W,OPT_MENU_B_H);
	create_button(cancel_pos,"Graphic/cancel1.png",cancel_b);

	add_children_panel(options,pvp);
	add_children_panel(options,pvc);
	add_children_panel(options,cancel_b);

	draw_panel(options, screen);
	SDL_Flip(screen);

	while((cancel == 0) && (quit == 0)){
		while (SDL_PollEvent( &e) != 0){
			if(e.type == SDL_MOUSEBUTTONDOWN){
				recti = create_rect(e.button.x,e.button.y, 0,0);
				/*pvp?*/
				if( isClick(pvp, recti)){
					new_game(board, 0, WHITE,TWO_PLAYER,WHITE);
					cancel = 1;
				}
				/*pvc?*/
				if( isClick(pvc, recti)){
					choose_diff_color(board);
					cancel = 1;
				}
				/*cancel?*/
				if( isClick(cancel_b, recti)){
					cancel = 1;
				}
			}
		}
	}
	free_panel(options);
	return 1;
}

int choose_diff_color(char** board){
	/*PVC game chosen - now choose diff and color*/
	Control* dialog, *dialog_back, *diff_1, *diff_2, *diff_3, *diff_4, *diff_best, *col_black, *col_white, *ok, *Cancel;
	SDL_Rect dialog_pos, diff_1_pos, diff_2_pos, diff_3_pos, diff_4_pos, col_black_pos, col_white_pos, ok_pos, recti, diff_best_pos, Cancel_pos;
	int ok_ = 0, diff_choose = 0,diff_choose_c = 0, col_choose1 = 0;
	char col_choose[6] ;


	dialog_pos = create_rect(200, 140,400, 350);
	dialog = (Control*) malloc(sizeof(Control));
	dialog_back = (Control*) malloc(sizeof(Control));
	create_panel(dialog_pos,dialog);
	create_image(dialog_pos,"Graphic/new_game_dialog.png",dialog_back);

	add_children_panel(dialog,dialog_back);

	col_black = (Control*) malloc(sizeof(Control));
	if (col_black == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	col_black_pos = create_rect(OPT_MENU_B_X,OPT_MENU_B_Y + BUTTON_SPACE/5,COL_B_W,COL_B_H);
	create_button(col_black_pos,"Graphic/black.png",col_black);

	col_white = (Control*) malloc(sizeof(Control));
	if (col_white == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	col_white_pos = create_rect(OPT_MENU_B_X + 4*BUTTON_SPACE,OPT_MENU_B_Y + BUTTON_SPACE/5 -2,COL_B_W,COL_B_H);
	create_button(col_white_pos,"Graphic/white.png",col_white);

	diff_best = (Control*) malloc(sizeof(Control));
	if (diff_best == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	diff_best_pos = create_rect(dialog->rect.x+152.5,OPT_MENU_B_Y + 30 ,85,50);
	create_button(diff_best_pos,"Graphic/diff_best.png",diff_best);

	diff_1 = (Control*) malloc(sizeof(Control));
	if (diff_1 == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	diff_1_pos = create_rect(OPT_MENU_B_X,OPT_MENU_B_Y + 2.2*BUTTON_SPACE ,DIFF_B_W,DIFF_B_H);
	create_button(diff_1_pos,"Graphic/dif_1.png",diff_1);

	diff_2 = (Control*) malloc(sizeof(Control));
	if (diff_2 == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	diff_2_pos = create_rect(OPT_MENU_B_X + DIFF_B_SPACE ,OPT_MENU_B_Y + 2.2*BUTTON_SPACE ,DIFF_B_W,DIFF_B_H);
	create_button(diff_2_pos,"Graphic/dif_2.png",diff_2);

	diff_3 = (Control*) malloc(sizeof(Control));
	if (diff_3 == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	diff_3_pos = create_rect(OPT_MENU_B_X + 2*DIFF_B_SPACE ,OPT_MENU_B_Y + 2.2*BUTTON_SPACE ,DIFF_B_W,DIFF_B_H);
	create_button(diff_3_pos,"Graphic/dif_3.png",diff_3);

	diff_4 = (Control*) malloc(sizeof(Control));
	if (diff_4 == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	diff_4_pos = create_rect(OPT_MENU_B_X + 3*DIFF_B_SPACE ,OPT_MENU_B_Y + 2.2*BUTTON_SPACE ,DIFF_B_W,DIFF_B_H);
	create_button(diff_4_pos,"Graphic/dif_4.png",diff_4);

	ok = (Control*) malloc(sizeof(Control));
	if (ok == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	ok_pos = create_rect(dialog->rect.x+120 ,OPT_MENU_B_Y + 4*BUTTON_SPACE ,DIFF_B_W,DIFF_B_H);
	create_button(ok_pos,"Graphic/ok1.png",ok);

	Cancel = (Control*) malloc(sizeof(Control));
	if (Cancel == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	Cancel_pos = create_rect(dialog->rect.x+200 ,OPT_MENU_B_Y + 4*BUTTON_SPACE ,DIFF_B_W,DIFF_B_H);
	create_button(Cancel_pos,"Graphic/cancel_s1.png",Cancel);

	add_children_panel(dialog,col_white);
	add_children_panel(dialog,col_black);
	add_children_panel(dialog,diff_1);
	add_children_panel(dialog,diff_2);
	add_children_panel(dialog,diff_3);
	add_children_panel(dialog,diff_4);
	add_children_panel(dialog,diff_best);
	add_children_panel(dialog, Cancel);
	add_children_panel(dialog,ok);

	draw_panel(dialog, screen);
	SDL_Flip(screen);
	while ((ok_ == 0) && (quit == 0 )&& (cancel == 0)){
		while (SDL_PollEvent( &e) != 0){
				if(e.type == SDL_MOUSEBUTTONDOWN){
					diff_choose_c = 0; 
					recti = create_rect(e.button.x,e.button.y, 0,0);
					if(((recti.y > OPT_MENU_B_Y + 2.2*BUTTON_SPACE) || (isClick(diff_best,recti))) && (!isClick(ok,recti))){
						if(diff_choose_c == 0){
							if(diff_choose == 1){
								mark_button(diff_1,"Graphic/dif_1.png");
								diff_choose = 0;
							}
							if(diff_choose == 2){
								mark_button(diff_2,"Graphic/dif_2.png");
								diff_choose = 0;
							}
							if(diff_choose == 3){
								mark_button(diff_3,"Graphic/dif_3.png");
								diff_choose = 0;
							}
							if(diff_choose == 4){
								mark_button(diff_4,"Graphic/dif_4.png");
								diff_choose = 0;
							}
							if(diff_choose == 5){
								mark_button(diff_best,"Graphic/diff_best.png");
								diff_choose = 0;
							}
							if( isClick(diff_1, recti)){
								mark_button(diff_1,"Graphic/dif_1_c.png");
								diff_choose = 1;
								diff_choose_c = 1;
							}
							if( isClick(diff_2, recti)){
								mark_button(diff_2,"Graphic/dif_2_c.png");
								diff_choose = 2;
								diff_choose_c = 1;
							}
							if( isClick(diff_3, recti)){
								mark_button(diff_3,"Graphic/dif_3_c.png");
								diff_choose = 3;
								diff_choose_c = 1;
							}
							if( isClick(diff_4, recti)){
								mark_button(diff_4,"Graphic/dif_4_c.png");
								diff_choose = 4;
								diff_choose_c = 1;
							}
							if( isClick(diff_best, recti)){
								mark_button(diff_best,"Graphic/diff_best_c.png");
								diff_choose = 5;
								diff_choose_c = 1;
							}
						}
					}
					if(col_choose1 == 0){
						if( isClick(col_black, recti)){
							mark_button(col_black,"Graphic/Black_c.png");
							strcpy(col_choose,BLACK);
							col_choose1 = 1;
						}
						if( isClick(col_white, recti)){
							mark_button(col_white,"Graphic/White_c.png");
							strcpy(col_choose,WHITE);
							col_choose1 = 1;
						}
					}
					if((col_choose1 == 1) && (diff_choose != 0)){
						if(isClick(ok, recti)){
							new_game(board, diff_choose, col_choose, PLAYER_VS_COMP, WHITE);
							cancel = 1;
						}
					}
					if(isClick(Cancel, recti)){
						cancel = 1;
				}
			}
		}
	}
	free_panel(dialog);
	return 0;
}

int mark_button(Control* button, char *path){
	SDL_FreeSurface(button->surface);
	button->surface = load_image(path);
	draw_panel(button,screen);
	SDL_Flip(screen);
	return 1;
}

int new_game(char** board, int diff_choose, char * col_choose, int game_mode, char *starting_col){
	/*Main Game Area items*/
	Control *Board_back, *side_menu_back, *Board, *side_menu;
	/*Side menu buttons*/
	Control *restart, *save_game, *main_menu_b, *quit_b;
	/*rect for the side menu and board - rects==locations on the screen*/
	SDL_Rect side_menu_pos, Board_pos, restart_pos, save_game_pos, main_menu_b_pos,quit_b_pos;

	if(main_game_alloc == 1){
		free_panel(main_game_window);
		main_game_alloc = 0;
	}
	main_game_window = (Control*) malloc(sizeof(Control));
	if (main_game_window == NULL){
		printf("Error: standard function malloc has failed");
		exit(0);
	}
	main_game_alloc = 1;
	Board = (Control*) malloc(sizeof(Control));
	if (Board == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	Board_back = (Control*) malloc(sizeof(Control));
	if (Board_back == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	side_menu = (Control*) malloc(sizeof(Control));
	if (side_menu == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	side_menu_back = (Control*) malloc(sizeof(Control));
	if (side_menu_back == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}

	/*create main game window*/
	create_panel(main_offset, main_game_window);
	side_menu_pos=main_offset;
	side_menu_pos.w = (RIGHT_WIDTH);
	side_menu_pos.x = (RIGHT_MARGIN);
	Board_pos = main_offset;
	Board_pos.w = RIGHT_MARGIN;
	/*Create Game area panel and images*/
	create_panel(Board_pos, Board);
	/*Create Side menu Panel*/
	create_panel(side_menu_pos, side_menu);
	/*Load the graphics*/
	create_image(Board->rect,"Graphic/board2.png",Board_back);
	create_image(side_menu->rect,"Graphic/side_menu3.png", side_menu_back);

	/*add the Game and side menu as children of the main panel*/
	add_children_panel(main_game_window, Board);
	add_children_panel(main_game_window, side_menu);
	add_children_panel(Board, Board_back);
	add_children_panel(side_menu, side_menu_back);

	restart = (Control*) malloc(sizeof(Control));
	if (restart == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	restart_pos = create_rect(SIDE_MENU_X_BUTTONS,SIDE_MENU_Y_BUTTONS,SIDE_MENU_B_W,SIDE_MENU_B_H);
	create_button(restart_pos,"Graphic/restart_game1.png",restart);
	add_children_panel(side_menu,restart);

	save_game = (Control*) malloc(sizeof(Control));
	if (save_game == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	save_game_pos = create_rect(SIDE_MENU_X_BUTTONS,SIDE_MENU_Y_BUTTONS + BUTTON_SPACE,SIDE_MENU_B_W,SIDE_MENU_B_H);
	create_button(save_game_pos,"Graphic/save_game1.png",save_game);
	add_children_panel(side_menu,save_game);

	main_menu_b = (Control*) malloc(sizeof(Control));
	if (main_menu_b == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	main_menu_b_pos = create_rect(SIDE_MENU_X_BUTTONS,SIDE_MENU_Y_BUTTONS + 2*BUTTON_SPACE,SIDE_MENU_B_W,SIDE_MENU_B_H);
	create_button(main_menu_b_pos,"Graphic/main_menu_button1.png",main_menu_b);
	add_children_panel(side_menu,main_menu_b);

	quit_b = (Control*) malloc(sizeof(Control));
	if (quit_b == NULL){
			printf("Error: standard function malloc has failed");
			exit(0);
	}
	quit_b_pos = create_rect(SIDE_MENU_X_BUTTONS,SIDE_MENU_Y_BUTTONS + 7*BUTTON_SPACE,SIDE_MENU_B_W,SIDE_MENU_B_H);
	create_button(quit_b_pos,"Graphic/quit_game1.png",quit_b);
	add_children_panel(side_menu,quit_b);

	/*drawing in DFS - the father, than the first child, and his first and so on. the last is is drawn on top*/
	draw_panel(main_game_window, screen);

	SDL_Flip(screen);
	while( (quit == 0)&& (cancel == 0) ){
		if(game_mode == TWO_PLAYER)
			main_game_loop_pvp(board, col_choose,starting_col);
		if(game_mode == PLAYER_VS_COMP)
			main_game_loop_pvc(board, col_choose, starting_col, diff_choose);
	}
	
	return 0;
}
void draw_board(char** board){
	Control* curr , *Board = main_game_window->children[0];
	int i,j;

	while (Board->curr_size != 1){
		remove_last_child_panel(Board);
	}

	/********************/
	for (i = 0 ; i < BOARD_SIZE; i++){
		for(j = 0 ; j < BOARD_SIZE; j++) {
			if(board[i][j] == PAWN_W){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/white_pawn.png");
			}
			if(board[i][j] == PAWN_B){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/black_pawn.png");
			}
			if (board[i][j] == BISHOP_B){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/black_bishop.png");
			}
			if (board[i][j] == BISHOP_W){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/white_bishop.png");
			}
			if (board[i][j] == QUEEN_B){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/black_queen.png");
			}
			if (board[i][j] == QUEEN_W){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/white_queen.png");
			}
			if (board[i][j] == ROOK_B){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/black_rook.png");
			}
			if (board[i][j] == ROOK_W){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/white_rook.png");
			}
			if (board[i][j] == KNIGHT_B){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/black_knight.png");
			}
			if (board[i][j] == KNIGHT_W){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/white_knight.png");
			}
			if (board[i][j] == KING_B){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/black_king.png");
			}
			if (board[i][j] == KING_W){
				curr = (Control*) malloc(sizeof(Control));
				add_piece(curr, i,j,"Graphic/white_king.png");
			}
		}
	}
	
	/*print_board_(board);*/
	draw_panel(Board, screen);
	SDL_Flip(screen);
	return;

}

void add_piece(Control *button, int i, int j, char * path){
	SDL_Rect rect;
	rect = create_rect(x_off[i],y_off[j],52,62);
	create_button(rect,path,button);
	add_children_panel(main_game_window->children[0],button);

}


int check_where_clicked(Control *Board, SDL_Rect click_loc, loc *location){
	int i,j;

	
	for(i = 0; i < BOARD_SIZE; i++){
		for (j = 0 ; j < BOARD_SIZE; j++) {
			if( ( click_loc.x >  x_off[i] - 5 ) && ( click_loc.x < x_off[i] + DELTA_X - 3  ) && (click_loc.y > y_off[j] - 10 ) && ( click_loc.y <  y_off[j] + DELTA_Y -10)){
				/*the x and y are the actual indexes on the board*/
				location->x = i; location->y = j;
				return 1;

			}
		}
	}

	return 0;
}

void mark_location(SDL_Rect click_loc, char *path){
	Control *valid_loc, *Board = main_game_window->children[0] ;
	
	valid_loc = (Control*) malloc(sizeof(Control));
	create_image(click_loc,path,valid_loc);
	add_children_panel(Board, valid_loc);
	draw_panel(Board, screen);
	SDL_Flip(screen);

}


int main_game_loop_pvp(char** board, char * col_choose, char* starting_col){
	/*the loop where the full game will be run in. will use the console mode's functions in order to do it.*/
	Control* quit_b = main_game_window->children[1]->children[4],*restart = main_game_window->children[1]->children[1], *save_game = main_game_window->children[1]->children[2];
	Control* main_menu_b = main_game_window->children[1]->children[3], *Board = main_game_window->children[0], *marked_positions;
	Control * mark, *check;
	move *all_moves, *my_move;
	SDL_Rect click_loc;
	loc location;
	char ** original_board, **board1 ,curr_color[6];
	int marked = 0, num_moves = 0, i, get_loc_ret, user_move_ret = 0, promote_c = 0, depth = 1, mode = 1, game_mode = 1;
	int move_alloc = 0, all_moves_alloc = 0;
	copy_board(&original_board,board);
	copy_board(&board1, board);

	strcpy(curr_color,starting_col);
	draw_board(board1);
	SDL_Flip(screen);
	while((quit == 0) && (cancel == 0)){
		while (SDL_PollEvent( &e) != 0){
			if(e.type == SDL_MOUSEBUTTONDOWN){
				click_loc = create_rect(e.button.x, e.button.y,73,69 );
				if(click_loc.x > 644){	
					if(isClick(quit_b,click_loc)){
						quit = 1;
					}
					if(isClick(main_menu_b,click_loc)){
						cancel = 1;
						
					}
					if(isClick(restart,click_loc)){
						free_board(&board1);
						copy_board(&board1, original_board);
						draw_board(board1);
						strcpy(curr_color,starting_col);
					}
					if(isClick(save_game, click_loc)){
						load_and_save_game_gui(board1,mode, game_mode,curr_color,curr_color, depth);
						draw_panel(main_game_window,screen);
						SDL_Flip(screen);
					}
				}
				else {
					if(marked == 1){
						for(i = 1; i < marked_positions->curr_size; i++){
							if (isClick(marked_positions->children[i], click_loc))
								if(check_where_clicked(Board,click_loc, &location)){
									if(user_move_ret == CHECK_RET){
										draw_panel(main_game_window->children[1],screen);
										user_move_ret = 0;
									}
									my_move->to.x = location.x;
									my_move->to.y = location.y;
									my_move->promotion = EOL;
									if((board1[my_move->from.x][my_move->from.y] == PAWN_B || board1[my_move->from.x][my_move->from.y] == PAWN_W)&& (location.y == 7 || location.y == 0))
										promote_c = 1;
									user_move_ret = user_move(board1,curr_color,my_move);
									/*promotion!*/
									if(promote_c == 1){
										promote(board1, curr_color, location.x, location.y);
										promote_c = 0;
										draw_board(board1);
										SDL_Flip(screen);
									}
									draw_board(board1);
									SDL_Flip(screen);
									if(user_move_ret == CHECK_RET){
										check = (Control*) malloc(sizeof(Control));
										create_image(create_rect(main_menu_b->rect.x - 20,main_menu_b->rect.y+50,0,0),"Graphic/check.png",check);
										add_children_panel(Board,check);
										draw_panel(Board,screen);
									}
									if(user_move_ret == MATE_RET){
										win_gui(curr_color);
										if(move_alloc == 1){
											free(my_move);
											move_alloc = 0;
										}
										quit = 1;
									}
									if(move_alloc == 1){
										free(my_move);
										move_alloc = 0;
									}
									strcpy(curr_color,opponent_color(curr_color));
									break;
								}
						}
						remove_last_child_panel(Board);
						draw_board(board1);
						SDL_Flip(screen);
						marked = 0;
						if(move_alloc == 1){
							free(my_move);
							move_alloc = 0;
						}
						
					}
					if(check_where_clicked (Board, click_loc, &location)){
						if(move_alloc == 1){
							free(my_move);
							move_alloc = 0;
						}
						my_move = (move*) malloc (sizeof(move));
						move_alloc = 1;
						my_move->from.x = location.x;
						my_move->from.y = location.y;
						if(all_moves_alloc == 1){
							free(all_moves);
							all_moves_alloc = 0;
						}
						all_moves = (move*)malloc(sizeof(move));
						all_moves_alloc = 1;
						if(all_moves == NULL){
							printf("Error: standard function malloc has failed");
							end_prog();
							exit (0);
						}
						/*printf("%d,%d, %c", location.x, location.y, board[location.x][location.y]);*/
						get_loc_ret = get_location_moves(board1, location, &all_moves,&num_moves,curr_color, 1);
						if (get_loc_ret == 1){
							mark = (Control*) malloc(sizeof(Control));
								if (mark == NULL){
									printf("Error: standard function malloc has failed");
									exit(0);
							}
							create_image(create_rect(x_off[location.x],y_off[location.y],DELTA_X,DELTA_Y),"Graphic/marked3.png",mark);
							marked_positions = (Control*) malloc(sizeof(Control));
							if (marked_positions == NULL){
									printf("Error: standard function malloc has failed");
									exit(0);
								}
							create_panel(Board->rect,marked_positions);
							add_children_panel(Board,marked_positions);
							add_children_panel(marked_positions,mark);
							
							for(i = 0; i < num_moves; i++){
								mark = (Control*) malloc(sizeof(Control));
								if (mark == NULL){
									printf("Error: standard function malloc has failed");
									exit(0);
								}
								create_image(create_rect(x_off[all_moves[i].to.x],y_off[all_moves[i].to.y],DELTA_X,DELTA_Y ),"Graphic/marked.png", mark);
								add_children_panel(marked_positions,mark);
							}
							num_moves = 0;
							draw_panel(Board,screen);
							SDL_Flip(screen);
							marked = 1;

						}
					
						if(all_moves_alloc == 1){
							free(all_moves);
							all_moves_alloc = 0;
						}
					}
				}
			}
		}
	}
	if(main_game_alloc == 1){
		free_panel(main_game_window);
		main_game_alloc = 0;
	}
	if(move_alloc == 1){
		free(my_move);
		move_alloc = 0;
	}
	if(all_moves_alloc == 1){
		free(all_moves);
		all_moves_alloc = 0;
	}
	free_board(&original_board);
	free_board(&board1);

	return 0;
}


int main_game_loop_pvc(char **board, char* user_color, char *starting_col, int depth){
	Control* quit_b = main_game_window->children[1]->children[4],*restart = main_game_window->children[1]->children[1], *save_game = main_game_window->children[1]->children[2];
	Control* main_menu_b = main_game_window->children[1]->children[3], *Board = main_game_window->children[0], *marked_positions;
	Control * mark, *check;
	move *all_moves, *my_move;
	SDL_Rect click_loc;
	loc location;
	char** original_board,**board1, curr_color[6], move_un[MAX_CMD_LENGTH];
	int marked = 0, num_moves = 0, i, get_loc_ret, user_move_ret = 0, pc_move_rt = 0, promote_c = 0, mode = 1, game_mode = 2;
	int move_alloc = 0, all_move_alloc = 0;

	copy_board(&original_board,board);
	copy_board(&board1,board);
	strcpy(curr_color, starting_col);
	draw_board(board1);
	SDL_Flip(screen);
	while((quit == 0) && (cancel == 0)){
		while (SDL_PollEvent( &e) != 0){
			if(strcmp(user_color,curr_color) != 0){
				pc_move_rt = computer_turn(board1, opponent_color(user_color), depth, move_un);
				if(pc_move_rt == CHECK_RET){
					check = (Control*) malloc(sizeof(Control));
					create_image(create_rect(main_menu_b->rect.x - 20,main_menu_b->rect.y+50,0,0),"Graphic/check.png",check);
					add_children_panel(Board,check);
					draw_panel(Board,screen);
				}
				draw_board(board1);
				if(pc_move_rt == MATE_RET){
					win_gui(curr_color);
					quit = 1;
					break;
				}
				draw_board(board1);
				strcpy(curr_color, opponent_color(curr_color));
			}
			if(e.type == SDL_MOUSEBUTTONDOWN){
				click_loc = create_rect(e.button.x, e.button.y,73,69 );
				if(click_loc.x > 644){	
					if(isClick(quit_b,click_loc)){
						quit = 1;
						break;
					}
					if(isClick(main_menu_b,click_loc)){
						cancel = 1;
						break;
					}
					if(isClick(restart,click_loc)){
						free_board(&board1);
						copy_board(&board1, original_board);
						draw_board(board1);
						strcpy(curr_color, starting_col);
					}				
					if(isClick(save_game,click_loc)){
						load_and_save_game_gui(board1, mode,game_mode,user_color,curr_color,depth);
						draw_panel(main_game_window,screen);
						SDL_Flip(screen);
					}
				}
				/*click on the Board*/
				else{
					if(strcmp(user_color, curr_color) == 0){
						if(marked == 1){
						for(i = 1; i < marked_positions->curr_size; i++){
							if (isClick(marked_positions->children[i], click_loc)){
								if(check_where_clicked(Board,click_loc, &location)){
									draw_panel(main_game_window->children[1],screen);
									user_move_ret = 0;
									my_move->to.x = location.x;
									my_move->to.y = location.y;
									my_move->promotion = EOL;
									if((board1[my_move->from.x][my_move->from.y] == PAWN_B || board1[my_move->from.x][my_move->from.y] == PAWN_W)&& (location.y == 7 || location.y == 0))
										promote_c = 1;
									user_move_ret = user_move(board1,curr_color,my_move);
									if(promote_c == 1){
										promote(board1, curr_color, location.x, location.y);
										promote_c = 0;
										draw_board(board1);
										SDL_Flip(screen);
									}
									if(user_move_ret == CHECK_RET){
										check = (Control*) malloc(sizeof(Control));
										if (check == NULL){
											printf("Error: standard function malloc has failed");
											exit(0);
										}
										create_image(create_rect(main_menu_b->rect.x - 20,main_menu_b->rect.y+50,0,0),"Graphic/check.png",check);
										add_children_panel(Board,check);
										draw_panel(Board,screen);
									}
									draw_board(board1);
									SDL_Flip(screen);
									if(user_move_ret == MATE_RET){
										if(move_alloc == 1){
											free(my_move);
											move_alloc = 0;
										}
										win_gui(curr_color);
										quit = 1;
										break;
										
									}
									if(move_alloc == 1){
										free(my_move);
										move_alloc = 0;
									}
									strcpy(curr_color,opponent_color(curr_color));
									break;
								}
							}
						}
						remove_last_child_panel(Board);
						draw_board(board1);
						SDL_Flip(screen);
						marked = 0;			
						if(move_alloc == 1){
							free(my_move);
							move_alloc = 0;
						}
					}
					if(check_where_clicked (Board, click_loc, &location)){
						if(move_alloc == 1){
							free(my_move);
							move_alloc = 0;
						}
						my_move = (move*) malloc (sizeof(move));
						if (my_move == NULL){
							printf("Error: standard function malloc has failed");
							exit(0);
						}
						move_alloc = 1;
						my_move->from.x = location.x;
						my_move->from.y = location.y;
						if(all_move_alloc == 1){
							free(all_moves);
							all_move_alloc = 0;
						}
						all_moves = (move*)malloc(sizeof(move));
						all_move_alloc = 1;
						if(all_moves == NULL){
							printf("Error: standard function malloc has failed");
							end_prog();
							exit (0);
						}
						/*printf("%d,%d, %c", location.x, location.y, board[location.x][location.y]);*/
						get_loc_ret = get_location_moves(board1, location, &all_moves,&num_moves,curr_color, 1);
						if (get_loc_ret == 1){
							mark = (Control*) malloc(sizeof(Control));
							create_image(create_rect(x_off[location.x],y_off[location.y],DELTA_X,DELTA_Y),"Graphic/marked3.png",mark);
							marked_positions = (Control*) malloc(sizeof(Control));
							create_panel(Board->rect,marked_positions);
							add_children_panel(Board,marked_positions);
							add_children_panel(marked_positions,mark);
							
							for(i = 0; i < num_moves; i++){
								mark = (Control*) malloc(sizeof(Control));
								create_image(create_rect(x_off[all_moves[i].to.x],y_off[all_moves[i].to.y],DELTA_X,DELTA_Y),"Graphic/marked.png", mark);
								add_children_panel(marked_positions,mark);
							}
							num_moves = 0;
							draw_panel(Board,screen);
							SDL_Flip(screen);
							marked = 1;

						}
					
						if(all_move_alloc == 1){
							free(all_moves);
							all_move_alloc = 0;
						}
					}

					}

				}
			}
		}
	}
	if(main_game_alloc == 1){
		free_panel(main_game_window);
		main_game_alloc = 0;
	}
	if(move_alloc == 1){
		free(my_move);
		move_alloc = 0;
	}
	if(all_move_alloc == 1){
		free(all_moves);
		all_move_alloc = 0;
	}
	free_board(&original_board);
	free_board(&board1);
	return 0;
}

void win_gui(char *curr_color){
	Control* dialog, *dialog_back, *ok;
	SDL_Rect dialog_pos, click_loc;
	int close = 0;

	dialog_pos = create_rect(200, 140,400, 350);
	dialog = (Control*) malloc(sizeof(Control));
	dialog_back = (Control*) malloc(sizeof(Control));
	ok = (Control*) malloc(sizeof(Control));
	
	create_panel(dialog_pos,dialog);
	create_button(create_rect(dialog->rect.x+162.5 ,OPT_MENU_B_Y + 4*BUTTON_SPACE ,DIFF_B_W,DIFF_B_H),"Graphic/ok1.png", ok);
	if( strcmp(curr_color,WHITE) == 0){
		create_image(dialog_pos,"Graphic/white_win.png",dialog_back);
	}
	else{
		create_image(dialog_pos,"Graphic/black_win.png",dialog_back);
	}
	add_children_panel(dialog,dialog_back);
	add_children_panel(dialog,ok);
	add_children_panel(main_game_window->children[0],dialog);
	draw_panel(dialog,screen);
	SDL_Flip(screen);
	while(close == 0){
		while (SDL_PollEvent(&e) != 0){
			if(e.type == SDL_MOUSEBUTTONDOWN){
				click_loc = create_rect(e.button.x,e.button.y,0,0);
				if(isClick(ok,click_loc)){
					close = 1;
					remove_last_child_panel(main_game_window->children[0]);
					return;
				}
			}
		}
	}
}

void promote (char ** board ,char* curr_color, int x, int y){
	Control *prom_dialog ,*back, *queen, *bishop, *rook, *knight, *ok;
	SDL_Rect click_loc;
	int close = 0, choose = 0;

	prom_dialog = (Control*) malloc(sizeof(Control));
	create_panel(create_rect(200, 140,400, 350), prom_dialog);

	back = (Control*) malloc(sizeof(Control));
	create_image(prom_dialog->rect,"Graphic/promote.png", back);
	add_children_panel(prom_dialog, back);

	ok = (Control*) malloc(sizeof(Control));
	create_button(create_rect(prom_dialog->rect.x+162.5 ,OPT_MENU_B_Y + 4*BUTTON_SPACE ,DIFF_B_W,DIFF_B_H),"Graphic/ok1.png", ok);
	add_children_panel(prom_dialog,ok);

	queen = (Control*) malloc(sizeof(Control));
	bishop = (Control*) malloc(sizeof(Control));
	knight = (Control*) malloc(sizeof(Control));
	rook = (Control*) malloc(sizeof(Control));
	if( strcmp(curr_color,WHITE) == 0) {
		create_image(create_rect(OPT_MENU_B_X,OPT_MENU_B_Y + BUTTON_SPACE,COL_B_W,COL_B_H),"Graphic/white_bishop.png",bishop);
		create_image(create_rect(OPT_MENU_B_X,OPT_MENU_B_Y + BUTTON_SPACE + COL_B_H,COL_B_W,COL_B_H),"Graphic/white_queen.png",queen);
		create_image(create_rect(OPT_MENU_B_X + 4*BUTTON_SPACE,OPT_MENU_B_Y + BUTTON_SPACE -2,COL_B_W,COL_B_H),"Graphic/white_rook.png", rook);
		create_image(create_rect(OPT_MENU_B_X + 4*BUTTON_SPACE,OPT_MENU_B_Y + BUTTON_SPACE +COL_B_H -2,COL_B_W,COL_B_H),"Graphic/white_knight.png",knight);

	}

	else {
		create_image(create_rect(OPT_MENU_B_X,OPT_MENU_B_Y + BUTTON_SPACE,COL_B_W,COL_B_H),"Graphic/black_bishop.png",bishop);
		create_image(create_rect(OPT_MENU_B_X,OPT_MENU_B_Y + BUTTON_SPACE + COL_B_H,COL_B_W,COL_B_H),"Graphic/black_queen.png",queen);
		create_image(create_rect(OPT_MENU_B_X + 4*BUTTON_SPACE,OPT_MENU_B_Y + BUTTON_SPACE -2,COL_B_W,COL_B_H),"Graphic/black_rook.png", rook);
		create_image(create_rect(OPT_MENU_B_X + 4*BUTTON_SPACE,OPT_MENU_B_Y + BUTTON_SPACE +COL_B_H -2,COL_B_W,COL_B_H),"Graphic/black_knight.png",knight);


	}
	add_children_panel(prom_dialog,queen);
	add_children_panel(prom_dialog,bishop);
	add_children_panel(prom_dialog,rook);
	add_children_panel(prom_dialog, knight);
	draw_panel(prom_dialog,screen);
	SDL_Flip(screen);
	while(close == 0){
		while (SDL_PollEvent(&e) != 0){
			if(e.type == SDL_MOUSEBUTTONDOWN){
				click_loc = create_rect(e.button.x,e.button.y,0,0);
				if(isClick(bishop,click_loc) && choose == 0){
					if(strcmp(curr_color,WHITE) == 0){
						mark_button(bishop,"Graphic/white_bishop_marked.png");
						board[x][y] = BISHOP_W;
					}
					else{
						mark_button(bishop,"Graphic/black_bishop_marked.png");
						board[x][y] = BISHOP_B;
					}
					choose = 1;
				}
				if(isClick(queen,click_loc)  && choose == 0){
					if(strcmp(curr_color,WHITE) == 0){
						mark_button(queen,"Graphic/white_queen_marked.png");
						board[x][y] = QUEEN_W;
					}
					else{
						mark_button(queen,"Graphic/black_queen_marked.png");
						board[x][y] = QUEEN_B;
					}
					choose = 1;
				}
				if(isClick(rook,click_loc)  && choose == 0){
					if(strcmp(curr_color,WHITE) == 0){
						mark_button(rook,"Graphic/white_rook_marked.png");
						board[x][y] = ROOK_W;
					}
					else{
						mark_button(rook,"Graphic/black_rook_marked.png");
						board[x][y] = ROOK_B;
					}
					choose = 1;
				}
				if(isClick(knight,click_loc)  && choose == 0){
					if(strcmp(curr_color,WHITE) == 0){
						mark_button(knight,"Graphic/white_knight_marked.png");
						board[x][y] = KNIGHT_W;
					}
					else{
						mark_button(knight,"Graphic/black_knight_marked.png");
						board[x][y] = KNIGHT_B;
					}
					
					choose = 1;
				}

				if(isClick(ok,click_loc) && (choose == 1)){
					close = 1;
				}
			}
		}
	}

	free_panel(prom_dialog);
	return;

}