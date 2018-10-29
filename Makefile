CC = gcc

SOURCE = src/mainTest.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c
SOURCE_UT = src/indexSearchTesting.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c

NAME_OF_EXECUTABLE = mainUt
NAME_OF_UT = unitTest

OBJECT = $(SOURCE:.c=.o)
OBJECT_UT = $(SOURCE_UT:.c=.o)

VALGRIND_FLAGS = --leak-check=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes -v

all: executable ut
	@echo  
	@echo Compile finished

executable: $(SOURCE) $(NAME_OF_EXECUTABLE)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_EXECUTABLE) $(SOURCE) -lm -lcunit

ut: $(SOURCE_UT) $(NAME_OF_UT)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_UT) $(SOURCE_UT) -lm -lcunit
	@echo Compiled Mini Unit Tests
	@echo 

$(NAME_OF_EXECUTABLE): $(OBJECT)
	$(CC) -g  -O0 $(OBJECT) -o $@ -lm -lcunit
	@echo Compiled Main Test
	@echo 

$(NAME_OF_UT): $(OBJECT_UT)
	$(CC) -g  -O0 $(OBJECT_UT) -o $@ -lm -lcunit


.c.o:
	$(CC) -c $< -o $@ -lm -lcunit


runMainUt: executable
	./$(NAME_OF_EXECUTABLE)

runUt: ut
	./$(NAME_OF_UT)

runValgrind: executable
	valgrind $(VALGRIND_FLAGS) ./$(NAME_OF_EXECUTABLE)

cacheMisses: executable
	perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./$(NAME_OF_EXECUTABLE)

clean:
	rm -f $(NAME_OF_EXECUTABLE)
	rm -f src/*.o

