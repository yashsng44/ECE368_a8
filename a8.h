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
    int edge_count;
    int unique_edge_count;
    int ** Edges;
} Vertex;

struct adj_Vertex * build_adjacency_list(char * filename, int * vertices, int * period);
void find_shortest_path(struct adj_Vertex * adj_list, int source, int target, int vertices, int period);
void free_adjacency_list(struct adj_Vertex *adj_list, int num_nodes);

#endif
