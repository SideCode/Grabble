OBJECTS = main.o cleanup.o input.o thought.o
NAME = grabble
CFLAGS = -g -c -Wall
CSDLFLAGS = `sdl-config --cflags` 
all: grabble
	
$(NAME): $(OBJECTS)
	gcc -g -Wall `sdl-config --libs` $(OBJECTS) -o $(NAME) 
		
main.o: main.c
	gcc $(CFLAGS) $(CSDLFLAGS) main.c
	
cleanup.o: cleanup.c
	gcc $(CFLAGS) $(CSDLFLAGS) cleanup.c

input.o: input.c
	gcc $(CFLAGS) $(CSDLFLAGS) input.c
	
thought.o: thought.c
	gcc $(CFLAGS) thought.c
	
clean:
	rm -rf *o $(NAME)
