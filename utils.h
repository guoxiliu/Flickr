#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <igraph.h>

/**
 * Print an igraph vector to a given file. 
 */ 
void print_vector(igraph_vector_t* v, char* filename) {
    FILE *outFile = fopen(filename, "w");
    if (outFile == NULL) {
        fprintf(stderr, "Open the output file error!");
        return;
    }

    // Print the vector to the file.
    long int i;
    for (i = 0; i < igraph_vector_size(v); i++) {
        fprintf(outFile, "%li\n", (long int) VECTOR(*v)[i]);
    }
    fprintf(outFile, "\n");
    fclose(outFile);
}


/**
 * Add nodes from a csv file to the given graph.
 */
void add_nodes_from_csv(igraph_t* graph, const char* filename) {
    FILE* inFile = fopen(filename, "r");
    if (inFile == NULL) {
        fprintf(stderr, "Open csv file error!\n");
        return;
    }

    int num, lines = 0;
    while (fscanf(inFile, "%d ", &num) != EOF) { lines++; }
    
    if (lines != num) {
	    fprintf(stderr, "The number of lines are different from the last node id!\n");
        return;
    }

    igraph_add_vertices(graph, num, 0);
}


/**
 * Add edges from a csv file to the given graph.
 */ 
void add_edges_from_csv(igraph_t* graph, const char* filename) {
    FILE* inFile = fopen(filename, "r");
    if (inFile == NULL) {
        fprintf(stderr, "Open csv file error!\n");
        return;
    }

    char line[50];
    // Turn on attribute handling.
	igraph_i_set_attribute_table(&igraph_cattribute_table);
    
    igraph_vector_t edge_vector, edge_weights;
    igraph_vector_init(&edge_vector, 0);
    while (fgets(line, 50, inFile)  != NULL) {
        // Separate the string by commas.
        char* pt = strtok(line, ",");
        while (pt != NULL) {
            int id = atoi(pt)-1;      // convert the string to an integer, the vertex id starts with 0 in igraph
            if (igraph_vector_push_back(&edge_vector, id) == IGRAPH_ENOMEM) {
                fprintf(stderr, "Not enough memory!\n");
                return;
            }
            pt = strtok (NULL, ",");
        }
    }
	// Add edges into the graph, and set the weight of each edge to 1.
    igraph_add_edges(graph, &edge_vector, 0);
    igraph_vector_destroy(&edge_vector);

	igraph_vector_init(&edge_weights, igraph_ecount(graph)); 
	igraph_vector_fill(&edge_weights, 1.0);
	SETEANV(graph, "weight", &edge_weights);

	igraph_vector_destroy(&edge_weights);
}


/**
 * Get the unique elements of a given vector.
 */
void get_unique_vector(igraph_vector_t* given_vector, igraph_vector_t* unique_vector) {
    igraph_vector_init(unique_vector, 0);

    for (int i = 0; i < igraph_vector_size(given_vector); i++) {
        if (igraph_vector_search(unique_vector, 0, VECTOR(*given_vector)[i], NULL) == 0) {
            igraph_vector_push_back(unique_vector, VECTOR(*given_vector)[i]);
        }
    }
}
