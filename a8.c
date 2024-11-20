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


void downward_heapify(int * pq, int size, int target) { // size in this contex is that of the array
    /// everytime we are getting rid of the root node, i.e. shortest targeta
    int temp = pq[target];  // target refers to the node of ineterest
    //int temp2 = distance[target]; // peform that same heapify on the distance array
    int j;

    while((j = (2*target + 1) <= size)) { // checking the indice to the left child                                    // if they are within array bounds, that is
        j = (2*target+1); // checking the left child 

        if (j < size && pq[j] > pq[j+1]) {// if the variable is smalll and smaller than its right sibling
        j++;
        }
        if (temp <= pq[j]) {// checking the target, if its smaller than the smallest child
            break; // ex. 0 <= inf true
        } else {
        pq[target] = pq[j]; // p[0] = target var, node left or right... 
        //distance[target] = distance[j];
        target = j;
        }
    }

   // distance[target] = temp2;
    pq[target] = temp; 
}

int extract_min(int * pq, int * size) {
    int min = pq[0]; // we know that the min value is always in the root, heap property
    //*d_u = distance[0];
    pq[0] = pq[*size - 1]; // move to the root... the distance should also be moved.
    //distance[0] = distance[*size -1];
    (*size)--; // reduce the size, i wonder why *size-- wasn't working
    downward_heapify(pq, *size, 0); // the target to be removed is always zero. The goal is to adjust the rest of the mess below the root.
    return min; // returns, again, the original index value of the list, , which is the root node as declared before the readjustment
}

void djk_mul_weights(struct adj_Vertex * adj_list, int source, int target, int vertices, int period) {

    //intializing the arrays to use
    int * pq = malloc(vertices*sizeof(int)); // empty priority queue, a min heap 
    int * distance = malloc(vertices*sizeof(int)); // shortest current index 
    int size = vertices;
    int ** prev_array = malloc(sizeof(int*)); // create a previous 2d array with one row rn

    for (int m = 0; m < size; m++) {
        distance[m] = 1000000; // max out the distances 
        pq[m] = m; // populate pq with the vertices
        prev_array[m] = NULL; 
    }
    
    distance[source] = 0; // initially, the names match the index value so we can directly assign it like so

    for (int n = (size/2) - 1; n >= 0; n--){
    downward_heapify(pq, size, source); // perform downward heapify for initialization, brings the node assigned as the source, distance 0, to the top.
    } // why do we perform the heapify this many times?
    
    // heap brings the source node to the top!

    int track_depth = 0;
    int idx = 0;

    while (size > 0) { // equivalent of not being empty...
        int u = extract_min(pq, &size); // this is the NAME of the minimum node.
        int d_u = distance[u]; // getting the name of the minimum value in the heap
        struct adj_Edges *edges = adj_list[u].Edges; // get a copy of the edges adjacency list , referrable by the name index 
        int edge_count = adj_list[u].edge_count - 1; // get a copy of the count

        for (int i = 0; i < edge_count; i++) { // we dont check the node here. 
            int v = edges[i].name; // for the first input in the edges array, get its corresponding namee
            int weight_index = track_depth % period; // get the weight index
            int d_st = d_u + edges[i].weights[weight_index]; // 
            if (d_st == (1000000 + edges[i].weights[weight_index])) {
                d_st = edges[i].weights[weight_index];
            }
            //printf("calculated distance between <%d, %d> is: %d + %d = %d\n", u, v, distance[u], edges[i].weights[weight_index], d_st);

            if (distance[v] > d_st) { // just has to be within the range of values..it seems like they are ordered!
                printf("replacing distance from %d, %d from %d to %d\n", u, v, distance[v], d_st);
                distance[v] = d_st; // replacing the distance on that index with this 
                
                // if (idx == 0) {
                // prev_array[idx] = malloc(sizeof(int)*vertices); // creating a new previous entry directly 
                // for (int k = 0; k < vertices; k++) {
                //     prev_array[idx][v] = u; // we just keep track of the name of the previous index through 
                // }
                // } else {
                // prev_array = realloc(prev_array, (idx+1)*sizeof(int)); // realloc for another row
                // prev_array[idx] = malloc(sizeof(int)*vertices);
                // for (int k = 0; k < vertices; k++) {
                //     prev_array[idx][v] = u; // we just keep track of the name of the previous index through 
                // }
                // }
                // idx++;

                for (int j = 0; j < size; j++) {
                    if (pq[j] == v) { // go through the pq, find the index with name of the node i want, downward heapify with one connected edge
                        downward_heapify(pq, size, j); // adjust the standing in the heap of the newly readjusted value
                        break;
                    }
                }
                }
            }
        }
        track_depth++;


    // for (int i = 0; i < vertices; i++) {
    //     for(int j = 0; j < vertices; j++){
    //         printf("%d ", prev_array[i][j]);
    //     }
    //     printf("\n");
    // }

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
