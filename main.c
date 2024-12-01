#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include <math.h>
#include "a8.h" // Ensure this header file is correct and included in your project


int main(int argc, char **argv) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }

    int V, N, source, target;

    struct adj_Vertex *adj_list = build_adjacency_list(argv[1], &V, &N); // Build the graph

    while(scanf("\n%d %d", &source, &target) == 2) {
        if (target > (V - 1) || target < 0 || source > (V - 1) || source < 0) {
            break;
        }
        find_shortest_path(adj_list, source, target, V, N);
    }


    free_adjacency_list(adj_list, V);
    return EXIT_SUCCESS;
}
