CC = gcc

SOURCE = src/mainTest.c src/inputManip.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c
SOURCE_UT = src/indexSearchTesting.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c
SOURCE_MAIN = src/main.c src/inputManip.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c
SOURCE_INPUT_UT = src/inputTesting.c src/inputManip.c src/resultListManip.c

NAME_OF_EXECUTABLE = mainUt
NAME_OF_UT = unitTest
NAME_OF_MAIN = main
NAME_OF_INPUT_UT = inputUt

OBJECT = $(SOURCE:.c=.o)
OBJECT_UT = $(SOURCE_UT:.c=.o)
OBJECT_MAIN = $(SOURCE_MAIN:.c=.o)
OBJECT_INPUT_UT = $(SOURCE_INPUT_UT:.c=.o)

VALGRIND_FLAGS = --leak-check=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes -v

all: executable ut mainTarget inputUtTarget
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

inputUtTarget: $(SOURCE_INPUT_UT) $(NAME_OF_INPUT_UT)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_INPUT_UT) $(SOURCE_INPUT_UT) -lm -lcunit
	@echo Compiled Input Unit Test
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

$(NAME_OF_INPUT_UT): $(OBJECT_INPUT_UT)
	$(CC) -g  -O0 $(OBJECT_INPUT_UT) -o $@ -lm -lcunit

.c.o:
	$(CC) -c $< -o $@ -lm -lcunit


runMainUt: executable
	./$(NAME_OF_EXECUTABLE)

runUt: ut
	./$(NAME_OF_UT)

runInputUt:
	./$(NAME_OF_INPUT_UT)

runMain:
	./$(NAME_OF_MAIN) -R testTables/file3_10 -S testTables/file3_10 -r 1 -s 2 -t binary -o testTables/outFile

runValgrind:
	valgrind $(VALGRIND_FLAGS) ./$(NAME_OF_MAIN) -R testTables/file3_10 -S testTables/file3_10 -r 1 -s 2 -t binary -o testTables/outFile

cacheMisses: executable
	perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./$(NAME_OF_EXECUTABLE)

clean:
	rm -f $(NAME_OF_EXECUTABLE)
	rm -f $(NAME_OF_UT)
	rm -f $(NAME_OF_MAIN)
	rm -f $(NAME_OF_INPUT_UT)
	rm -f src/*.o

