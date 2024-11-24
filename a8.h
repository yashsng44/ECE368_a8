#ifndef a8_h
#define a8_h 
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include <math.h>

// adjacency list nodes 
typedef struct adj_Vertex {
    int name;
    int edge_count;
    struct adj_Edges * Edges;
} Vertex;

typedef struct adj_Edges {
    int name;
    int * weights;
} Edges;

typedef struct {
    int vertex;
    int depth;  // Track depth explicitly for weight cycles
    int cost;
} QueueNode;

typedef struct {
    QueueNode* nodes;
    int* positions;  // Track positions in heap for faster updates
    int size;
    int capacity;
} PriorityQueue;

struct adj_Vertex * build_adjacency_list(char * filename, int * vertices, int * period);
void djk_mul_weights(struct adj_Vertex * adj_list, int source, int target, int vertices, int period);
void free_adj_list(struct adj_Vertex * adj_list, int vertices, int period);
#endif
