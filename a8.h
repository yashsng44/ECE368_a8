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

// typedef struct Queue {
//     Node * front, *rear;
// } 

// typedef struct Node {
//     int name;
//     struct Node * next;
// }

struct adj_Vertex * build_adjacency_list(char * filename, int * vertices, int * period);
void djk_mul_weights(struct adj_Vertex * adj_list, int source, int target, int vertices, int period);
#endif