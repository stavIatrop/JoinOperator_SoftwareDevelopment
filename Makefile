CC = gcc

SOURCE_SEARCH_LIST_UT = src/searchListTesting.c src/resultListManip.c src/chainFollower.c src/hashing.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c
SOURCE_MAIN = src/main.c src/radixHashJoin.c src/inputManip.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c src/interList.c
SOURCE_INPUT_UT = src/inputTesting.c src/inputManip.c src/resultListManip.c
SOURCE_REORDERING_UT = src/viceTests.c src/functions.c
SOURCE_INDEXING_UT = src/indexTesting.c src/indexManip.c src/hashing.c src/sideFunctions.c
SOURCE_PROJECT_UT = src/projectUnitTest.c src/radixHashJoin.c src/inputManip.c src/resultListManip.c src/hashing.c src/chainFollower.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c src/I_O_structManip.c src/queryManip.c
SOURCE_QUERY_EXECUTE_UT = src/queryExecuteTesting.c src/jointPerformer.c src/radixHashJoin.c src/interList.c src/resultListManip.c src/checksum.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c src/inputManip.c src/hashing.c src/chainFollower.c src/I_O_structManip.c src/queryManip.c src/jobScheduler.c src/getStats.c src/bitVector.c src/hashTreeManip.c src/pipeI_O.c
SOURCE_I_O_RECEIVER = src/I_O_Receiver.c src/I_O_structManip.c src/pipeI_O.c src/queryManip.c src/jointPerformer.c src/radixHashJoin.c src/interList.c src/resultListManip.c src/checksum.c src/indexing.c src/indexManip.c src/sideFunctions.c src/H1.c src/functions.c src/inputManip.c src/hashing.c src/chainFollower.c src/jobScheduler.c src/getStats.c src/bitVector.c src/graph.c src/hashTreeManip.c


NAME_OF_SEARCH_LIST_UT = searchListUnitTest
NAME_OF_MAIN = main
NAME_OF_INPUT_UT = inputUt
NAME_OF_REORDERING_UT = reorderingTest
NAME_OF_INDEXING_UT = indexingUnitTest
NAME_OF_PROJECT_UT = projectUnitTest
NAME_OF_QUERY_EXECUTE_UT = queryExecuteUnitTest
NAME_OF_I_O_RECEIVER = ioreceiver

OBJECT_SEARCH_LIST_UT = $(SOURCE_SEARCH_LIST_UT:.c=.o)
OBJECT_MAIN = $(SOURCE_MAIN:.c=.o)
OBJECT_INPUT_UT = $(SOURCE_INPUT_UT:.c=.o)
OBJECT_REORDERING_UT = $(SOURCE_REORDERING_UT:.c=.o)
OBJECT_INDEXING_UT = $(SOURCE_INDEXING_UT:.c=.o)
OBJECT_PROJECT_UT = $(SOURCE_PROJECT_UT:.c=.o)
OBJECT_QUERY_EXECUTE_UT = $(SOURCE_QUERY_EXECUTE_UT:.c=.o)
OBJECT_I_O_RECEIVER = $(SOURCE_I_O_RECEIVER:.c=.o)

VALGRIND_FLAGS = --leak-check=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes -v

all: queryExecuteUtTarget inputUtTarget searchListUt reorderingUtTarget indexingUnitTest ioreceiverTarget
	@echo  
	@echo Compile finished

projectUt: $(SOURCE_PROJECT_UT) $(NAME_OF_PROJECT_UT)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_PROJECT_UT) $(SOURCE_PROJECT_UT) -lm -lcunit
	@echo Compiled Project Unit tests
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

indexingUt: $(SOURCE_INDEXING_UT) $(NAME_OF_INDEXING_UT)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_INDEXING_UT) $(SOURCE_INDEXING_UT) -lm -lcunit
	@echo Compiled Indexing Unit Test
	@echo 

queryExecuteUtTarget: $(SOURCE_QUERY_EXECUTE_UT) $(NAME_OF_QUERY_EXECUTE_UT)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_QUERY_EXECUTE_UT) $(SOURCE_QUERY_EXECUTE_UT) -lm -lcunit -pthread
	@echo Compiled Indexing Unit Test
	@echo 


mainTarget: $(SOURCE_MAIN) $(NAME_OF_MAIN)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_MAIN) $(SOURCE_MAIN) -lm -lcunit
	@echo Compiled Main
	@echo

ioreceiverTarget: $(SOURCE_I_O_RECEIVER) $(NAME_OF_I_O_RECEIVER)
	$(CC) -g -O0 -Wall -o  $(NAME_OF_I_O_RECEIVER) $(SOURCE_I_O_RECEIVER) -lm -pthread
	@echo Compiled I_O_Receiver
	@echo

# harnessTarget:
# 	./compile.sh
# 	@echo Compiled harness
# 	@echo

$(NAME_OF_PROJECT_UT): $(OBJECT_PROJECT_UT)
	$(CC) -g  -O0 $(OBJECT_PROJECT_UT) -o $@ -lm -lcunit

$(NAME_OF_SEARCH_LIST_UT): $(OBJECT_SEARCH_LIST_UT)
	$(CC) -g  -O0 $(OBJECT_SEARCH_LIST_UT) -o $@ -lm -lcunit

$(NAME_OF_MAIN): $(OBJECT_MAIN)
	$(CC) -g  -O0 $(OBJECT_MAIN) -o $@ -lm -lcunit

$(NAME_OF_INPUT_UT): $(OBJECT_INPUT_UT)
	$(CC) -g  -O0 $(OBJECT_INPUT_UT) -o $@ -lm -lcunit

$(NAME_OF_REORDERING_UT): $(OBJECT_REORDERING_UT)
	$(CC) -g  -O0 $(OBJECT_REORDERING_UT) -o $@ -lm -lcunit

$(NAME_OF_INDEXING_UT): $(OBJECT_INDEXING_UT)
	$(CC) -g  -O0 $(OBJECT_INDEXING_UT) -o $@ -lm -lcunit

$(NAME_OF_QUERY_EXECUTE_UT): $(OBJECT_QUERY_EXECUTE_UT)
	$(CC) -g  -O0 $(OBJECT_QUERY_EXECUTE_UT) -o $@ -lm -lcunit -pthread

$(NAME_OF_I_O_RECEIVER): $(OBJECT_I_O_RECEIVER)
	$(CC) -g -O0 $(OBJECT_I_O_RECEIVER) -o $@ -lm -pthread


.c.o:
	$(CC) -c $< -o $@ -lm -lcunit -pthread


runProjectUt: projectUt
	./$(NAME_OF_PROJECT_UT)

runQueryExecuteUt:
	./$(NAME_OF_QUERY_EXECUTE_UT)

runSearchListUt: searchListUt
	./$(NAME_OF_SEARCH_LIST_UT)

runInputUt:
	./$(NAME_OF_INPUT_UT)

runReorderingUt:
	./$(NAME_OF_REORDERING_UT)

runIndexingUt:
	./$(NAME_OF_INDEXING_UT)

runValIndexingUt:
	valgrind $(VALGRIND_FLAGS) ./$(NAME_OF_INDEXING_UT)

runMain:
	./$(NAME_OF_MAIN) -R testTables/r3_10000000_ordered -S testTables/r3_10000000_ordered -r 2 -s 2 -t binary -o testTables/outFile

runValgrind:
	valgrind $(VALGRIND_FLAGS) ./$(NAME_OF_MAIN) -R testTables/r3_10000000_ordered -S testTables/r3_10000000_ordered -r 0 -s 0 -t binary -o testTables/outFile

runValgrindUt:
	valgrind $(VALGRIND_FLAGS) ./$(NAME_OF_QUERY_EXECUTE_UT)

runHarness:
	./harness small/small.init small/small.work small/small.result ./ioreceiver

runHarnessPublic:
	./harness public/public.init public/public.work public/public.result ./ioreceiver

runValgrindHarness:
	valgrind $(VALGRIND_FLAGS) --log-file="valgrindOut" ./harness small/small.init small/small.work small/small.result ./ioreceiver

cacheMisses:
	perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,faults,migrations ./$(NAME_OF_MAIN) -R testTables/rSame3_1000000 -S testTables/rSame3_1000000 -r 1 -s 2 -t binary -o testTables/outFile

clean:
	rm -f $(NAME_OF_EXECUTABLE)
	rm -f $(NAME_OF_SEARCH_LIST_UT)
	rm -f $(NAME_OF_MAIN)
	rm -f $(NAME_OF_INPUT_UT)
	rm -f $(NAME_OF_REORDERING_UT)
	rm -f $(NAME_OF_INDEXING_UT)
	rm -f $(NAME_OF_PROJECT_UT)
	rm -f $(NAME_OF_QUERY_EXECUTE_UT)
	rm -f $(NAME_OF_I_O_RECEIVER)
	rm -f src/*.o

