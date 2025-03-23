#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#define MAX_VERTICES 2000000

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

Node* graph[MAX_VERTICES];
int max_clique_size = 0;

bool is_neighbor(int v, int u) {
    if(v < 0 || v >= MAX_VERTICES || u < 0 || u >= MAX_VERTICES) return false;
    
    Node* temp = graph[v];
    while(temp) {
        if(temp->vertex == u) return true;
        temp = temp->next;
    }
    return false;
}

void tomita_algorithm(int* active_nodes, int actual_vertex_count) {
    typedef struct {
        int* R;         // Current clique
        int R_size;
        int* P;         // Potential candidates
        int P_size;
        int* X;         // Excluded candidates
        int X_size;
    } StackFrame;

    StackFrame* stack = malloc(MAX_VERTICES * sizeof(StackFrame));
    int stack_top = 0;

    stack[stack_top++] = (StackFrame){
        .R = malloc(0), .R_size = 0,
        .P = active_nodes, .P_size = actual_vertex_count,
        .X = malloc(0), .X_size = 0
    };

    while(stack_top > 0) {
        StackFrame frame = stack[--stack_top];

        if(frame.P_size == 0 && frame.X_size == 0) {
            printf("Maximal Clique (Size %d): ", frame.R_size);
            for(int i = 0; i < frame.R_size; i++) {
                printf("%d ", frame.R[i]); // 0-based output
            }
            printf("\n");
            
            if(frame.R_size > max_clique_size) {
                max_clique_size = frame.R_size;
            }
        } else {
            int pivot = -1, max_degree = -1;
            for(int i = 0; i < frame.P_size + frame.X_size; i++) {
                int u = (i < frame.P_size) ? frame.P[i] : frame.X[i - frame.P_size];
                int degree = 0;
                for(int j = 0; j < frame.P_size; j++) {
                    if(is_neighbor(u, frame.P[j])) degree++;
                }
                if(degree > max_degree) {
                    max_degree = degree;
                    pivot = u;
                }
            }

            for(int i = frame.P_size-1; i >= 0; i--) {
                int v = frame.P[i];
                if(pivot != -1 && is_neighbor(pivot, v)) continue;

                int* new_R = malloc((frame.R_size + 1) * sizeof(int));
                memcpy(new_R, frame.R, frame.R_size * sizeof(int));
                new_R[frame.R_size] = v;

                int* new_P = malloc(frame.P_size * sizeof(int));
                int new_P_size = 0;
                for(int j = 0; j < frame.P_size; j++) {
                    if(j != i && is_neighbor(v, frame.P[j])) {
                        new_P[new_P_size++] = frame.P[j];
                    }
                }

                int* new_X = malloc(frame.X_size * sizeof(int));
                int new_X_size = 0;
                for(int j = 0; j < frame.X_size; j++) {
                    if(is_neighbor(v, frame.X[j])) {
                        new_X[new_X_size++] = frame.X[j];
                    }
                }

                if(stack_top < MAX_VERTICES) {
                    stack[stack_top++] = (StackFrame){
                        .R = new_R, .R_size = frame.R_size + 1,
                        .P = new_P, .P_size = new_P_size,
                        .X = new_X, .X_size = new_X_size
                    };
                } else {
                    free(new_R);
                    free(new_P);
                    free(new_X);
                }

                frame.X = realloc(frame.X, (frame.X_size + 1) * sizeof(int));
                frame.X[frame.X_size++] = v;
                frame.P[i] = frame.P[--frame.P_size];
            }
        }

        free(frame.R);
        free(frame.P);
        free(frame.X);
    }
    free(stack);
}

int main() {
    FILE* file = fopen("wiki.txt", "r");
    if(!file) {
        printf("Error opening file\n");
        return 1;
    }

    int vertices, edges;
    if(fscanf(file, "%d %d", &vertices, &edges) != 2) {
        printf("Invalid header\n");
        fclose(file);
        return 1;
    }

    int *sources = malloc(edges * sizeof(int));
    int *dests = malloc(edges * sizeof(int));
    int min_node = INT_MAX;
    int max_node = -1;

    for(int i = 0; i < edges; i++) {
        if(fscanf(file, "%d %d", &sources[i], &dests[i]) != 2) {
            printf("Invalid edge at line %d\n", i+2);
            edges = i;  
            break;
        }
        if(sources[i] < min_node) min_node = sources[i];
        if(dests[i] < min_node) min_node = dests[i];
        if(sources[i] > max_node) max_node = sources[i];
        if(dests[i] > max_node) max_node = dests[i];
    }

    int offset = (min_node == 0) ? 0 : 1;
    printf("Using %s-based input conversion\n", offset ? "1" : "0");

    bool node_exists[MAX_VERTICES] = {false};
    int actual_vertex_count = 0;

    for(int i = 0; i < edges; i++) {
        int src = sources[i] - offset;
        int dest = dests[i] - offset;

        if(src >= 0 && src < MAX_VERTICES) {
            if(!node_exists[src]) {
                node_exists[src] = true;
                actual_vertex_count++;
            }
        }
        if(dest >= 0 && dest < MAX_VERTICES) {
            if(!node_exists[dest]) {
                node_exists[dest] = true;
                actual_vertex_count++;
            }
        }
    }

    int* active_nodes = malloc(actual_vertex_count * sizeof(int));
    int active_index = 0;
    for(int i = 0; i < MAX_VERTICES; i++) {
        if(node_exists[i]) {
            active_nodes[active_index++] = i;
        }
    }


    for(int i = 0; i < edges; i++) {
        int src = sources[i] - offset;
        int dest = dests[i] - offset;

        if(src >= 0 && src < MAX_VERTICES && dest >= 0 && dest < MAX_VERTICES) {
            Node* newNode = malloc(sizeof(Node));
            newNode->vertex = dest;
            newNode->next = graph[src];
            graph[src] = newNode;

            newNode = malloc(sizeof(Node));
            newNode->vertex = src;
            newNode->next = graph[dest];
            graph[dest] = newNode;
        }
    }
    free(sources);
    free(dests);
    fclose(file);

    clock_t start = clock();
    tomita_algorithm(active_nodes, actual_vertex_count);
    clock_t end = clock();

    printf("\nMaximum Clique Size: %d\n", max_clique_size);
    printf("Total Execution Time: %.2f seconds\n", (double)(end - start)/CLOCKS_PER_SEC);

    for(int i = 0; i < MAX_VERTICES; i++) {
        Node* current = graph[i];
        while(current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }


    return 0;
}
