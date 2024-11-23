#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "a8.h"
#define INF 1000000

/*
QUESTION: why downward heapify before the while loop?
ensure that the heap property holds true for the ENTIRE PQ:
for example: after init: pq = [INF, INF, 0, INF] -> [0, INF, INF, INF] (as we want, otherwise the root node would not contain the minimum value)
*/

void downward_heapify(int *pq, int *d, int size, int target) {
    int vertices = 4;
    int temp = pq[target];       // Store the current value
    int temp_d = d[temp];        // Get corresponding distance
    int j;

    while ((j = 2 * target + 1) < size) {  // Check if the left child exists
        if (j + 1 < size && d[pq[j]] > d[pq[j + 1]]) {
            j++;  // Move to the right child if it's smaller
        }

        if (temp_d <= d[pq[j]]) {
            break;  // If current is smaller than the smallest child, stop
        }

        pq[target] = pq[j];  // Move the smaller child up
        target = j;
    }

    pq[target] = temp;  // Place the original value in its correct position
    for (int e = 0; e < size; e++) {
        printf("pq[%d] = %d ", e, pq[e]);
    }
    printf("\n");
    for (int e = 0; e < vertices; e++) {
        printf("d[%d] = %d ", e, d[e]);
    }
    printf("\n");
}


int extract_min(int * pq, int * d, int * size) {
    int min = pq[0]; // we know that the min value is always in the root, heap property
    pq[0] = pq[*size - 1]; // move to the root... the distance should also be moved.
    (*size)--; // reduce the size, i wonder why *size-- wasn't working
    downward_heapify(pq, d, *size, 0); // the target to be removed is always zero. The goal is to adjust the rest of the mess below the root.
    return min; // returns, again, the original index value of the list, , which is the root node as declared before the readjustment
}

void print_path(int *prev, int target) {
    if (prev[target] == -1) { // Base case: no predecessor
        printf("%d", target);
        return;
    }
    print_path(prev, prev[target]); // Recursive call to print the predecessor
    printf(" -> %d", target); // Print the current node
}

void djk_mul_weights(Vertex *adj_list, int source, int target, int vertices, int period) {
    int *pq = malloc(vertices * sizeof(int)); // Priority queue
    int *distance = malloc(vertices * sizeof(int)); // Distance array
    int *prev = malloc(vertices * sizeof(int)); // Previous node array
    int *depth = malloc(vertices* (sizeof(int))); // idea: maintain a depth ARRAY of each node wrt to the source.
    int size = vertices;

    // Initialize arrays
    for (int i = 0; i < vertices; i++) {
        distance[i] = INF;
        prev[i] = -1; // -1 indicates no predecessor
        pq[i] = i; // Initialize priority queue with all vertices
    }
    distance[source] = 0;

    // Build the initial heap
    for (int i = size / 2 - 1; i >= 0; i--) {
        downward_heapify(pq, distance, size, i);
    }

    // Process the priority queue 
    // i dont need to keep track of *every* path but the the order in which i take them matters what weights i pick up
    while (size > 0) {
        int u = extract_min(pq, distance, &size); // Get vertex with minimum distance
        if (u == target) break; // Stop if target is reached 

        Edges *edges = adj_list[u].Edges;
        int edge_count = adj_list[u].edge_count;
        int unique_edges_count = edge_count / period;
    
        for (int i = 0; i < edge_count; 2*i) {
            int v = edges[i].name; // Neighbor vertex
            int edge_weight = edges[i].weight; // Weight of the edge

            int alt_distance = distance[u] + edge_weight; // Calculate alternative distance
            if (alt_distance < distance[v]) {
                distance[v] = alt_distance;
                
                prev[v] = u; // Track the predecessor
                // Re-heapify the affected vertex
                for (int k = 0; k < size; k++) {
                    if (pq[k] == v) {
                        downward_heapify(pq, distance, size, k);
                        break;
                    }
                }
            }
        }
        }
    }

    // Print distances
    printf("Distances from source %d:\n", source);
    for (int i = 0; i < vertices; i++) {
        printf("Vertex %d: %d\n", i, distance[i]);
    }

    // Print the path from source to target
    printf("\nPath from %d to %d: ", source, target);
    if (distance[target] == INF) {
        printf("No path exists.\n");
    } else {
        print_path(prev, target); // Backtrack to reconstruct the path
        printf("\n");
    }

    // Free allocated memory
    free(pq);
    free(distance);
    free(prev);
}



struct adj_Vertex * build_adjacency_list(char * filename, int * vertices, int * period)
{

    FILE *fol;
    fol = fopen(filename, "r");
    // testing if the file has successfully opened
    if (fol == NULL)
    {
        return NULL;
    }

    int num_weights = 0;
    int num_nodes = 0;
    fscanf(fol, "%d %d", &num_nodes, &num_weights); // vertices and edges
    *vertices = num_nodes;
    *period = num_weights;

    struct adj_Vertex * adj_list = malloc(sizeof(struct adj_Vertex)*(num_nodes)); // adjacency list

    int number;
    int line_size = num_weights + 2;
    int count_int = 0;
    int * input_buffer = malloc(sizeof(int)*line_size);
    int * connected_nodes_count = calloc(num_nodes, sizeof(int)); // [3,4,6] indicates 3 nodes connected to node 0, 4 nodes connected to node 1, etc...

    while (fscanf(fol, "%d ", &number) == 1) {
        input_buffer[count_int] = number;
        count_int++;
        if (count_int == line_size) { // should I wait to input the values after?
            int src = input_buffer[0]; 
            int dest = input_buffer[1];

            connected_nodes_count[src]++; // increment the count of connected nodes pairs

            if (connected_nodes_count[input_buffer[0]] == 1) {
            adj_list[src].name = input_buffer[0]; // initialize the list name with the source, shouldn't matter but it doesn't hurt 
            }

            adj_list[src].Edges = realloc(
                adj_list[src].Edges, (connected_nodes_count[src]*num_weights) * sizeof(struct adj_Edges)
                ); 

            int new_count =  adj_list[src].edge_count + num_weights; // increase the count of edges connected to the node by 
            int k = 0;

            for (int i = adj_list[src].edge_count; i < new_count; i++) {
                adj_list[src].Edges[i].name = dest;
                adj_list[src].Edges[i].weight = input_buffer[k+2];
                k++;
            }

            // all the same edges anyway, name don't matter. First: sort by the name in an ascending order. Then, sort by the weights in an ascending order.
            // so <0,1, 1> , <0,1,2>, <0,2,1>...etc
            
            adj_list[src].edge_count = new_count; // increase the count of edges connected to the node by 
            count_int = 0; // reset this hoe
        }
    }

    free(input_buffer);
    free(connected_nodes_count);
    fclose(fol);
    return adj_list;
}
