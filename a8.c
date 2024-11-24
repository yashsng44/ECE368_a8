#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "a8.h"
#define INF 2147483647

// int isEmpty(Queue* q)
// {

//     // If the front and rear are null, then the queue is
//     // empty, otherwise it's not
//     if (q->front == NULL && q->rear == NULL) {
//         return 1;
//     }
//     return 0;
// }

// void make_node (int data) {
//     Node * new_node = malloc(sizeof(Node));
//     new_node->name = new_data;
//     new_node->next = NULL;
//     return new_node;
// }

// void enQ (struct Queue * q, int new_value) {
//     Node * new = make_node(new_value);
//     if(q->rear = NULL) {
//         q->front = q->rear = new_node;
//         return;
//     }
//     q->rear->next = new_node;
//     q->rear = new_node;
// }

// int deQ (struct Queue * q) {
//     // If queue is empty, return
//     if (isEmpty(q)) {
//         printf("Queue Underflow\n");
//         return;
//     }

//     // Store previous front and move front one node
//     // ahead
//     Node* temp = q->front;
//     int val = temp->name;
//     q->front = q->front->next;

//     // If front becomes null, then change rear also
//     // to null
//     if (q->front == NULL)
//         q->rear = NULL;

//     // Deallocate memory of the old front node
//     free(temp);
//     return val;
// }

// struct Queue * createQueue()
// {
//     Queue* q = (Queue*)malloc(sizeof(Queue));
//     q->front = q->rear = NULL;
//     return q;
// }
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

    pq[target] = temp;
    for (int l = 0; l < vertices; l++) {
        printf("%d, ", pq[l]);
    }
    printf("\n");

}


int extract_min(int * pq, int * d, int * size) {
    int min = pq[0]; // we know that the min value is always in the root, heap property
    pq[0] = pq[*size - 1]; // move to the root... the distance should also be moved.
    pq[*size - 1] = min;
    (*size)--; // reduce the size, i wonder why *size-- wasn't working
    downward_heapify(pq, d, *size, 0); // the target to be removed is always zero. The goal is to adjust the rest of the mess below the root.
    return min; // returns, again, the original index value of the list, , which is the root node as declared before the readjustment
}

void upward_heapify(int *pq, int *d, int size, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (d[pq[parent]] <= d[pq[index]]) {
            break;
        }

        // Swap parent and child
        int temp = pq[parent];
        pq[parent] = pq[index];
        pq[index] = temp;

        index = parent;
    }
}


void djk_mul_weights(struct adj_Vertex *adj_list, int source, int target, int vertices, int period) {
    int *pq = malloc(vertices * sizeof(int));
    int *distance = malloc(vertices * sizeof(int));
    int *prev = malloc(vertices * sizeof(int));
    int *depth = malloc(vertices * sizeof(int));
    int source_edges = adj_list[source].edge_count;
    int best_distance = INF;
    int *best_prev = malloc(vertices * sizeof(int));
    int size;

    // Try each edge from source as the starting edge
    for (int start_edge = 0; start_edge < source_edges; start_edge++) {
        // Reset arrays for this iteration
    
        size = vertices;
        for (int i = 0; i < vertices; i++) {
            distance[i] = INF;
            prev[i] = -1;
            pq[i] = i;
            depth[i] = 0;
        }
        
        // Set source distance to 0
        distance[source] = 0;
        
        struct adj_Edges * curr_edge = &adj_list[source].Edges[start_edge]; // taking the address
        int first_neighbor = curr_edge->name;
        distance[first_neighbor] = curr_edge->weights[0];
        prev[first_neighbor] = source;
        depth[first_neighbor] = 1;

        // Build initial heap
        for (int i = vertices / 2 - 1; i >= 0; i--) {
            downward_heapify(pq, distance, size, i);
        }

        while (size > 0) {
            int u = extract_min(pq, distance, &size);
            
            if (u == target) {
                if (distance[target] < best_distance) {
                    best_distance = distance[target];
                    memcpy(best_prev, prev, vertices * sizeof(int));
                }
                break;
            }

            // Process each edge from current vertex
            for (int i = 0; i < adj_list[u].edge_count; i++) {
                struct adj_Edges *edge = &adj_list[u].Edges[i];
                int v = edge->name;
                
                // Select weight based on current path depth
                int current_depth = depth[u];
                int weight_index = current_depth % period;
                int weight = edge->weights[weight_index];
                
                int new_distance = distance[u] + weight;

                // If we found a better path
                if (new_distance < distance[v]) {
                    distance[v] = new_distance;
                    prev[v] = u;
                    depth[v] = depth[u] + 1;

                    // Update position in priority queue
                    for (int j = 0; j < size; j++) {
                        if (pq[j] == v) {
                            upward_heapify(pq, distance, size, j);
                            break;
                        }
                    }
                }
            }
        }

        // Debug output for current iteration
        printf("\nTrying source edge %d (source->%d):\n", start_edge, first_neighbor);
        if (distance[target] != INF) {
            printf("Found path with distance: %d\n", distance[target]);
            printf("Path: ");
            // Print path in correct order (source to target)
            int * path = malloc(vertices*sizeof(int));
            int path_len = 0;
            for (int v = target; v != -1; v = prev[v]) {
                path[path_len++] = v;
            }
            for (int i = path_len - 1; i >= 0; i--) {
                printf("%d ", path[i]);
            }
            printf("\n");
        }
    }

    // Print final best path
    if (best_distance == INF) {
        printf("\nNo path exists from %d to %d\n", source, target);
    } else {
        printf("\nBest path found with distance %d\n", best_distance);
        printf("Path: ");
        // Print best path in correct order
        int * path = malloc(vertices*sizeof(int));
        int path_len = 0;
        for (int v = target; v != -1; v = best_prev[v]) {
            path[path_len++] = v;
        }
        for (int i = path_len - 1; i >= 0; i--) {
            printf("%d ", path[i]);
        }
        printf("\n");
    }

    free(pq);
    free(distance);
    free(prev);
    free(depth);
    free(best_prev);
}



struct adj_Vertex * build_adjacency_list(char * filename, int * vertices, int * period)
{

    // the input in the text file will be as such:
    /*
    4 2, V, N, he period of the edge weights 
    0 1 5 1
    1 2 6 2
    0 2 3 4
    2 3 4 15

    The adjacency list will look like:
    adj[0] = [1: (5,1), 2: (3,4)] // each vertex. edge, and weight will have their own array 
    .
    .
    .
    */

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

    // create my adj_list
    struct adj_Vertex * adj_list = malloc(sizeof(struct adj_Vertex)*num_nodes);

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

            connected_nodes_count[src]++; // increment the count of connected nodes 

            if (connected_nodes_count[input_buffer[0]] == 1) {
            adj_list[src].name = input_buffer[0]; // initialize the list name with the source, shouldn't matter but it doesn't hurt 
            }

            adj_list[src].Edges = realloc(
                adj_list[src].Edges, connected_nodes_count[src] * sizeof(struct adj_Edges)
                );// reallocate with more edges

            adj_list[src].edge_count++;

            struct adj_Edges * new_edge = &adj_list[src].Edges[connected_nodes_count[src] - 1];
            new_edge->name = dest;
            new_edge->weights = malloc(num_weights * sizeof(int));

            for (int i = 0; i < num_weights; i++) {
                new_edge->weights[i] = input_buffer[i+2];
            }

            count_int = 0; // reset this hoe
        }
    }

    free(input_buffer);
    free(connected_nodes_count);
    fclose(fol);
    return adj_list;
}
