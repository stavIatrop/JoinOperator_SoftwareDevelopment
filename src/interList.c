#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "queryStructs.h"

inter * initialiseInter(myint_t cols, myint_t * joinedRels, myint_t ** rowIds) {
    inter * retInter = (inter *) malloc(sizeof(inter));
    retInter->joinedRels = joinedRels;
    retInter->numOfCols = cols;
    retInter->rowIds = rowIds;

    return retInter;
}

nodeInter * initialiseNode(myint_t cols, myint_t * joinedRels, myint_t ** rowIds) {
    nodeInter * node = (nodeInter *) malloc(sizeof(nodeInter));
    node->data = initialiseInter(cols, joinedRels, rowIds);
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
    freeNodeListRec(head->start);
    free(head);

}

void pushInter(headInter * head ,myint_t cols, myint_t * joinedRels, myint_t ** rowIds) {
    if(head->numOfIntermediates == 0) {
        head->start = initialiseNode(cols, joinedRels, rowIds);
    }
    else {
        nodeInter * finalNode = head->start;
        for(int whichNode = 0; whichNode < head->numOfIntermediates - 1; whichNode++) {
            finalNode = finalNode->next;
        }
        finalNode->next = initialiseNode(cols, joinedRels, rowIds);
    }
    head->numOfIntermediates += 1;
}

void refreshInter(nodeInter * node, myint_t cols, myint_t * joinedRels, myint_t ** rowIds) {
    node->data->joinedRels = joinedRels;
    node->data->numOfCols = cols;
    node->data->rowIds = rowIds;
}

void deleteInternode(headInter * head, nodeInter * node) {
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