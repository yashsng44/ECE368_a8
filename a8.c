#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "a8.h"

#define INF 1000000
int source, target, N, V;

void downward_heapify(int **pq, int **d, int size, int ref, int time) {
    if (size < 1) {
        return;
    }

    int *temp = pq[ref];
    int temp_d = d[temp[0]][time];
    int j;

    while ((j = 2 * ref + 1) < size) {
        if (j + 1 < size && d[pq[j][0]][time] > d[pq[j + 1][0]][time]) {
            j++;
        }
        if (temp_d <= d[pq[j][0]][time]) {
            break;
        }
        pq[ref] = pq[j];
        ref = j;
    }

    pq[ref] = temp;
}

void enQueue(int vertex, int weight, int **pq, int *size, int **d) {
    pq[*size] = malloc(2 * sizeof(int));
    pq[*size][0] = vertex;
    pq[*size][1] = weight;
    (*size)++;
    downward_heapify(pq, d, *size, 0, weight);
}

int *extract_min(int **pq, int **d, int *size, int time) {
    if (*size <= 0) {
        return NULL;
    }
    int *min = pq[0];
    pq[0] = pq[--(*size)]; // pre-decrement the size? so pq[0] = pq[0]?, do I even care about keeping track of this....tbh...
    downward_heapify(pq, d, *size, 0, time); // okay yeah downard heapify, move the value with the smallest distance up the list (?) to be evaluated first (regardless they will be)
    return min;
}

void print_priority_queue(int **pq, int size) {
    printf("Priority Queue (size: %d): ", size);
    for (int i = 0; i < size; i++) {
        printf("(%d, %d) ", pq[i][0], pq[i][1]);
    }
    printf("\n");
}

void find_shortest_path(struct adj_Vertex *adj_list) {
    if (source == target) {
        printf("Shortest distance from %d to %d is: 0\nPath: %d\n", source, target, source);
        return;
    }

    // Priority queue and distance matrix
    int **pq = malloc(V * sizeof(int *));  // Priority queue
    int **d = malloc(V * sizeof(int *));  // Distance matrix
    int *parent = malloc(V * sizeof(int));  // Parent array
    int size = 0;

    // Initialize distances and parent array
    for (int i = 0; i < V; i++) {
        d[i] = malloc((N + 1) * sizeof(int));
        for (int j = 0; j <= N; j++) {
            d[i][j] = INF;  // Initialize all distances to infinity
        }
        parent[i] = -1;  // Initialize parent array to -1
    }

    d[source][0] = 0;  // Distance to source at time 0
    enQueue(source, 0, pq, &size, d);  // Enqueue the source with initial weight 0 
    // nothing to heap just yet, I only added one thing to my priority queue.

    while (size > 0) {
        // get the first value off the pq
        int *u = extract_min(pq, d, &size, 0);  // Extract min from pr
        int curr_vertex = u[0];
        int curr_weight = u[1];
        free(u);

        printf("Dequeued vertex: (%d, %d)\n", curr_vertex, curr_weight);

        int time_step = curr_weight % N;  // Current time step modulo the period
        // Process all outgoing edges
        for (int edge_idx = 0; edge_idx < adj_list[curr_vertex].unique_edge_count; edge_idx++) {
            int edge_offset = time_step + edge_idx;
            int v = adj_list[curr_vertex].Edges[edge_offset][0];  // Destination vertex
            int weight = adj_list[curr_vertex].Edges[edge_offset][1];  // Edge weight

            printf("Processing edge %d -> %d (weight: %d, time_step: %d)\n", curr_vertex, v, weight, time_step);

            int next_time = (time_step + 1) % N;  // Calculate next time step
            int new_dist = d[curr_vertex][time_step] + weight;

            if (new_dist < d[v][next_time]) {
                printf("Updating distance of %d at time %d: %d -> %d\n", v, next_time, d[v][next_time], new_dist);
                d[v][next_time] = new_dist;
                parent[v] = curr_vertex;
                enQueue(v, new_dist, pq, &size, d);  // Enqueue the updated vertex
            }
        }
    }

    // Find the shortest path to the target across all time steps
    int min_distance = INF;

    for (int t = 0; t <= N; t++) {
        if (d[target][t] < min_distance) {
            min_distance = d[target][t];
        }
    }

    if (min_distance == INF) {
        printf("No path from %d to %d\n", source, target);
    } else {
        printf("Shortest path distance from %d to %d is: %d\n", source, target, min_distance);

        // Reconstruct and print the path
        printf("Path: ");
        int node = target;
        while (node != -1) {
            printf("%d ", node);
            node = parent[node];
        }
        printf("\n");
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
            adj_list[src].Edges = realloc(adj_list[src].Edges, (total_edges) * sizeof(int));
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
