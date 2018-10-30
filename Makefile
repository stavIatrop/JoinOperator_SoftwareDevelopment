CC = gcc

SOURCE = src/mainTest.c src/inputManip.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c
SOURCE_UT = src/indexSearchTesting.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c
SOURCE_MAIN = src/main.c src/inputManip.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c

NAME_OF_EXECUTABLE = mainUt
NAME_OF_UT = unitTest
NAME_OF_MAIN = main

OBJECT = $(SOURCE:.c=.o)
OBJECT_UT = $(SOURCE_UT:.c=.o)
OBJECT_MAIN = $(SOURCE_MAIN:.c=.o)

VALGRIND_FLAGS = --leak-check=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes -v

all: executable ut mainTarget
	@echo  
	@echo Compile finished

executable: $(SOURCE) $(NAME_OF_EXECUTABLE)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_EXECUTABLE) $(SOURCE) -lm -lcunit
	@echo Compiled Main Test
	@echo 

ut: $(SOURCE_UT) $(NAME_OF_UT)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_UT) $(SOURCE_UT) -lm -lcunit
	@echo Compiled Mini Unit Tests
	@echo 

mainTarget: $(SOURCE_MAIN) $(NAME_OF_MAIN)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_MAIN) $(SOURCE_MAIN) -lm -lcunit
	@echo Compiled Main
	@echo 

$(NAME_OF_EXECUTABLE): $(OBJECT)
	$(CC) -g  -O0 $(OBJECT) -o $@ -lm -lcunit
	
$(NAME_OF_UT): $(OBJECT_UT)
	$(CC) -g  -O0 $(OBJECT_UT) -o $@ -lm -lcunit

$(NAME_OF_MAIN): $(OBJECT_MAIN)
	$(CC) -g  -O0 $(OBJECT_MAIN) -o $@ -lm -lcunit

.c.o:
	$(CC) -c $< -o $@ -lm -lcunit


runMainUt: executable
	./$(NAME_OF_EXECUTABLE)

runUt: ut
	./$(NAME_OF_UT)

runMain:
	./$(NAME_OF_MAIN)

runValgrind: executable
	valgrind $(VALGRIND_FLAGS) ./$(NAME_OF_EXECUTABLE)

cacheMisses: executable
	perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./$(NAME_OF_EXECUTABLE)

clean:
	rm -f $(NAME_OF_EXECUTABLE)
	rm -f $(NAME_OF_UT)
	rm -f $(NAME_OF_MAIN)
	rm -f src/*.o

