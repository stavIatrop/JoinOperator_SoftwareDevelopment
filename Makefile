CC = gcc
SOURCE = src/hello.c

NAME_OF_EXECUTABLE = main

OBJECT = $(SOURCE:.c=.o)

VALGRIND_FLAGS = --leak-check=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes -v

all: executable
	@echo Compile finished

executable: $(SOURCE) $(NAME_OF_EXECUTABLE)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_EXECUTABLE) $(SOURCE) -lm
 
$(NAME_OF_EXECUTABLE): $(OBJECT)
	$(CC) -g  -O0 $(OBJECT) -o $@ -lm

.c.o:
	$(CC) -c $< -o $@ -lm 


clean:
	rm -f $(NAME_OF_EXECUTABLE)
	rm -f src/*.o

