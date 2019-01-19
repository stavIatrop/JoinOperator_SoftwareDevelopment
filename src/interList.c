#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "basicStructs.h"
#include "queryStructs.h"
#include "resultListInterface.h"
#include "jointPerformer.h"

inter * initialiseInter(myint_t cols, myint_t rows, myint_t * joinedRels, myint_t ** rowIds) {
    inter * retInter = (inter *) malloc(sizeof(inter));
    retInter->joinedRels = joinedRels;
    retInter->numOfCols = cols;
    retInter->rowIds = rowIds;
    retInter->numbOfRows = rows;
    return retInter;
}

nodeInter * initialiseNode(myint_t cols, myint_t rows, myint_t * joinedRels, myint_t ** rowIds) {
    nodeInter * node = (nodeInter *) malloc(sizeof(nodeInter));
    node->data = initialiseInter(cols, rows, joinedRels, rowIds);
    node->next = NULL;

    return node;
}

headInter * initialiseHead() {
    headInter * head = (headInter *) malloc(sizeof(headInter));
    head->numOfIntermediates = 0;
    head->start = NULL;

    return head;
}

void freeNode(nodeInter * node) {
    for(myint_t whichCol = 0; whichCol < node->data->numOfCols; whichCol++) {
       free(node->data->rowIds[whichCol]);
    }
    free(node->data->rowIds);
    free(node->data->joinedRels);
    free(node->data);
    free(node);
}

void freeNodeListRec(nodeInter * node) {
    if(node->next != NULL) {
        freeNodeListRec(node->next);
    }
    freeNode(node);
}

void freeInterList(headInter * head) {
    if(head->start != NULL) {
        freeNodeListRec(head->start);
    }
    free(head);

}

void pushInter(headInter * head ,myint_t cols, myint_t rows, myint_t * joinedRels, myint_t ** rowIds) {
    if(head->numOfIntermediates == 0) {
        head->start = initialiseNode(cols, rows, joinedRels, rowIds);
    }
    else {
        nodeInter * finalNode = head->start;
        for(myint_t whichNode = 0; whichNode < head->numOfIntermediates - 1; whichNode++) {
            finalNode = finalNode->next;
        }
        finalNode->next = initialiseNode(cols, rows, joinedRels, rowIds);
    }
    head->numOfIntermediates += 1;
}

void updateInter(nodeInter * node, myint_t cols,  myint_t rows, myint_t * joinedRels, myint_t ** rowIds) {
    free(node->data->joinedRels);
    for(myint_t i = 0; i < node->data->numOfCols; i++) {
        free(node->data->rowIds[i]);
    }
    free(node->data->rowIds);
    node->data->joinedRels = joinedRels;
    node->data->numOfCols = cols;
    node->data->rowIds = rowIds;
    node->data->numbOfRows = rows;
}

void deleteInterNode(headInter * head, nodeInter * node) {
    nodeInter * temp = node->next;
    if(head->numOfIntermediates == 0) {
        perror("Unexpected delete on empty list\n");
    }
    else {
        //Traverse until you find the node to be deleted
        if(head->start == node) {
            freeNode(node);
            head->start = temp;
        }
        else {
            nodeInter * currentNode = head->start;
            for(myint_t whichNode = 0; whichNode < head->numOfIntermediates - 1; whichNode++) {
                if(currentNode->next == node) {
                    freeNode(node);
                    currentNode->next = temp;
                    break;
                }
                currentNode = currentNode->next;
            }
        }
    }
    head->numOfIntermediates -= 1;
}

//Creates a new Intermediate array containing 2 columns from the radixHashJoin results
myint_t ** createResultArray(headResult * head, myint_t * size, char switched) {

    myint_t numbOfResults = head->totalSize;
    *size = numbOfResults;
    myint_t ** retArr = (myint_t **) malloc(2 * sizeof(myint_t *));
    for(myint_t i = 0; i < 2; i++) {
        retArr[i] = (myint_t *) malloc(numbOfResults * sizeof(myint_t));
    }

    //Inserting the results in the intermediate array
    resultNode * currentNode = head->firstNode;    
    myint_t counter = 0;
    if(switched == 0) {
        for(myint_t whichNode = 0; whichNode < head->numbOfNodes; whichNode++) {
            for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {
                retArr[0][counter] = currentNode->tuples[whichRes].rowR;
                retArr[1][counter] = currentNode->tuples[whichRes].rowS;
                counter += 1;
            }
            currentNode = currentNode->nextNode;
        }
    }
    else {
        for(myint_t whichNode = 0; whichNode < head->numbOfNodes; whichNode++) {
            for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {
                retArr[0][counter] = currentNode->tuples[whichRes].rowS;
                retArr[1][counter] = currentNode->tuples[whichRes].rowR;
                counter += 1;
            }
            currentNode = currentNode->nextNode;
        }
    }
    

    return retArr;
}

//CASE: Both join relationships are not a member of any other Intermediate
void createInterFromRes(headInter * headInt, headResult * headRes, myint_t rel1, myint_t rel2, char switched) {
    myint_t rows;
    myint_t ** rowIds = createResultArray(headRes, &rows, switched);

    myint_t * joinedRels = (myint_t *) malloc(2 * sizeof(myint_t));
    joinedRels[0] = rel1; 
    joinedRels[1] = rel2;

    pushInter(headInt, 2, rows, joinedRels, rowIds);

}

//Keeps the rows that passed the join and adds a new column from the results
myint_t ** updateRowIds(nodeInter * intNode, headResult * headRes, myint_t results, char switched) {

    //Memory Allocations
    myint_t ** retArr = (myint_t **) malloc((intNode->data->numOfCols + 1) * sizeof(myint_t *));
    for(myint_t i = 0; i < intNode->data->numOfCols + 1; i++) {
        retArr[i] = (myint_t *) malloc(results * sizeof(myint_t));
    }

    //Copy the rows of the inter that where joined and add them the row of the new relationship
    resultNode * currentNode = headRes->firstNode;
    myint_t base = 0;
    if(switched == 0) {
        for(myint_t whichNode = 0; whichNode < headRes->numbOfNodes; whichNode++) {
            for(myint_t whichCol = 0; whichCol < intNode->data->numOfCols + 1; whichCol++) {
                for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {
                    if(whichCol != intNode->data->numOfCols) {
                        retArr[whichCol][whichRes + base] = intNode->data->rowIds[whichCol][currentNode->tuples[whichRes].rowR];
                    }
                    else {
                        retArr[whichCol][whichRes + base] = currentNode->tuples[whichRes].rowS;
                    }
                        
                }
            }
            base += currentNode->size;
            currentNode = currentNode->nextNode;
        }
    }
    else {
        for(myint_t whichNode = 0; whichNode < headRes->numbOfNodes; whichNode++) {
            for(myint_t whichCol = 0; whichCol < intNode->data->numOfCols + 1; whichCol++) {
                for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {
                    if(whichCol != intNode->data->numOfCols) {
                        retArr[whichCol][whichRes + base] = intNode->data->rowIds[whichCol][currentNode->tuples[whichRes].rowS];
                    }
                    else {
                        retArr[whichCol][whichRes + base] = currentNode->tuples[whichRes].rowR;
                    }
                        
                }
            }
            base += currentNode->size;
            currentNode = currentNode->nextNode;
        }
    }
    
    return retArr;
}

//CASE: One relationship belongs to an intermediate (care the R relationship must be the Intermediate)
void updateInterFromRes(nodeInter * intNode, headResult * headRes, myint_t addedRel, char switched) {
    //New rows
    myint_t numbOfResults = headRes->totalSize;

    //Add new rel in joinedRels
    myint_t * joinedRels = (myint_t *) malloc((intNode->data->numOfCols + 1) * sizeof(myint_t));
    for(myint_t whichRel = 0; whichRel < intNode->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = intNode->data->joinedRels[whichRel];
    }
    joinedRels[intNode->data->numOfCols] = addedRel;

    //Create new rowIds
    myint_t ** newRowIds = updateRowIds(intNode, headRes, numbOfResults, switched);


    updateInter(intNode, intNode->data->numOfCols + 1, numbOfResults, joinedRels, newRowIds);

}

//Takes the rows of both inters that passed the join and joins the into one
myint_t ** joinRowIds(nodeInter * node1, nodeInter * node2, headResult * headRes, myint_t results, char switched) {

    //Memory Allocations
    myint_t ** retArr = (myint_t **) malloc((node1->data->numOfCols + node2->data->numOfCols) * sizeof(myint_t *));
    for(myint_t i = 0; i < node1->data->numOfCols + node2->data->numOfCols; i++) {
        retArr[i] = (myint_t *) malloc(results * sizeof(myint_t));
    }

    resultNode * currentNode = headRes->firstNode;    
    myint_t counter = 0;
    for(myint_t whichNode = 0; whichNode < headRes->numbOfNodes; whichNode++) {
        if(switched == 0) {
            for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {
                //Add the rowIds of the first Intermediate
                for(myint_t whichCol = 0; whichCol < node1->data->numOfCols; whichCol++) {
                    retArr[whichCol][counter] = node1->data->rowIds[whichCol][currentNode->tuples[whichRes].rowR];
                }

                //Add the rowIds of the second Intermediate
                for(myint_t whichCol = node1->data->numOfCols; whichCol < node1->data->numOfCols + node2->data->numOfCols; whichCol++) {
                    retArr[whichCol][counter] = node2->data->rowIds[whichCol - node1->data->numOfCols][currentNode->tuples[whichRes].rowS];
                }

                counter += 1;
            }
        }
        else {
            for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {
                //Add the rowIds of the first Intermediate
                for(myint_t whichCol = 0; whichCol < node1->data->numOfCols; whichCol++) {
                    retArr[whichCol][counter] = node1->data->rowIds[whichCol][currentNode->tuples[whichRes].rowS];
                }

                //Add the rowIds of the second Intermediate
                for(myint_t whichCol = node1->data->numOfCols; whichCol < node1->data->numOfCols + node2->data->numOfCols; whichCol++) {
                    retArr[whichCol][counter] = node2->data->rowIds[whichCol - node1->data->numOfCols][currentNode->tuples[whichRes].rowR];
                }

                counter += 1;
            }
        }
        
        currentNode = currentNode->nextNode;
    }
    return retArr;
}

//CASE: Both relationships belong to an intermediate (Inter1 will be updated, Inter2 will be deleted)
void updateInterAndDelete(headInter * headInt, nodeInter * node1, nodeInter * node2, headResult * headRes, char switched, char packaged) {
    //New rows
    myint_t numbOfResults = headRes->totalSize;

    myint_t * joinedRels = (myint_t *) malloc((node1->data->numOfCols + node2->data->numOfCols) * sizeof(myint_t));

    //Add joinedRels of node1 in new joinedRels
    for(myint_t whichRel = 0; whichRel < node1->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = node1->data->joinedRels[whichRel];
    }

    //Add joinedRels of node2 in new joinedRels
    for(myint_t whichRel = node1->data->numOfCols; whichRel < node1->data->numOfCols + node2->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = node2->data->joinedRels[whichRel - node1->data->numOfCols];
    }

    //Create new rowIds
    myint_t ** newRowIds = joinRowIds(node1, node2, headRes, numbOfResults, switched);

    //Update Inter1
    updateInter(node1, node1->data->numOfCols + node2->data->numOfCols, numbOfResults, joinedRels, newRowIds);

    //Delete Inter2
    deleteInterNode(headInt, node2);

}

//CASE: Self join on rel that exists in an Intermediate
void updateInterSelfJoin(nodeInter * node, myint_t * joinRows, myint_t numbOfRows) {
    myint_t ** newRowIds;

    newRowIds = (myint_t **) malloc(node->data->numOfCols * sizeof(myint_t *));
    for(myint_t i = 0; i < node->data->numOfCols; i++) {
        newRowIds[i] = (myint_t *) malloc(numbOfRows * sizeof(myint_t));
    }

    for(myint_t whichRow = 0; whichRow < numbOfRows; whichRow++) {
        for(myint_t whichCol = 0; whichCol < node->data->numOfCols; whichCol++) {
            newRowIds[whichCol][whichRow] = node->data->rowIds[whichCol][joinRows[whichRow]];
        }
    }

    myint_t * joinedRels = (myint_t *) malloc(node->data->numOfCols * sizeof(myint_t));
    for(myint_t whichRel = 0; whichRel < node->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = node->data->joinedRels[whichRel];
    }
    
    updateInter(node, node->data->numOfCols, numbOfRows, joinedRels, newRowIds);
}

//CASE: Self join on rel that does NOT exist in an Intermediate
void createInterSelfJoin(headInter * head, myint_t rel, myint_t * rows, myint_t numbOfRows) {
    myint_t ** newRowIds = (myint_t **) malloc( sizeof(myint_t *));
    for(myint_t i = 0; i < 1; i++) {
        newRowIds[i] = (myint_t *) malloc(numbOfRows * sizeof(myint_t));
    }

    for(myint_t whichRow = 0; whichRow < numbOfRows; whichRow++) {
        newRowIds[0][whichRow] = rows[whichRow];
    }

    myint_t * joinedRels = (myint_t *) malloc(sizeof(myint_t));
    joinedRels[0] = rel;

    pushInter(head, 1, numbOfRows, joinedRels, newRowIds);

}

myint_t ** updateRowIds2(nodeInter * intNode, headResult * headRes, myint_t *results, myint_t existingRel, char switched) {

    //Memory Allocations
    myint_t cursize = *results;
    myint_t ** retArr = (myint_t **) malloc((intNode->data->numOfCols + 1) * sizeof(myint_t *));
    for(myint_t i = 0; i < intNode->data->numOfCols + 1; i++) {
        retArr[i] = (myint_t *) malloc(cursize * sizeof(myint_t));
    }
    // fprintf(stderr, "RESULTS: %ld\n", headRes->numbOfNodes);

    myint_t existingRelPlace;
    for(myint_t whichRel = 0; whichRel < intNode->data->numOfCols; whichRel++) {
        if (intNode->data->joinedRels[whichRel] == existingRel) {
            existingRelPlace = whichRel;
            break;
        }
    }

    //fprintf(stderr, "AAAAAAAA");

    //Copy the rows of the inter that were joined and add them the row of the new relationship
    resultNode * currentNode = headRes->firstNode;    
    myint_t counter = 0;
    myint_t next;
    for(myint_t whichNode = 0; whichNode < headRes->numbOfNodes; whichNode++) {
        if(switched == 0) {
            for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {

                next = findNextRowId(intNode->data->rowIds[existingRelPlace], currentNode->tuples[whichRes].rowR, intNode->data->numbOfRows);
                for (myint_t p = currentNode->tuples[whichRes].rowR; p < next; p++) {

                    for(myint_t whichCol = 0; whichCol < intNode->data->numOfCols; whichCol++) {
                            retArr[whichCol][counter] = intNode->data->rowIds[whichCol][p];
                    }
                    retArr[intNode->data->numOfCols][counter] = currentNode->tuples[whichRes].rowS;
                    counter += 1;
                    if (counter==cursize) {
                        cursize += *results;
                        for(myint_t i = 0; i < intNode->data->numOfCols + 1; i++) {
                            retArr[i] = (myint_t *) realloc(retArr[i],cursize * sizeof(myint_t));
                        }
                    }

                }
            }
        }
        else {
            for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {

                next = findNextRowId(intNode->data->rowIds[existingRelPlace], currentNode->tuples[whichRes].rowS, intNode->data->numbOfRows);
                for (myint_t p = currentNode->tuples[whichRes].rowS; p < next; p++) {

                    for(myint_t whichCol = 0; whichCol < intNode->data->numOfCols; whichCol++) {
                            retArr[whichCol][counter] = intNode->data->rowIds[whichCol][p];
                    }
                    retArr[intNode->data->numOfCols][counter] = currentNode->tuples[whichRes].rowR;
                    counter += 1;
                    if (counter==cursize) {
                        cursize += *results;
                        for(myint_t i = 0; i < intNode->data->numOfCols + 1; i++) {
                            retArr[i] = (myint_t *) realloc(retArr[i],cursize * sizeof(myint_t));
                        }
                    }
                        
                }

            }
        }
        currentNode = currentNode->nextNode;
    }

    for(myint_t i = 0; i < intNode->data->numOfCols + 1; i++) {
        retArr[i] = (myint_t *) realloc(retArr[i],counter * sizeof(myint_t));
    }
    *results = counter;

    return retArr;
}

//CASE: One relationship belongs to an intermediate (care the R relationship must be the Intermediate)
void updateInterFromRes2(nodeInter * intNode, headResult * headRes, myint_t addedRel, myint_t existingRel, myint_t skipped, char switched) {
    //New rows
    myint_t numbOfResults = headRes->totalSize + skipped;
    //fprintf(stderr, "AAAAAAAAAAAA: %ld\n", numbOfResults);

    //Add new rel in joinedRels
    myint_t * joinedRels = (myint_t *) malloc((intNode->data->numOfCols + 1) * sizeof(myint_t));
    for(myint_t whichRel = 0; whichRel < intNode->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = intNode->data->joinedRels[whichRel];
    }
    joinedRels[intNode->data->numOfCols] = addedRel;

    //Create new rowIds
    myint_t ** newRowIds = updateRowIds2(intNode, headRes, &numbOfResults, existingRel, switched);


    updateInter(intNode, intNode->data->numOfCols + 1, numbOfResults, joinedRels, newRowIds);

    //fprintf(stderr, "    Join inter rows = %ld | cols = %ld\n", intNode->data->numbOfRows, intNode->data->numOfCols);


}

myint_t ** joinRowIds2(nodeInter * node1, nodeInter * node2, headResult * headRes, myint_t *results, myint_t existingRel1, myint_t existingRel2, char switched) {

    //Memory Allocations
    myint_t cursize = *results;
    myint_t ** retArr = (myint_t **) malloc((node1->data->numOfCols + node2->data->numOfCols) * sizeof(myint_t *));
    for(myint_t i = 0; i < node1->data->numOfCols + node2->data->numOfCols; i++) {
        retArr[i] = (myint_t *) malloc(cursize * sizeof(myint_t));
        if (retArr[i]==NULL) fprintf(stderr,"Not enough memory, %ld\n", *results);
    }

    myint_t existingRelPlace1, existingRelPlace2;

    for(myint_t whichRel = 0; whichRel < node1->data->numOfCols; whichRel++) {
        if (node1->data->joinedRels[whichRel] == existingRel1) {
            existingRelPlace1 = whichRel;
            break;
        }
    }

    for(myint_t whichRel = 0; whichRel < node2->data->numOfCols; whichRel++) {
        if (node2->data->joinedRels[whichRel] == existingRel2) {
            existingRelPlace2 = whichRel;
            break;
        }
    }

    resultNode * currentNode = headRes->firstNode;    
    myint_t counter = 0, next1, next2;
    for(myint_t whichNode = 0; whichNode < headRes->numbOfNodes; whichNode++) {
        if(switched == 0) {
            for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {

                next1 = findNextRowId(node1->data->rowIds[existingRelPlace1],currentNode->tuples[whichRes].rowR,node1->data->numbOfRows);
                next2 = findNextRowId(node2->data->rowIds[existingRelPlace2],currentNode->tuples[whichRes].rowS,node2->data->numbOfRows);
                for (myint_t p1 = currentNode->tuples[whichRes].rowR; p1 < next1; p1++) {
                    for (myint_t p2 = currentNode->tuples[whichRes].rowS; p2 < next2; p2++) {

                        for(myint_t whichCol = 0; whichCol < node1->data->numOfCols; whichCol++) {
                            retArr[whichCol][counter] = node1->data->rowIds[whichCol][p1];
                        }

                        //Add the rowIds of the second Intermediate
                        for(myint_t whichCol = node1->data->numOfCols; whichCol < node1->data->numOfCols + node2->data->numOfCols; whichCol++) {
                            retArr[whichCol][counter] = node2->data->rowIds[whichCol - node1->data->numOfCols][p2];
                        }

                        counter += 1;
                        if (counter==cursize) {
                            cursize += *results;
                            for(myint_t i = 0; i < node1->data->numOfCols + node2->data->numOfCols; i++) {
                                retArr[i] = (myint_t *) realloc(retArr[i],cursize * sizeof(myint_t));
                            }
                        }

                    }
                }
                //fprintf(stderr, "%ld\n", counter);

            }

        }
        else {
            for(myint_t whichRes = 0; whichRes < currentNode->size; whichRes++) {
                
                next1 = findNextRowId(node1->data->rowIds[existingRelPlace1],currentNode->tuples[whichRes].rowS,node1->data->numbOfRows);
                next2 = findNextRowId(node2->data->rowIds[existingRelPlace2],currentNode->tuples[whichRes].rowR,node2->data->numbOfRows);
                for (myint_t p1 = currentNode->tuples[whichRes].rowS; p1 < next1; p1++) {
                    for (myint_t p2 = currentNode->tuples[whichRes].rowR; p2 < next2; p2++) {

                        for(myint_t whichCol = 0; whichCol < node1->data->numOfCols; whichCol++) {
                            retArr[whichCol][counter] = node1->data->rowIds[whichCol][p1];
                        }

                        //Add the rowIds of the second Intermediate
                        for(myint_t whichCol = node1->data->numOfCols; whichCol < node1->data->numOfCols + node2->data->numOfCols; whichCol++) {
                            retArr[whichCol][counter] = node2->data->rowIds[whichCol - node1->data->numOfCols][p2];
                        }

                        counter += 1;
                        if (counter==cursize) {
                            cursize += *results;
                            for(myint_t i = 0; i < node1->data->numOfCols + node2->data->numOfCols; i++) {
                                retArr[i] = (myint_t *) realloc(retArr[i],cursize * sizeof(myint_t));
                            }
                        }

                    }
                }                //fprintf(stderr, "AA %ld\n", counter);

           }
        }
        
        currentNode = currentNode->nextNode;
    }

    for(myint_t i = 0; i < node1->data->numOfCols + node2->data->numOfCols; i++) {
        retArr[i] = (myint_t *) realloc(retArr[i],counter * sizeof(myint_t));
    }
    *results = counter;
    return retArr;
}

//CASE: Both relationships belong to an intermediate (Inter1 will be updated, Inter2 will be deleted)
void updateInterAndDelete2(headInter * headInt, nodeInter * node1, nodeInter * node2, headResult * headRes, myint_t existingRel1, myint_t existingRel2, myint_t skipped, char switched) {
    //New rows
    myint_t numbOfResults = headRes->totalSize + skipped;

    myint_t * joinedRels = (myint_t *) malloc((node1->data->numOfCols + node2->data->numOfCols) * sizeof(myint_t));

    //Add joinedRels of node1 in new joinedRels
    for(myint_t whichRel = 0; whichRel < node1->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = node1->data->joinedRels[whichRel];
    }

    //Add joinedRels of node2 in new joinedRels
    for(myint_t whichRel = node1->data->numOfCols; whichRel < node1->data->numOfCols + node2->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = node2->data->joinedRels[whichRel - node1->data->numOfCols];
    }

    myint_t ** newRowIds = joinRowIds2(node1, node2, headRes, &numbOfResults, existingRel1, existingRel2, switched);

    //Update Inter1
    updateInter(node1, node1->data->numOfCols + node2->data->numOfCols, numbOfResults, joinedRels, newRowIds);

    //Delete Inter2
    deleteInterNode(headInt, node2);
}