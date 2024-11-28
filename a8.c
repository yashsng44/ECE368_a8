#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "a8.h"

#define INF 1000000
int source, target, N, V;

void downward_heapify(int *pq, int **d, int size, int ref, int time) {
    if (size < 1) {
        return;
    }

    int temp = pq[ref];
    int temp_d = d[temp][time];
    int j;

    while ((j = 2 * ref + 1) < size) {
        if (j + 1 < size && d[pq[j]][time] > d[pq[j + 1]][time]) {
            j++;
        }
        if (temp_d <= d[pq[j]][time]) {
            break;
        }
        pq[ref] = pq[j];
        ref = j;
    }

    pq[ref] = temp;
}

void enQueue(int vertex, int time, int *pq, int *size, int **d) {
    pq[(*size)++] = vertex;
    downward_heapify(pq, d, *size, 0, time);
}

int extract_min(int *pq, int **d, int *size, int time) {
    if (*size <= 0) {
        return -1;
    }
    int min = pq[0]; // get root element
    pq[0] = pq[--(*size)]; // move it to the "back" of the pq; pq[size] 
    downward_heapify(pq, d, *size, 0, time); // call heapify... with the new size
    return min; // return the minimum
}

void find_shortest_path(struct adj_Vertex *adj_list) {
    if (source == target) {
        printf("Shortest distance from %d to %d is: 0\n", source, target);
        return;
    }

    // Priority queue and distance matrix
    int *pq = malloc(V * sizeof(int));    // Priority queue
    int **d = malloc(V * sizeof(int*));  // Distance matrix, V = number of rows = vertices.
    int size = 0;

    // Initialize distances
    for (int i = 0; i < V; i++) {
        d[i] = malloc(N * sizeof(int));
        for (int j = 0; j < N + 1; j++) { // intialize N + 1 columns, where the first one is reserved for the source variable.
            d[i][j] = INF;  // Initialize all distances to infinity
        }
    }

    d[source][0] = 0; // enqueue the distance at time 0
    pq[size++] = source;  // Enqueue the source, increase the queue size
    int time = 0; //initialize the time...
    while (size > -1) {
        // printf("Distance matrix (d):\n");
        // for (int i = 0; i < V; i++) {
        //     printf("Vertex %d: ", i);
        //     for (int j = 0; j < N + 1; j++) {
        //         if (d[i][j] == INF) {
        //             printf("INF ");  // Print 'INF' for infinity
        //         } else {
        //             printf("%d ", d[i][j]);  // Print the actual distance
        //         }
        //     }
        //     printf("\n");  // Newline after each vertex
        // }

        int u = extract_min(pq, d, &size, time); // get pq, d, size, 0, 0);

        int mod = time % N; // use the outter time tracker to check where we are externally.
        int current_time = d[u][mod] + 1; // intial check, d[u][0], where the 0th column is reserved for the source array, + 1 = 1 d[u][1]
        struct adj_Edges *edges = adj_list[u].Edges; // get the edges...
        
        for (int i = 0; i < adj_list[u].edge_count; i++) { // for all the edges....
            int v = edges[i].name; // get the name
            int offset = v / 2;
            int weight = edges[offset + current_time - 1].weights;  // edges[1].weights at the current time..
            time = current_time; // add the most recent time back to the outter counter 
            int next_time;
            if (time == N) {
                next_time = time; // i want to maintain the said next_time
            } else {
                next_time = (current_time + 1) % N;    // 2 % 2 = 0
            }
            int new_dist = d[u][current_time] + weight; // new distance d[u][]

            // Relaxation: Update distance if a shorter path is found
            if (new_dist < d[v][next_time]) {
                d[v][next_time] = new_dist;
                pq[size++] = v;  // Enqueue the neighbor
                downward_heapify(pq, d, size, 0, next_time);  // Restore heap property
            }
        }
    }

    // Find the shortest path to the target across all time steps
    int min_distance = INF;
    for (int t = 0; t < N; t++) {
        if (d[target][t] < min_distance) {
            min_distance = d[target][t];
        }
    }

    if (min_distance == INF) {
        printf("No path from %d to %d\n", source, target);
    } else {
        printf("Shortest path distance from %d to %d is: %d\n", source, target, min_distance);
    }

    // Free memory
    for (int i = 0; i < V; i++) {
        free(d[i]);
    }
    free(d);
    free(pq);
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
            adj_list[src].Edges = realloc(adj_list[src].Edges, (connected_nodes_count[src] * num_weights) * sizeof(struct adj_Edges));
            adj_list[src].name = src;

            int new_count = adj_list[src].edge_count + num_weights;
            int k = 0;

            for (int i = adj_list[src].edge_count; i < new_count; i++) {
                adj_list[src].Edges[i].name = dest;
                adj_list[src].Edges[i].weights = input_buffer[k + 2];
                k++;
            }

            adj_list[src].edge_count = new_count;
            adj_list[src].unique_edge_count = new_count / N;
            count_int = 0;
        }
    }

    free(input_buffer);
    free(connected_nodes_count);
    fclose(file);
    return adj_list;
}
