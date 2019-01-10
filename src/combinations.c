#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basicStructs.h"
#include "pipeI_O.h"

static int indexComb = 0;

void combinationUtil(int * arr , int data[], int start, int end, int index, int r, char ** combs) 
{ 
    // Current combination is ready to be printed, print it 
    if (index == r) 
    { 

        combs[indexComb] = (char *)malloc((2 * r + 1) * sizeof(char));
        memset(combs[indexComb], '\0', 2 * r + 1 );
        for (int j=0; j<r; j++) {
            char * str = (char *) malloc( (numDigits(data[j]) + 1) * sizeof(char) );
            memset(str, '\0', numDigits(data[j]) + 1);
            sprintf(str, "%d", data[j]);
            strcat(combs[indexComb], str);
            strcat(combs[indexComb], " ");
            free(str);
        } 
            //printf("%d ", data[j]); 
        
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
    // A temporary array to store all combination one by one 
    int data[r]; 
    int * arr;
    arr = (int *) malloc(n * sizeof(int));
    for( int i = 0; i < n; i++)
        arr[i] = i;

    // Print all combination using temprary array 'data[]' 
    combinationUtil(arr, data, 0, n-1, 0, r, combs); 
    free(arr);
} 
  
/* arr[]  ---> Input Array 
   data[] ---> Temporary array to store current combination 
   start & end ---> Staring and Ending indexes in arr[] 
   index  ---> Current index in data[] 
   r ---> Size of a combination to be printed */



myint_t Factorial(myint_t n)
{
    if (n >= 1)
        return n*Factorial(n-1);
    else
        return 1;
}