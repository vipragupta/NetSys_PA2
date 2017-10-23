
CC=gcc
CFLAGS= 


SRC= webserver.c
 

all: $(SRC)
	$(CC) -o webserver.o $(SRC)

.PHONY : clean
clean :
	-rm -f *.o 
