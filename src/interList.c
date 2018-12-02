#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "basicStructs.h"
#include "queryStructs.h"
#include "resultListInterface.h"

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
    for(int whichCol = 0; whichCol < node->data->numOfCols; whichCol++) {
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
        for(int whichNode = 0; whichNode < head->numOfIntermediates - 1; whichNode++) {
            finalNode = finalNode->next;
        }
        finalNode->next = initialiseNode(cols, rows, joinedRels, rowIds);
    }
    head->numOfIntermediates += 1;
}

void updateInter(nodeInter * node, myint_t cols,  myint_t rows, myint_t * joinedRels, myint_t ** rowIds) {
    free(node->data->joinedRels);
    for(int i = 0; i < node->data->numOfCols; i++) {
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
        printf("Unexpected delete on empty list\n");
    }
    else {
        //Traverse until you find the node to be deleted
        if(head->start == node) {
            freeNode(node);
            head->start = temp;
        }
        else {
            nodeInter * currentNode = head->start;
            for(int whichNode = 0; whichNode < head->numOfIntermediates - 1; whichNode++) {
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

myint_t ** createResultArray(headResult * head, myint_t * size) {
    if(head->numbOfNodes == 0) {
        *size = 0;
        return NULL;
    }

    myint_t numbOfResults = countSizeOfList(head);
    *size = numbOfResults;
    myint_t ** retArr = (myint_t **) malloc(2 * sizeof(myint_t *));
    for(int i = 0; i < 2; i++) {
        retArr[i] = (myint_t *) malloc(numbOfResults * sizeof(myint_t));
    }

    //Inserting the results in the intermediate array
    resultNode * currentNode = head->firstNode;    
    myint_t counter = 0;
    for(int whichNode = 0; whichNode < head->numbOfNodes; whichNode++) {
        for(int whichRes = 0; whichRes < currentNode->size; whichRes++) {
            retArr[0][counter] = currentNode->tuples[whichRes].rowR;
            retArr[1][counter] = currentNode->tuples[whichRes].rowS;
            counter += 1;
        }
        currentNode = currentNode->nextNode;
    }

    return retArr;
}

//CASE: Both join relationships are not a member of any other Intermediate
void createInterFromRes(headInter * headInt, headResult * headRes, myint_t rel1, myint_t rel2) {
    myint_t rows;
    myint_t ** rowIds = createResultArray(headRes, &rows);

    myint_t * joinedRels = (myint_t *) malloc(2 * sizeof(myint_t));
    joinedRels[0] = rel1; 
    joinedRels[1] = rel2;

    pushInter(headInt, 2, rows, joinedRels, rowIds);
}

myint_t ** updateRowIds(nodeInter * intNode, headResult * headRes, myint_t results) {
    if(results == 0) {
        return NULL;
    }

    //Memory Allocations
    myint_t ** retArr = (myint_t **) malloc((intNode->data->numOfCols + 1) * sizeof(myint_t *));
    for(myint_t i = 0; i < intNode->data->numOfCols + 1; i++) {
        retArr[i] = (myint_t *) malloc(results * sizeof(myint_t));
    }

    //Copy the rows of the inter that where joined and add them the row of the new relationship
    resultNode * currentNode = headRes->firstNode;    
    myint_t counter = 0;
    for(int whichNode = 0; whichNode < headRes->numbOfNodes; whichNode++) {
        for(int whichRes = 0; whichRes < currentNode->size; whichRes++) {
            for(int whichCol = 0; whichCol < intNode->data->numOfCols; whichCol++) {
                retArr[whichCol][counter] = intNode->data->rowIds[whichCol][currentNode->tuples[whichRes].rowR];
            }
            retArr[intNode->data->numOfCols][counter] = currentNode->tuples[whichRes].rowS;
            counter += 1;
        }
        currentNode = currentNode->nextNode;
    }

    return retArr;
}

//CASE: One relationship belongs to an intermediate (care the R relationship must be the Intermediate)
void updateInterFromRes(nodeInter * intNode, headResult * headRes, myint_t addedRel) {
    //New rows
    myint_t numbOfResults = countSizeOfList(headRes);

    //Add new rel in joinedRels
    myint_t * joinedRels = (myint_t *) malloc((intNode->data->numOfCols + 1) * sizeof(myint_t));
    for(int whichRel = 0; whichRel < intNode->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = intNode->data->joinedRels[whichRel];
    }
    joinedRels[intNode->data->numOfCols] = addedRel;

    //Create new rowIds
    myint_t ** newRowIds = updateRowIds(intNode, headRes, numbOfResults);

    updateInter(intNode, intNode->data->numOfCols + 1, numbOfResults, joinedRels, newRowIds);
}

myint_t ** joinRowIds(nodeInter * node1, nodeInter * node2, headResult * headRes, myint_t results) {
    if(results == 0) {
        return NULL;
    }

    //Memory Allocations
    myint_t ** retArr = (myint_t **) malloc((node1->data->numOfCols + node2->data->numOfCols) * sizeof(myint_t *));
    for(myint_t i = 0; i < node1->data->numOfCols + node2->data->numOfCols; i++) {
        retArr[i] = (myint_t *) malloc(results * sizeof(myint_t));
    }

    resultNode * currentNode = headRes->firstNode;    
    myint_t counter = 0;
    for(int whichNode = 0; whichNode < headRes->numbOfNodes; whichNode++) {
        for(int whichRes = 0; whichRes < currentNode->size; whichRes++) {
            //Add the rowIds of the first Intermediate
            for(int whichCol = 0; whichCol < node1->data->numOfCols; whichCol++) {
                retArr[whichCol][counter] = node1->data->rowIds[whichCol][currentNode->tuples[whichRes].rowR];
            }

            //Add the rowIds of the second Intermediate
            for(int whichCol = node1->data->numOfCols; whichCol < node1->data->numOfCols + node2->data->numOfCols; whichCol++) {
                retArr[whichCol][counter] = node2->data->rowIds[whichCol - node1->data->numOfCols][currentNode->tuples[whichRes].rowS];
            }

            counter += 1;
        }
        currentNode = currentNode->nextNode;
    }
    return retArr;
}

//CASE: Both relationships belong to an intermediate (Inter1 will be updated, Inter2 will be deleted)
void updateInterAndDelete(headInter * headInt, nodeInter * node1, nodeInter * node2, headResult * headRes) {
    //New rows
    myint_t numbOfResults = countSizeOfList(headRes);

    myint_t * joinedRels = (myint_t *) malloc((node1->data->numOfCols + node2->data->numOfCols) * sizeof(myint_t));

    //Add joinedRels of node1 in new joinedRels
    for(int whichRel = 0; whichRel < node1->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = node1->data->joinedRels[whichRel];
    }

    //Add joinedRels of node2 in new joinedRels
    for(int whichRel = node1->data->numOfCols; whichRel < node1->data->numOfCols + node2->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = node2->data->joinedRels[whichRel - node1->data->numOfCols];
    }

    //Create new rowIds
    myint_t ** newRowIds = joinRowIds(node1, node2, headRes, numbOfResults);

    //Update Inter1
    updateInter(node1, node1->data->numOfCols + node2->data->numOfCols, numbOfResults, joinedRels, newRowIds);

    //Delete Inter2
    deleteInterNode(headInt, node2);
}

//CASE: Self join on rel that exists in an Intermediate
void updateInterSelfJoin(nodeInter * node, myint_t * joinRows, myint_t numbOfRows) {
    myint_t ** newRowIds;
    if(numbOfRows == 0) {
        newRowIds = NULL;
    }
    else {
        newRowIds = (myint_t **) malloc(node->data->numOfCols * sizeof(myint_t *));
        for(int i = 0; i < node->data->numOfCols; i++) {
            newRowIds[i] = (myint_t *) malloc(numbOfRows * sizeof(myint_t));
        }

        for(int whichRow = 0; whichRow < numbOfRows; whichRow++) {
            for(int whichCol = 0; whichCol < node->data->numOfCols; whichCol++) {
                newRowIds[whichCol][whichRow] = node->data->rowIds[whichCol][joinRows[whichRow]];
            }
        }
    }

    myint_t * joinedRels = (myint_t *) malloc(node->data->numOfCols * sizeof(myint_t));
    for(int whichRel = 0; whichRel < node->data->numOfCols; whichRel++) {
        joinedRels[whichRel] = node->data->joinedRels[whichRel];
    }
    
    updateInter(node, node->data->numOfCols, numbOfRows, joinedRels, newRowIds);
}

//CASE: Self join on rel that does NOT exist in an Intermediate
void createInterSelfJoin(headInter * head, myint_t rel, myint_t * rows, myint_t numbOfRows) {
    myint_t ** newRowIds = (myint_t **) malloc( sizeof(myint_t *));
    for(int i = 0; i < 1; i++) {
        newRowIds[i] = (myint_t *) malloc(numbOfRows * sizeof(myint_t));
    }

    for(myint_t whichRow = 0; whichRow < numbOfRows; whichRow++) {
        newRowIds[0][whichRow] = rows[whichRow];
    }

    myint_t * joinedRels = (myint_t *) malloc(sizeof(myint_t));
    joinedRels[0] = rel;

    pushInter(head, 1, numbOfRows, joinedRels, newRowIds);
}