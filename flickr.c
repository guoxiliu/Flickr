#include "louvain.h"
#include <time.h>

/**
 * Main function.
 */
int main(int argc, char* argv[]) 
{
	igraph_t userGraph;
	igraph_i_set_attribute_table(&igraph_cattribute_table);
	
	// Define path and file names.
	char path[30] = "./dataset/";
	char node_file_name[30], edge_file_name[30]; 
	strcpy(node_file_name, path); strcat(node_file_name, "nodes.csv");
	strcpy(edge_file_name, path); strcat(edge_file_name, "edges.csv");

	// Set the graph to be undirected.
	igraph_empty(&userGraph, 0, IGRAPH_UNDIRECTED);

	// Add vertices from "nodes.csv" file.
	add_nodes_from_csv(&userGraph, node_file_name);
	printf("The number of nodes in the graph is: %d\n", igraph_vcount(&userGraph));

	// Add edges from "edges.csv" file.
	add_edges_from_csv(&userGraph, edge_file_name);
	printf("The number of edges in the graph is: %d\n", igraph_ecount(&userGraph));

	// Create a vector to store community partition.
	igraph_vector_t communities;

	// Compute the run time.
	clock_t start = clock();
	apply_method(&userGraph, &communities);
	clock_t end = clock();
	double time_spent = (double) (end-start) / CLOCKS_PER_SEC;
	printf("Time spent for Louvain method: %.2f seconds\n", time_spent);
	print_vector(&communities, "communities.test");

	// igraph_vector_t unique_communities;
	// get_unique_vector(&communities, &unique_communities);
	// print_vector(&unique_communities, "unique_communities.test");

	// // Destroy created structures to release memory.
	// igraph_vector_destroy(&unique_communities);

	igraph_vector_destroy(&communities);
	igraph_destroy(&userGraph);

	return 0;
}
