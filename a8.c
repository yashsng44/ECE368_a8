#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "a8.h"

#define INF 1000000

void free_adjacency_list(struct adj_Vertex *adj_list, int num_nodes) {
    for (int i = 0; i < num_nodes; i++) {
        if (adj_list[i].Edges != NULL) {
            for (int j = 0; j < adj_list[i].edge_count; j++) {
                free(adj_list[i].Edges[j]);
            }
            free(adj_list[i].Edges);
        }
    }
    free(adj_list);
}

void downward_heapify(int **pq, int size, int ref) {
    if (size < 1) {
        return;
    }

    int *temp = pq[ref];
    int temp_weight = temp[2]; // Compare based on weight
    int j;

    while ((j = 2 * ref + 1) < size) {
        if (j + 1 < size && pq[j][2] > pq[j + 1][2]) {
            j++;
        }
        if (temp_weight <= pq[j][2]) {
            break;
        }
        pq[ref] = pq[j];
        ref = j;
    }

    pq[ref] = temp;
}

void enQueue(int vertex, int weight, int time_step, int **pq, int *size) {
    pq[*size] = malloc(3 * sizeof(int)); // Now holds 3 values
    pq[*size][0] = vertex;
    pq[*size][1] = time_step;
    pq[*size][2] = weight;
    (*size)++;
}

void print_priority_queue(int **pq, int size) {
    // printf("Priority Queue (size: %d): ", size);
    for (int i = 0; i < size; i++) {
        // printf("(%d, time_step: %d, weight: %d) ", pq[i][0], pq[i][1], pq[i][2]);
    }
    // printf("\n");
}


void print_distance_array(int **d, int rows, int cols) {
    // printf("Distance Array:\n");
    for (int i = 0; i < rows; i++) {
        printf("Vertex %d: ", i);
        for (int j = 0; j < cols; j++) {
            if (d[i][j] == INF) {
                printf("INF ");
            } else {
                printf("%d ", d[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void print_visited_array(int *visited, int size) {
    printf("Visited Array:\n");
    for (int i = 0; i < size; i++) {
        printf("Vertex %d: %d\n", i, visited[i]); // Print the value for each vertex
    }
}

void find_shortest_path(struct adj_Vertex *adj_list, int source, int target, int V, int N) {
    if (source == target) {
        printf("%d %d\n", source, target);
        return;
    }

    // Priority queue and distance matrixs
    int **pq = malloc((V *N) * sizeof(int *));  // Priority queue
    int **d = malloc(V * sizeof(int *));  // Distance matrix
    int *parent = malloc(V * sizeof(int));  // Parent array
    int *visited = calloc(V, sizeof(int));  // Visited array
    int size = 0;

    // Initialize distances and parent array
    for (int i = 0; i < V; i++) {
        d[i] = malloc((N + 1) * sizeof(int));
        for (int j = 0; j < N + 1; j++) {
            d[i][j] = INF;  // Initialize all distances to infinity
        }
        parent[i] = -1;  // Initialize parent array to -1
    }

    d[source][0] = 0;  // Distance to source at time 0
    // nothing to heap just yet, I only added one thing to my priority queue.
    int time_step = 0; // initialize the current time step outside of the loop
    enQueue(source, 0, time_step, pq, &size);  // Enqueue the source with initial weight 0 
    
    while (size > 0) {
        // get the first value off the pq
        int * u = pq[0];
        pq[0] = pq[--(size)]; // decrease the size of the array, as we dequeued something
        //downward_heapify(pq, size, 0);
        int curr_vertex = u[0];
        int curr_time_step = u[1];
        int curr_weight = u[2];
        free(u);

       // // printf("Dequeued vertex: (%d, %d)\n", curr_vertex, curr_weight);
        if (curr_weight > d[curr_vertex][curr_time_step]) continue;)  { // if the current distance 
        //printf("skip: %d, %d\n", curr_vertex, visited[curr_vertex]);
        continue;
        } 
        visited[curr_vertex]++;
        print_visited_array(visited, V);
        if (visited[curr_vertex] > V) {
            printf("Cycle detected involving vertex %d\n", curr_vertex);
            continue; // Allow exploration of other paths despite the cycle
        }
        
        if (curr_vertex == target) {
        int node = target;
        int * reverse = malloc(V * N * sizeof(int));
        int top = -1;
        while (node != -1) {
            reverse[++top] = node;
            node = parent[node];
        }
        while(top >= 0) {
            printf("%d ", reverse[top--]);
        }
        printf("\n");
        free(reverse);
        break;
        }
        // enqueue connected edges corresponding to the time component
        int mod_time = curr_time_step % N;  // Current time step modulo the periode
        // printf("run from %d to %d \n", mod_time, adj_list[curr_vertex].unique_edge_count);

        for (int edge_idx = mod_time; edge_idx < adj_list[curr_vertex].edge_count; edge_idx = edge_idx + N) {
            int v = adj_list[curr_vertex].Edges[edge_idx][0];  // vertex
            if (d[v][curr_time_step] != INF) {
            continue; // Skip if already visited
            }
            int weight = adj_list[curr_vertex].Edges[edge_idx][1];  // Edge weight
            // printf("Adding edge %d -> %d (weight: %d, time_step: %d)\n", curr_vertex, v, weight, time_step);
            //update the corresponding distances from the source node

            int new_weight = d[curr_vertex][curr_time_step] + weight; // or the existing one?

            // printf("evaluated: %d < %d ?\n", new_weight, d[v][mod_time + 1]);
            if (!visited[v] && new_weight < d[v][mod_time + 1]) { 
            d[v][mod_time + 1] = new_weight; // it is simply at *that* time
            parent[v] = curr_vertex;
            enQueue(v, new_weight, mod_time + 1, pq, &size); // enqueue the weights
            }
        
        }
        
       //print_distance_array(d, V, N + 1);
        for (int k = (size/2) - 1; k >= 0; k--) {
        downward_heapify(pq, size, k); // heap with respect to this...
        }

        //print_priority_queue(pq, size);
        curr_time_step++;
    }

      // Free memory
    for (int i = 0; i < V; i++) {
        free(d[i]);
    }
    free(d);
    for (int i = 0; i < size; i++) {
        free(pq[i]);
    }
    free(pq);
    free(parent);
    free(visited);
}


struct adj_Vertex* build_adjacency_list(char* filename, int* vertices, int* period) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    int num_weights, num_nodes;
    fscanf(file, "%d %d", &num_nodes, &num_weights);
    *vertices = num_nodes;
    *period = num_weights;

    struct adj_Vertex* adj_list = malloc(sizeof(struct adj_Vertex) * num_nodes);

    if (!adj_list) {
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < num_nodes; i++) {
        adj_list[i].Edges = NULL;
        adj_list[i].edge_count = 0;
    }

    int line_size = num_weights + 2;
    int* input_buffer = malloc(sizeof(int) * line_size);
    int* connected_nodes_count = calloc(num_nodes, sizeof(int));
    int number, count_int = 0;

    while (fscanf(file, "%d", &number) == 1) {
        input_buffer[count_int++] = number;
        if (count_int == line_size) {
            int src = input_buffer[0];
            int dest = input_buffer[1];

            connected_nodes_count[src]++;
            int total_edges = connected_nodes_count[src] * num_weights;
            adj_list[src].Edges = realloc(adj_list[src].Edges, (total_edges) * sizeof(int *));
            if (adj_list[src].Edges == NULL) {
                // Handle memory allocation failure
                fprintf(stderr, "Memory reallocation failed.\n");
                    free(input_buffer);
                    free(connected_nodes_count);
                    fclose(file);
                exit(EXIT_FAILURE);
            }
            adj_list[src].name = src;

            int k = 0;
            for (int i = (connected_nodes_count[src] - 1)*num_weights; i < (total_edges); i++) {
                adj_list[src].Edges[i] = malloc(2*sizeof(int));
                adj_list[src].Edges[i][0] = dest;
                adj_list[src].Edges[i][1] = input_buffer[k + 2];
                k++;
            }

            adj_list[src].edge_count = total_edges;
            adj_list[src].unique_edge_count = total_edges / num_weights;
            count_int = 0;
        }
    }

    free(input_buffer);
    free(connected_nodes_count);
    fclose(file);
    return adj_list;
}
