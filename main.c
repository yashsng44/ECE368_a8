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

build_adjacency_list(argv[1]);
}
    