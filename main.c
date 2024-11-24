#include <stdio.h>  
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h>
#include <math.h>
#include "a8.h"

int main(int argc, char ** argv) {

if (argc != 2)
    {
      return EXIT_FAILURE;
}

int vertices, period, source, target = 0;
struct adj_Vertex * adj_list = build_adjacency_list(argv[1], &vertices, &period); // build the graph

while(scanf("\n%d %d", &source, &target) == 2) {
  djk_mul_weights(adj_list, source, target, vertices, period);
}

free_adj_list(adj_list, vertices, period);
return EXIT_SUCCESS;

}
    
