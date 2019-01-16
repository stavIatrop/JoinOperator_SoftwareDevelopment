#include <stdio.h>

#include "basicStructs.h"

//source : http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html

void SetBit(myint_t * distinctArray, myint_t k) {

    myint_t i = k / (sizeof(myint_t) * 8);            // i = array index (use: distinctArray[i])
    myint_t pos = k % (sizeof(myint_t) * 8);          // pos = bit position in distinctArray[i]

    myint_t flag = 1;   

    flag = flag << pos;      

    distinctArray[i] = distinctArray[i] | flag;      // Set the bit at the k-th position in distinctArray[i]

    return;
}

myint_t TestBit(myint_t * distinctArray, myint_t k) {

    myint_t i = k/(sizeof(myint_t) * 8);
    myint_t pos = k%(sizeof(myint_t) * 8);

    myint_t flag = 1;  

    flag = flag << pos;     
 
    if ( distinctArray[i] & flag )      // Test the bit at the k-th position in distinctArray[i]
        return 1;                       // k-th bit is 1
    else
        return 0;                       // k-th bit is 0
}

