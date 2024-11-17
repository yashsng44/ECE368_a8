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
printf("%d\n", adj_list[0].name);

while(scanf("%d %d", &source, &target) == 3) {
  djk_mul_weights(adj_list, source, target, vertices, period);
}

// free_adj_list(adj_list);

return EXIT_SUCCESS;

}
    