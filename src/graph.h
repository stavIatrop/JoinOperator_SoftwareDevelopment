#ifndef GRAPH_H
#define GRAPH_H

#include "basicStructs.h"
#include "queryStructs.h"

typedef struct EdgeStruct {

    myint_t parentCol;
    myint_t nodeCol;
    myint_t node;
    myint_t joinId;
    struct EdgeStruct *nextEdge;

}edge;

typedef struct VertexStruct {

    myint_t vertexId;
    myint_t numEdges;
    edge * firstEdge;

}vertex;

typedef struct GraphStruct {

    vertex * graphArray;
    myint_t numVertices;

}graph;

graph * InitialiseGraph(myint_t );
void InsertGraph(vertex *, myint_t, myint_t, myint_t, myint_t);
void printGraph(graph * );
void ConstructGraph(graph *, query *);
int Connected(graph *, myint_t , myint_t, myint_t *, myint_t *, myint_t *  );
void FreeVertex(edge * );
void FreeGraph(graph *);

#endif