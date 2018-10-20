CC = gcc
SOURCE = src/webServerMain.c

NAME_OF_EXECUTABLE = main

OBJECT = $(SOURCE:.c=.o)

VALGRIND_FLAGS = --leak-check=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes -v

all: server compTests crawler
	@echo Compile finished

#Webserver compile/running
server: $(SOURCE) $(NAME_OF_EXECUTABLE)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_EXECUTABLE) $(SOURCE_WEB_SERVER) -lm
 
$(NAME_OF_EXECUTABLE): $(OBJECT)
	$(CC) -g  -O0 $(OBJECT) -o $@ -lm -pthread

.c.o:
	$(CC) -c $< -o $@ -lm 


clean: cleanSavedir
	rm -f $(NAME_OF_EXECUTABLE)
	rm -f $(NAME_OF_EXECUTABLE_CRAWLER)
	rm -f src/*.o

