#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "queryStructs.h"


graph * InitialiseGraph( myint_t rels) {

    graph * newGraph = (graph *) malloc(sizeof(graph));
    newGraph->numVertices = rels;
    newGraph->graphArray = (vertex *)malloc(newGraph->numVertices * sizeof(vertex));
    for(int i = 0; i < newGraph->numVertices; i++) {
        newGraph->graphArray[i].firstEdge = NULL;
        newGraph->graphArray[i].numEdges = 0;
        newGraph->graphArray[i].vertexId = i;
    }
    return newGraph;
}

void InsertGraph(vertex * parent, myint_t parentCol, myint_t edgeRel, myint_t edgeCol, myint_t joinId) {

    if( parent->firstEdge == NULL) {        //no previous connections

        parent->firstEdge = (edge *) malloc(sizeof(edge));
        parent->numEdges = 1;
        parent->firstEdge->nextEdge = NULL;
        parent->firstEdge->parentCol = parentCol;
        parent->firstEdge->node = edgeRel;
        parent->firstEdge->nodeCol = edgeCol;
        parent->firstEdge->joinId = joinId;
        return;
    }

    parent->numEdges = parent->numEdges + 1;
    edge * temp = parent->firstEdge;
    while(temp->nextEdge != NULL) {
        temp = temp->nextEdge;
    }

    temp->nextEdge = (edge *) malloc(sizeof(edge));
    temp = temp->nextEdge;
    temp->nextEdge = NULL;
    temp->parentCol = parentCol;
    temp->node = edgeRel;
    temp->nodeCol = edgeCol;
    temp->joinId = joinId;
    return;
}

void ConstructGraph(graph * joinGraph, query * newQuery) {

    for(int i = 0; i < newQuery->numOfJoins; i++) {

        myint_t rel1 = newQuery->joins[i].participant1.rel;
        myint_t rel2 = newQuery->joins[i].participant2.rel;
        myint_t col1 = newQuery->joins[i].participant1.numCol;
        myint_t col2 = newQuery->joins[i].participant2.numCol;

        if(rel1 == rel2 && col1 != col2) {
            continue;
        }

        InsertGraph(&(joinGraph->graphArray[rel1]), col1, rel2, col2, i );     //undirected graph
        InsertGraph(&(joinGraph->graphArray[rel2]), col2, rel1, col1, i );
    }
    return;

}

void printGraph(graph * joinGraph) {

    for( int v = 0; v < joinGraph->numVertices; v++){

        fprintf(stderr,"R%ld-->" , joinGraph->graphArray[v].vertexId );
        edge * temp = joinGraph->graphArray[v].firstEdge;
        while(temp != NULL) {

            fprintf(stderr, "R%ld", temp->node);
            temp = temp->nextEdge;
        }
        fprintf(stderr, "\n");

    }
    return;

}


int connected(graph * joinGraph, myint_t vertex1, myint_t vertex2 ) {

    if(joinGraph->graphArray[vertex1].firstEdge == NULL) {
        return 0;
    }
    edge * temp = joinGraph->graphArray[vertex1].firstEdge;
    while(temp != NULL) {

        if(temp->node == vertex2) {
            return 1;
        }
        temp = temp->nextEdge;
    }
    return 0;
}

void FreeVertex(edge * head) {

    edge * temp = head;
    if (temp->nextEdge != NULL) {

        FreeVertex(temp->nextEdge);
    }
    free(temp);
    return;
}

void FreeGraph(graph * joinGraph){

    for(int i = 0; i < joinGraph->numVertices; i++) {

        if(joinGraph->graphArray[i].firstEdge != NULL) {

            FreeVertex( joinGraph->graphArray[i].firstEdge );
        }
        
    }
    free(joinGraph->graphArray);
    free(joinGraph);
    return;
}