all : Game

Game : algo ball main
	g++  -o Game algo.o ball.o main.o -s -lSDL ./libchipmunk.a -lSDL_gfx -lSDL_ttf -lglib-2.0

main : main.c
	gcc -Wall -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include  -lglib-2.0 -O2 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0 -c main.c -o main.o

ball : ball.h ball.c
	gcc -Wall -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include  -lglib-2.0 -O2 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0 -c ball.c -o ball.o

algo : algo.h algo.c
	gcc -Wall -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include  -lglib-2.0 -O2 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0 -c algo.c -o algo.o

clean :
	rm *.o Game
