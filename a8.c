#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "a8.h"
#define INF 1000000

void free_adj_list (struct adj_Vertex * adj_list, int vertices, int period) {
    if (adj_list == NULL) {
        return;
    }
    
        for (int i = 0; i < vertices; i++) {
            int edges = adj_list[i].edge_count;
            for (int j = 0; j < edges; j++) {
                free(adj_list[i].Edges[j].weights);
            } 
            free(adj_list[i].Edges);
        }
        
    free(adj_list);
}
// Initialize priority queue
static PriorityQueue* create_pq(int capacity) {
    PriorityQueue* pq = malloc(sizeof(PriorityQueue));
    pq->nodes = malloc(capacity * sizeof(QueueNode));
    pq->positions = malloc(capacity * sizeof(int));
    pq->size = 0;
    pq->capacity = capacity;
    memset(pq->positions, -1, capacity * sizeof(int));
    return pq;
}

// Swap nodes and update positions
static void swap(QueueNode* a, QueueNode* b, int* positions) {
    QueueNode temp = *a;
    *a = *b;
    *b = temp;
    positions[a->vertex] = positions[b->vertex];
    positions[b->vertex] = positions[temp.vertex];
}

// Bubble up operation for heap
static void bubble_up(PriorityQueue* pq, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (pq->nodes[parent].cost > pq->nodes[idx].cost) {
            swap(&pq->nodes[parent], &pq->nodes[idx], pq->positions);
            idx = parent;
        } else break;
    }
}

// Bubble down operation for heap
static void bubble_down(PriorityQueue* pq, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < pq->size && pq->nodes[left].cost < pq->nodes[smallest].cost)
        smallest = left;
    if (right < pq->size && pq->nodes[right].cost < pq->nodes[smallest].cost)
        smallest = right;

    if (smallest != idx) {
        swap(&pq->nodes[idx], &pq->nodes[smallest], pq->positions);
        bubble_down(pq, smallest);
    }
}

// Push to priority queue
static void pq_push(PriorityQueue* pq, int vertex, int depth, int cost) {
    int pos = pq->positions[vertex];
    if (pos != -1) {
        // Update existing node if new cost is better
        if (cost < pq->nodes[pos].cost) {
            pq->nodes[pos].cost = cost;
            pq->nodes[pos].depth = depth;
            bubble_up(pq, pos);
        }
    } else {
        // Add new node
        pos = pq->size++;
        pq->nodes[pos].vertex = vertex;
        pq->nodes[pos].depth = depth;
        pq->nodes[pos].cost = cost;
        pq->positions[vertex] = pos;
        bubble_up(pq, pos);
    }
}

// Pop from priority queue
static QueueNode pq_pop(PriorityQueue* pq) {
    QueueNode min = pq->nodes[0];
    pq->positions[min.vertex] = -1;
    pq->nodes[0] = pq->nodes[--pq->size];
    if (pq->size > 0) {
        pq->positions[pq->nodes[0].vertex] = 0;
        bubble_down(pq, 0);
    }
    return min;
}

// Free priority queue
static void free_pq(PriorityQueue* pq) {
    free(pq->nodes);
    free(pq->positions);
    free(pq);
}

// Shortest path finding function
void djk_mul_weights(struct adj_Vertex* adj_list, int source, int target, int vertices, int period) {
    int* distances = malloc(vertices * sizeof(int));
    int* parents = malloc(vertices * sizeof(int));
    bool* visited = calloc(vertices, sizeof(bool));
    PriorityQueue* pq = create_pq(vertices);

    if (!distances || !parents || !visited || !pq) {
        perror("Failed to allocate resources for shortest path calculation");
        free(distances);
        free(parents);
        free(visited);
        if (pq) free_pq(pq);
        return;
    }

    // Initialize arrays
    for (int i = 0; i < vertices; i++) {
        distances[i] = INF;
        parents[i] = -1;
    }

    distances[source] = 0;
    pq_push(pq, source, 0, 0);

    while (pq->size > 0) {
        QueueNode current = pq_pop(pq);
        int u = current.vertex;

        if (visited[u]) continue;
        visited[u] = true;

        // Process edges
        struct adj_Edges* edges = adj_list[u].Edges;
        for (int i = 0; i < adj_list[u].edge_count; i++) {
            int v = edges[i].name;
            int weight_index = current.depth % period;
            int weight = edges[i].weights[weight_index];
            int new_cost = current.cost + weight;

            if (new_cost < distances[v]) {
                distances[v] = new_cost;
                parents[v] = u;
                pq_push(pq, v, current.depth + 1, new_cost);
            }
        }
    }

    // Output results
    if (distances[target] == INF) {
        printf("No path from %d to %d\n", source, target);
    } else {
        printf("Shortest path from %d to %d with total cost %d:\n", source, target, distances[target]);

        // Reconstruct path
        int* path = malloc(vertices * sizeof(int));
        if (!path) {
            perror("Failed to allocate path array");
            free(distances);
            free(parents);
            free(visited);
            free_pq(pq);
            return;
        }

        int path_len = 0;
        for (int at = target; at != -1; at = parents[at]) {
            path[path_len++] = at;
        }

        for (int i = path_len - 1; i >= 0; i--) {
            printf("%d ", path[i]);
        }
        printf("\n");

        free(path);
    }

    // Free resources
    free(distances);
    free(parents);
    free(visited);
    free_pq(pq);
}


struct adj_Vertex* build_adjacency_list(char* filename, int* vertices, int* period) {
    FILE* fol = fopen(filename, "r");
    if (!fol) {
        perror("Failed to open file");
        return NULL;
    }

    int num_weights, num_nodes;
    fscanf(fol, "%d %d", &num_nodes, &num_weights);
    *vertices = num_nodes;
    *period = num_weights;

    struct adj_Vertex* adj_list = malloc(sizeof(struct adj_Vertex) * num_nodes);
    if (!adj_list) {
        perror("Failed to allocate adjacency list");
        fclose(fol);
        return NULL;
    }

    for (int i = 0; i < num_nodes; i++) {
        adj_list[i].Edges = NULL;
        adj_list[i].edge_count = 0;
    }

    int line_size = num_weights + 2;
    int* input_buffer = malloc(sizeof(int) * line_size);
    if (!input_buffer) {
        perror("Failed to allocate input buffer");
        free(adj_list);
        fclose(fol);
        return NULL;
    }

    int* connected_nodes_count = calloc(num_nodes, sizeof(int));
    if (!connected_nodes_count) {
        perror("Failed to allocate node counts");
        free(input_buffer);
        free(adj_list);
        fclose(fol);
        return NULL;
    }

    int number, count_int = 0;
    while (fscanf(fol, "%d", &number) == 1) {
        input_buffer[count_int++] = number;
        if (count_int == line_size) {
            int src = input_buffer[0];
            int dest = input_buffer[1];

            connected_nodes_count[src]++;
            adj_list[src].Edges = realloc(adj_list[src].Edges, connected_nodes_count[src] * sizeof(struct adj_Edges));
            if (!adj_list[src].Edges) {
                perror("Failed to reallocate edges");
                free(input_buffer);
                free(connected_nodes_count);
                free_adj_list(adj_list, num_nodes, num_weights);
                fclose(fol);
                return NULL;
            }

            struct adj_Edges* new_edge = &adj_list[src].Edges[connected_nodes_count[src] - 1];
            new_edge->name = dest;
            new_edge->weights = malloc(sizeof(int) * num_weights);
            if (!new_edge->weights) {
                perror("Failed to allocate weights");
                free(input_buffer);
                free(connected_nodes_count);
                free_adj_list(adj_list, num_nodes, num_weights);
                fclose(fol);
                return NULL;
            }

            for (int i = 0; i < num_weights; i++) {
                new_edge->weights[i] = input_buffer[2 + i];
            }

            adj_list[src].edge_count++;
            count_int = 0;  // Reset input buffer count
        }
    }

    free(input_buffer);
    free(connected_nodes_count);
    fclose(fol);
    return adj_list;
}
