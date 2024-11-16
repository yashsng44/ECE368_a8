#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "a8.h"

void build_adjacency_list(char *filename)
{

    // the input in the text file will be as such:
    /*
    4 2, V, N, he period of the edge weights 
    0 1 5 1
    1 2 6 2
    0 2 3 4
    2 3 4 15

    The adjacency list will look like:
    adj[0] = [(1,5,1),(2,3,4)]
    adj[1] = [(2,6,2)]
    adj[2] = [(3,4,15)]
    */

    FILE *fol;
    fol = fopen(filename, "r");
    // testing if the file has successfully opened
    if (fol == NULL)
    {
        return;
    }

    int num_weights = 0;
    int num_nodes = 0;
    fscanf(fol, "%d %d", &num_nodes, &num_weights); // vertices and edges

    // create my adj_list
    struct adj_Vertex * adj_list = malloc(sizeof(struct adj_Vertex)*num_nodes);
    //for (int i = 0; i < num_nodes; i++) {
    //     adj_list[i].Edges = malloc(sizeof(struct adj_Edges)*num_nodes); // so dont assume the number of inputs..reallaoc?
    //     for(int j = 0; j < num_weights; j++) { 
    //         adj_list[i].Edges[j].weights = malloc(sizeof(int)*num_weights); // add the weights dependant on the previous alloc?
    //     }
    // }

    int number;
    int line_size = num_weights + 2;
    int count_int = 0;
    int count_line = 0;
    int * input_buffer = malloc(sizeof(int)*line_size);

    while (fscanf(fol, "%d ", &number) == 1) {
        if(count_int == line_size) { // should I wait to input the values after?
            for(int k = 2; k < line_size; k++) {
            adj_list->name = input_buffer[0];
            printf("name: %d\n", adj_list->name);
            adj_list->Edges->name = input_buffer[1]; // updates it to the same location, so edges [0].. no new values..,, use reallaoc?
            printf("connected to: %d\n", adj_list->Edgename);
            adj_list[input_buffer[0]].Edges[input_buffer[1]].weights[k-2] = input_buffer[k];
            }
            count_line++;
            count_int = 0;
        } else {
            input_buffer[count_int] = number;
        }
    count_int++;
    }

    fclose(fol);
    return;
}
