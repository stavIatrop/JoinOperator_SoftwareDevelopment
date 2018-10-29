CC = gcc
SOURCE = src/indexSearchTesting.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c 

NAME_OF_EXECUTABLE = indexSearchIndexTest

OBJECT = $(SOURCE:.c=.o)

VALGRIND_FLAGS = --leak-check=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes -v

all: executable
	@echo Compile finished

executable: $(SOURCE) $(NAME_OF_EXECUTABLE)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_EXECUTABLE) $(SOURCE) -lm -lcunit
 
$(NAME_OF_EXECUTABLE): $(OBJECT)
	$(CC) -g  -O0 $(OBJECT) -o $@ -lm -lcunit


.c.o:
	$(CC) -c $< -o $@ -lm -lcunit


runTest: executable
	./$(NAME_OF_EXECUTABLE)

runValgrind: executable
	valgrind $(VALGRIND_FLAGS) ./$(NAME_OF_EXECUTABLE)

cacheMisses: executable
	perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./$(NAME_OF_EXECUTABLE)

clean:
	rm -f $(NAME_OF_EXECUTABLE)
	rm -f src/*.o

