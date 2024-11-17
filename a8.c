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


void downward_heapify(int * pq, int size, int target) { // size in this contex is that of the array
    /// everytime we are getting rid of the root node, i.e. shortest targeta
    int temp = pq[target];  // target refers to the node of ineterest
    int j;

    while((j = 2*target + 1 < size)) { // checking the indice to the left child                                    // if they are within array bounds, that is
        if (j < size && pq[j] > pq[j+1]) {// if the variable is smalll and smaller than its right sibling
        j++;
        }
        if (temp <= pq[j]) {// checking the target, if its smaller than the smallest child
            break;
        }
        pq[target] = pq[j]; // perform the swap between the elements..
        target = j;
    }

    pq[target] = temp; 
}

int extract_min(int * pq, int * size) {
    int min = pq[0]; // we know that the min value is always in the root, heap property
    pq[0] = pq[*size - 1]; // move to the root....
    *size--; // reduce the size
    downward_heapify(pq, *size, 0); // the target to be removed is always zero. The goal is to adjust the rest of the mess below the root.
    return min; // returns, again, the original index value of the list, , which is the root node as declared before the readjustment
}

void djk_mul_weights(struct adj_Vertex * adj_list, int source, int target, int vertices, int period) {

    //intializing the arrays to use
    int * pq = calloc(vertices, sizeof(int)); // intialize everything with 0
    int * distance = malloc(vertices*sizeof(int)); // shortest current index 
    int * prev = malloc(vertices*sizeof(int));
    // memset(prev, NULL, vertices*sizeof(int));
    for (int m = 0; m < sizeof(distance); m++) {
        distance[m] = 2147483646;
    }
    //memset(distance, 2147483646, vertices*sizeof(int)); // initialize the distance to the maximum integer signed value 
    int size = vertices;
    distance[source] = 0; // initalize the distance from the source to be the smallest, set to 0...
    downward_heapify(pq, size, source); // perform downward heapify for initialization, brings the node assigned as the source, distance 0, to the top.
    
    // assigment specific 
    int track_depth = 0;
    int d_prev_target_match = INF; // tracks the last distance value when there is a match from the source -> target
    int u = 0;

    while (sizeof(pq) > 0) { // equivalent of not being empty...
    u = extract_min(pq, &size);

    int size_of_u = sizeof(adj_list[u].Edges);
    // everytime  I readjust my priority queue, I'm somewhere deeper in my graph

    // WHEN IT IS THE TARGET!
    // u contains the name of the node of interest, at pq[0]. it is NOT 0.
    for (int i = 0; i < size_of_u; i++) { // we dont check the node here. 
        int v = adj_list[u].Edges[i].name; // redefine the vertex it is connected to

        int weight_index = track_depth % period;
        int d_curr_source = distance[u]; // this is the index we want? who gaf
        int d_curr_target = distance[v];
        int curr_weight = adj_list[u].Edges[v].weights[weight_index];
        int d_st = curr_weight + d_curr_source; 
 
        if ((v >= 0 && v <= (vertices - 1))&&(d_curr_target > d_st)) { // just has to be within the range of values..it seems like they are ordered!
            d_curr_target = d_st;
            prev[v] = u;
        }
    }
    }
    
    track_depth++;
    printf("test out\n");
    return;
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
