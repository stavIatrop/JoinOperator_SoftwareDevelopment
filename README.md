 # Ανάπτυξη Λογισμικού Για Πληροφοριακά Συστήματα (Μέρος 1ο)
   
 ## Μέλη Ομάδας:
 * Ιατροπούλου Σταυρούλα, 1115201500048
 * Ξύδας Μιχαήλ, 1115201500116
 * Στάης Βαλέριος, 1115201500148
                 
 

 ## Compile and Run
Compile: `make` will compile all the executables (unit tests and main executable)  

Run:
 * `./main -R path_of_R -S path_of_S -r col_R -s col_S -t binary/ascii -o outPath`  
 (-R is the path of table R, -r is the column of table R that we want to be joined with column s of table S)
 * `make runInputUt` runs the Input unit test
 * `make runReorderingUt`runs the Hash1 and Reordering tests
 * `make runIndexingUt` runs the Indexing unit tests
 * `make runSearchListUt` runs the Search and List unit test functions


!! In order for input test to be able to pass, `smallTestTables/file3_10, smallTestTables/file3_10ascii` must not be changed` !!  

 ## File Creation Script

 `gcc -o createTable createTable.c`  
 `./createTable numb_of_cols numb_of_rows  out_file_path bin/ascii` (by deafault the values are random) 
