# chessprog makefile

all: chessprog

chessprog: Chess.o Settings.o minimax.o GetMoves.o controlsv2.o gui.o 
	gcc -o chessprog Chess.o Settings.o  minimax.o GetMoves.o controlsv2.o gui.o -ansi -pedantic-errors `sdl-config --libs` -g -lm -L/usr/local/lib/SDL_image-1.2.12/lib -lSDL_image

Chess.o: Chess.c
	gcc -ansi -pedantic-errors -c -Wall -g Chess.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL

controlsv2.o: controlsv2.c
	gcc -ansi -pedantic-errors -c -Wall -g controlsv2.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL

Settings.o: Settings.c
	gcc -ansi -pedantic-errors -c -Wall -g Settings.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL

minimax.o: minimax.c
	gcc -ansi -pedantic-errors -c -Wall -g minimax.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL

GetMoves.o: GetMoves.c
	gcc -ansi -pedantic-errors -c -Wall -g GetMoves.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL
	
gui.o: gui.c
	gcc -ansi -pedantic-errors -c -Wall -g gui.c -lm `sdl-config --cflags` -I/usr/local/lib/SDL_image-1.2.12/include/SDL	

clean:
	-rm *.o chessprog