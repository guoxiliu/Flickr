#include "utils.h"
#include "louvain.h"
#include "suggestion.h"
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
	printf("The number of nodes in the graph is: %d.\n", igraph_vcount(&userGraph));

	// Add edges from "edges.csv" file.
	add_edges_from_csv(&userGraph, edge_file_name);
	printf("The number of edges in the graph is: %d.\n", igraph_ecount(&userGraph));

	// Create a vector to store community partition.
	igraph_vector_t communities;

	// Compute the run time of Louvain method.
	clock_t start = clock();
	apply_method(&userGraph, &communities);
	clock_t end = clock();
	double time_spent = (double) (end-start) / CLOCKS_PER_SEC;
	printf("Time spent for Louvain method: %.2f seconds.\n", time_spent);
	print_vector(&communities, "mycommunities.test");
	
	// Compute the modularity after clustering.
	// igraph_real_t modularity;
	// igraph_modularity(&userGraph, &communities, &modularity, NULL);
	// printf("The modularity is %.f\n", modularity);


	/* 
	// The code below tests the APIs provided by igraph.
	igraph_real_t modularity;
	igraph_matrix_t merges;
	igraph_vector_t modularities;

	igraph_matrix_init(&merges, 0, 0);
	igraph_vector_init(&communities, 0);
	igraph_vector_init(&modularities, 0);

	clock_t start = clock();
	igraph_community_fastgreedy(&userGraph, 0, &merges, &modularities, &communities);
	print_vector(&communities, "greedy_communities.test");
	// igraph_community_multilevel(&userGraph, NULL, &communities, NULL, &modularities);
	// print_vector(&communities, "multilevel_communities.test");
	clock_t end = clock();
	double time_spent = (double) (end-start) / CLOCKS_PER_SEC;
	printf("Time spent for multilevel method: %.2f seconds.\n", time_spent);
	igraph_modularity(&userGraph, &communities, &modularity, NULL);
	printf("The modularity is %f\n.", modularity);

	igraph_matrix_destroy(&merges);
	igraph_vector_destroy(&modularities);
	*/
	

	// Compute the unique communities.
	igraph_vector_t unique_communities;
	get_unique_vector(&communities, &unique_communities);
	// print_vector(&unique_communities, "unique_communities.test");
	printf("There are %ld different clusters.\n", igraph_vector_size(&unique_communities));
	igraph_vector_destroy(&unique_communities);

	// An example of friend suggestion.
	igraph_integer_t test_user_id = 300;
	igraph_vector_t possible_friends;
	igraph_vector_init(&possible_friends, 0);
	suggest_jaccard(&userGraph, &communities, test_user_id, &possible_friends);
	
	printf("The top %d friends suggestions for user %d are:", TOP, test_user_id);
	for (int i = 0; i < igraph_vector_size(&possible_friends); i++) {
		printf(" %ld", (long int) VECTOR(possible_friends)[i]);
	}
	printf(".\n");


	// Destroy created structures to release memory.
	igraph_vector_destroy(&possible_friends);
	igraph_vector_destroy(&communities);
	igraph_destroy(&userGraph);


	return 0;
}
