CC = gcc

SOURCE = src/mainTest.c src/inputManip.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c
SOURCE_SEARCH_LIST_UT = src/searchListTesting.c src/resultListManip.c src/hashing.c src/chainFollower.c
SOURCE_MAIN = src/main.c src/radixHashJoin.c src/inputManip.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c
SOURCE_INPUT_UT = src/inputTesting.c src/inputManip.c src/resultListManip.c
SOURCE_REORDERING_UT = src/viceTests.c src/functions.c

NAME_OF_EXECUTABLE = mainUt
NAME_OF_SEARCH_LIST_UT = searchListUnitTest
NAME_OF_MAIN = main
NAME_OF_INPUT_UT = inputUt
NAME_OF_REORDERING_UT = reorderingTest

OBJECT = $(SOURCE:.c=.o)
OBJECT_SEARCH_LIST_UT = $(SOURCE_SEARCH_LIST_UT:.c=.o)
OBJECT_MAIN = $(SOURCE_MAIN:.c=.o)
OBJECT_INPUT_UT = $(SOURCE_INPUT_UT:.c=.o)
OBJECT_REORDERING_UT = $(SOURCE_REORDERING_UT:.c=.o)


VALGRIND_FLAGS = --leak-check=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes -v

all: mainTarget inputUtTarget searchListUt reorderingUtTarget
	@echo  
	@echo Compile finished

executable: $(SOURCE) $(NAME_OF_EXECUTABLE)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_EXECUTABLE) $(SOURCE) -lm -lcunit
	@echo Compiled Main Test
	@echo 

searchListUt: $(SOURCE_SEARCH_LIST_UT) $(NAME_OF_SEARCH_LIST_UT)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_SEARCH_LIST_UT) $(SOURCE_SEARCH_LIST_UT) -lm -lcunit
	@echo Compiled Search and List Unit tests
	@echo 

inputUtTarget: $(SOURCE_INPUT_UT) $(NAME_OF_INPUT_UT)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_INPUT_UT) $(SOURCE_INPUT_UT) -lm -lcunit
	@echo Compiled Input Unit Test
	@echo 

reorderingUtTarget: $(SOURCE_REORDERING_UT) $(NAME_OF_REORDERING_UT)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_REORDERING_UT) $(SOURCE_REORDERING_UT) -lm -lcunit
	@echo Compiled Reordering Unit Test
	@echo 

mainTarget: $(SOURCE_MAIN) $(NAME_OF_MAIN)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_MAIN) $(SOURCE_MAIN) -lm -lcunit
	@echo Compiled Main
	@echo 

$(NAME_OF_EXECUTABLE): $(OBJECT)
	$(CC) -g  -O0 $(OBJECT) -o $@ -lm -lcunit
	
$(NAME_OF_SEARCH_LIST_UT): $(OBJECT_SEARCH_LIST_UT)
	$(CC) -g  -O0 $(OBJECT_SEARCH_LIST_UT) -o $@ -lm -lcunit

$(NAME_OF_MAIN): $(OBJECT_MAIN)
	$(CC) -g  -O0 $(OBJECT_MAIN) -o $@ -lm -lcunit

$(NAME_OF_INPUT_UT): $(OBJECT_INPUT_UT)
	$(CC) -g  -O0 $(OBJECT_INPUT_UT) -o $@ -lm -lcunit

$(NAME_OF_REORDERING_UT): $(OBJECT_REORDERING_UT)
	$(CC) -g  -O0 $(OBJECT_REORDERING_UT) -o $@ -lm -lcunit

.c.o:
	$(CC) -c $< -o $@ -lm -lcunit


runMainUt: executable
	./$(NAME_OF_EXECUTABLE)

runSearchListUt: searchListUt
	./$(NAME_OF_SEARCH_LIST_UT)

runInputUt:
	./$(NAME_OF_INPUT_UT)

runReorderingUt:
	./$(NAME_OF_REORDERING_UT)

runMain:
	./$(NAME_OF_MAIN) -R testTables/r3_10000000_random -S testTables/r3_10000000_random -r 1 -s 2 -t binary -o testTables/outFile

runValgrind:
	valgrind $(VALGRIND_FLAGS) ./$(NAME_OF_MAIN) -R testTables/r3_10000000_random -S testTables/r3_10000000_random -r 1 -s 2 -t binary -o testTables/outFile

cacheMisses:
	perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./$(NAME_OF_MAIN) -R testTables/rSame3_1000000 -S testTables/rSame3_1000000 -r 1 -s 2 -t binary -o testTables/outFile

clean:
	rm -f $(NAME_OF_EXECUTABLE)
	rm -f $(NAME_OF_SEARCH_LIST_UT)
	rm -f $(NAME_OF_MAIN)
	rm -f $(NAME_OF_INPUT_UT)
	rm -f $(NAME_OF_REORDERING_UT)
	rm -f src/*.o

