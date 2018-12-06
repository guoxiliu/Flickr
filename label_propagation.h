#ifndef LPA_HEADER_FILE
#define LPA_HEADER_FILE

#include "utils.h"
#include <igraph.h>

#define SEED 2018


/**
 * Get the cluster id which has the highest total weight of adjacent edges.
 */
void get_most_cluster_ids(igraph_t* graph, igraph_vector_t* clusters, igraph_integer_t node_id, igraph_vector_t* most_cluster_ids) {
	// Get the neighbors of given node.
	igraph_vector_t adj_nodes;
	igraph_vector_init(&adj_nodes, 0);
	igraph_neighbors(graph, &adj_nodes, node_id, IGRAPH_ALL);
	
	igraph_integer_t max_count = 0;
	igraph_vector_t cluster_counters;
	igraph_vector_init(most_cluster_ids, 0);
	igraph_vector_init(&cluster_counters, igraph_vcount(graph));

	for (int i = 0; i < igraph_vector_size(&adj_nodes); i++) {
		igraph_integer_t adj_node_id = VECTOR(adj_nodes)[i];
		igraph_integer_t adj_cluster_id = VECTOR(*clusters)[adj_node_id];
		
		VECTOR(cluster_counters)[adj_cluster_id]++;
		if (max_count < VECTOR(cluster_counters)[adj_cluster_id]) {
			max_count = VECTOR(cluster_counters)[adj_cluster_id];
			igraph_vector_resize(most_cluster_ids, 1);
			VECTOR(*most_cluster_ids)[0] = adj_cluster_id;
		} else if (max_count == VECTOR(cluster_counters)[adj_cluster_id]) {
			igraph_vector_push_back(most_cluster_ids, adj_cluster_id);
		}

	}

	// Destroy created vectors.
	igraph_vector_destroy(&cluster_counters);
	igraph_vector_destroy(&adj_nodes);
}


/**
 * Label Propagation Algorithm
 */
void label_propagation(igraph_t* graph, igraph_vector_t* clusters, int max_iter) {
	int node_num = igraph_vcount(graph);
	igraph_vector_init_seq(clusters, 0, node_num-1);
	igraph_bool_t can_stop = 0;

	int count = 0;
	while (!can_stop && count < max_iter) {
		can_stop = 1;

		/**
		 * Change the labels for the nodes in a given graph.
		 * The label for a specific ndoe is based on the labels of its neighbors.
		 */

		// Random traverse the graph.
		igraph_vector_t visit_sequence;
		igraph_vector_init_seq(&visit_sequence, 0, igraph_vcount(graph)-1);
		igraph_vector_shuffle(&visit_sequence);

		igraph_vector_t most_cluster_ids;
		igraph_vector_init(&most_cluster_ids, 0);

		for (int i = 0; i < igraph_vector_size(&visit_sequence); i++) {
			int node_id = VECTOR(visit_sequence)[i];
			int cluster_id = VECTOR(*clusters)[node_id];

			get_most_cluster_ids(graph, clusters, node_id, &most_cluster_ids);

			int rand_cluster_id = igraph_rng_get_integer(igraph_rng_default(), 
							0, igraph_vector_size(&most_cluster_ids)-1);
			/* This variable should store the cluster id! NOT the generated index!
				It took me a lot of time to debug!!!!! */
			rand_cluster_id = VECTOR(most_cluster_ids)[rand_cluster_id];

			if (!igraph_vector_contains(&most_cluster_ids, cluster_id)) {
				// Randomly pick one cluster id.
				can_stop = 0;
			}
			VECTOR(*clusters)[node_id] = rand_cluster_id;

			igraph_vector_destroy(&most_cluster_ids);
		}

		igraph_vector_destroy(&visit_sequence);
		count++;
	}
}


#endif
