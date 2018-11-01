tableCreator: gcc -o createTable createTable.c  
	      ./createTable `cols` `rows` `outFile` `bin` (for binary) (SUGGESTED)  
	      ./createTable `cols` `rows` `outFile` `ascii` (for ascii)  

main: make  
      ./main -R `path_of_R` -S `path_of_S` -r `col_R` -s `col_S` -t `binary/ascii` -o `outPath`  

!! Για να λειτουργούν τα input test μην πειράξετε τα `file3_10, file3_10ascii` που βρίσκονται στο `smallTestTables` !!  
