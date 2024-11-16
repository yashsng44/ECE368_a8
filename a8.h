#ifndef a6_h
#define a6_h 
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include <math.h>

// adjacency list nodes 
typedef struct adj_Vertex {
    int name;
    struct adj_Edges * Edges;
} Vertex;

typedef struct adj_Edges {
    int name;
    int * weights;
} Edges;

void build_adjacency_list(char * filename);

#endif