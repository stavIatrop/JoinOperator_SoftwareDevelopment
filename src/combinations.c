#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basicStructs.h"
#include "pipeI_O.h"

//Auxiliary functions altered and incorporated in order to find all possible combinations of size r from n numbers
//https://www.geeksforgeeks.org/print-all-possible-combinations-of-r-elements-in-a-given-array-of-size-n/

int indexComb;      //index for combs array, which is used for combination storage

void combinationUtil(int * arr , int data[], int start, int end, int index, int r, char ** combs) 
{ 

    if (index == r) 
    { 

        combs[indexComb] = (char *)malloc((r + 1) * sizeof(char));
        memset(combs[indexComb], '\0', r + 1 );

        for (int j=0; j<r; j++) {
            char * str = (char *) malloc( (numDigits(data[j]) + 1) * sizeof(char) );
            memset(str, '\0', numDigits(data[j]) + 1);
            sprintf(str, "%d", data[j]);
           
            strcat(combs[indexComb], str);
            
            free(str);
        }
        strcat(combs[indexComb], "\0");

        
        indexComb++;
        return; 
    } 
  
    // replace index with all possible elements. The condition 
    // "end-i+1 >= r-index" makes sure that including one element 
    // at index will make a combination with remaining elements 
    // at remaining positions 
    for (int i=start; i<=end && end-i+1 >= r-index; i++) 
    { 
        data[index] = arr[i]; 
        combinationUtil(arr, data, i+1, end, index+1, r, combs); 
    } 
} 



void getCombination( int n, int r, char ** combs) 
{ 

    int data[r];  //an array that will store all combinations of size r from n numbers
    int * arr;
    arr = (int *) malloc(n * sizeof(int));
    
    for( int i = 0; i < n; i++) {
        arr[i] = i;
    }
        
    indexComb = 0; 
    
    combinationUtil(arr, data, 0, n-1, 0, r, combs); 
    free(arr);
} 
  

myint_t Factorial(myint_t n)
{
    if (n >= 1)
        return n * Factorial(n-1);
    else
        return 1;
}
