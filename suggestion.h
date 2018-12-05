#ifndef SUGGESTION_HEADER_FILE
#define SUGGESTION_HEADER_FILE

#include "utils.h"
#include <igraph.h>

#define TOP 10

/**
 * Suggest potential friends based on the Jaccard similarity.
 */
void suggest_jaccard(igraph_t* graph, igraph_vector_t* clusters, igraph_integer_t node_id, igraph_vector_t* potential) {	

	int node_num = igraph_vcount(graph);
	int cluster_id = VECTOR(*clusters)[node_id];

	igraph_vector_t cluster_nodes, adj_nodes;
	igraph_vector_init(&cluster_nodes, 0);
	igraph_vector_init(&adj_nodes, 0);

	igraph_neighbors(graph, &adj_nodes, node_id, IGRAPH_ALL);
	
	// Get the nodes in the same cluster with the given node.
	for (int i = 0; i < node_num; i++) {
		if (i == node_id) continue;

		if (VECTOR(*clusters)[i] == cluster_id) {
			if (igraph_vector_contains(&adj_nodes, i) == 0) {	
				igraph_vector_push_back(&cluster_nodes, i);
			}
		}
	}

	igraph_vector_t top_scores;						// TOP scores are stored 
	igraph_vector_t cluster_adj_nodes;				// neighbors of the cluster node
	
	igraph_vector_init(&top_scores, TOP);
	igraph_vector_init(potential, TOP);
	igraph_vector_init(&cluster_adj_nodes, 0);

	igraph_real_t min_score = 0;					// stores the minimum score in `top_scores` vector 
	igraph_integer_t min_index = 0;					// stores the index of the minimum score
	
	for (int i = 0; i < igraph_vector_size(&cluster_nodes); i++) {
		int cluster_node_id = VECTOR(cluster_nodes)[i];
		igraph_neighbors(graph, &cluster_adj_nodes, cluster_node_id, IGRAPH_ALL);
		igraph_real_t common_adj_num = count_same_elements(&adj_nodes, &cluster_adj_nodes);

		// Calculate the Jaccard similarity.
		igraph_real_t cur_score = common_adj_num / (igraph_vector_size(&adj_nodes)
				+ igraph_vector_size(&cluster_adj_nodes) - common_adj_num);
		// printf("%f ", cur_score);

		if (cur_score > min_score) {
			VECTOR(top_scores)[min_index] = cur_score;
			VECTOR(*potential)[min_index] = i;
			min_score = igraph_vector_min(&top_scores);
			min_index = igraph_vector_which_min(&top_scores);
		}
	}

	igraph_vector_destroy(&top_scores);
	igraph_vector_destroy(&cluster_adj_nodes);
	igraph_vector_destroy(&adj_nodes);
	igraph_vector_destroy(&cluster_nodes);

}


#endif
