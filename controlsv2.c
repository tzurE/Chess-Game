#include "controlsv2.h"


void create_panel(SDL_Rect rect, Control* panel){
	panel->rect.h=rect.h;
	panel->rect.w=rect.w;
	panel->rect.x=rect.x;
	panel->rect.y=rect.y;
	panel->surface = NULL;
	/*no children yet*/
	panel->curr_size = 0;
	panel->flag = 0; 
	/*identifes what type of controler this is*/
	strcpy(panel->label, "panel"); 

}

void create_image (SDL_Rect rect, char* image_path, Control *image){

	/*the image is not a panel and therefor won't have any children*/
	image->flag = 0;
	image->surface = load_image(image_path);
	image->rect.h=rect.h;
	image->rect.w=rect.w;
	image->rect.x=rect.x;
	image->rect.y=rect.y;
	image->curr_size = 0;
	/*identifes what type of controler this is*/
	strcpy(image->label,"image");

	}

void create_button(SDL_Rect rect, char *image_path, Control *button){

		/*the button is not a panel and therefor won't have any children*/
	button->flag = 0;
	button->surface = load_image(image_path);
	button->rect.h=rect.h;
	button->rect.w=rect.w;
	button->rect.x=rect.x;
	button->rect.y=rect.y;
	button->curr_size = 0;
	/*identifes what type of controler this is*/
	strcpy(button->label,"button");
	
}

void add_children_panel(Control* panel, Control* child){

	if(panel->curr_size == 0){
		panel->children[0] = child;
		panel->curr_size = 1;
		return;
	}
	panel->children[panel->curr_size] = NULL;
	panel->children[panel->curr_size] = child;
	panel->curr_size = panel->curr_size + 1;
}

void remove_last_child_panel(Control *panel){
	int i = panel->curr_size-1;
	free_panel(panel->children[i]);
	panel->curr_size = i;
}

void draw_panel(Control* panel, SDL_Surface* screen){
	int i = 0;
	if((strcmp(panel->label, "image")== 0) || (strcmp(panel->label, "button")== 0)){	
		apply_surface(panel->rect,panel->surface, screen);
		
	}
	/*drawing in DFS - the father, than the first child, and his first and so on. the last is drawn on top*/
	while (i < (panel->curr_size)){
		/*printf("%d", i);*/
		draw_panel(panel->children[i], screen);
		i++;
		
	 }
}

void apply_surface(SDL_Rect offset,SDL_Surface* source, SDL_Surface* destination){

	SDL_BlitSurface( source, NULL, destination, &offset );
}
void free_panel(Control* panel){
	/*free the panel and all the lists */
	int i = 0;
	while(i < panel->curr_size){	
		free_panel(panel->children[i]);
		i++;
	}
	SDL_FreeSurface(panel->surface);
	free(panel);
	
}

													/********Image loading and optimizing general function*******/
SDL_Surface *load_image( char* filename ) {
	/*Temporary storage for the image that's loaded */
	SDL_Surface* loadedImage = NULL; 
	/*The optimized image that will be used */
	SDL_Surface* optimizedImage = NULL;

	loadedImage = IMG_Load(filename);
	if (loadedImage != NULL){
		optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
		SDL_FreeSurface(loadedImage);
	}

	return optimizedImage;
}

SDL_Rect create_rect(int x, int y, int w, int h){
	SDL_Rect rect;
	rect.h = h;
	rect.w = w;
	rect.x = x;
	rect.y = y;
	return rect;

}

int isClick(Control* button, SDL_Rect rect){
	if( ( rect.x > button->rect.x ) && ( rect.x < button->rect.x + button->rect.w ) && (rect.y > button->rect.y ) && ( rect.y < button->rect.y + button->rect.h )){
		return 1;
	}
	return 0;
}
