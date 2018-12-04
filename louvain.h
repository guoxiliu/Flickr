#ifndef LOUVAIN_HEADER_FILE
#define LOUVAIN_HEADER_FILE

#include "utils.h"
#include <stdlib.h>

void first_phase(igraph_t* graph, igraph_vector_t* clusters, int max_iter); 
void second_phase(igraph_t* graph, igraph_vector_t* clusters);
void apply_method(igraph_t* graph, igraph_vector_t* clusters);

#endif

/**
 * First phase of Louvain clustering algorithm.
 * In this phase, we are trying to merge each node with one of 
 * its neighbors to obtain the maximum modularity gain.
 */ 
void first_phase(igraph_t* graph, igraph_vector_t* clusters, int max_iter) {
    int node_num = igraph_vcount(graph);

    // Compute the sum of the weights of links in the graph.
    igraph_vector_t edge_weights;
    igraph_vector_init(&edge_weights, 0);
    EANV(graph, "weight", &edge_weights);
    igraph_real_t weights_sum = igraph_vector_sum(&edge_weights);

	// Create the vertex selector and vertex iterator.
    igraph_vit_t vertex_iterator;
    igraph_vs_t vertex_selector;
    igraph_vs_all(&vertex_selector);
    igraph_vit_create(graph, vertex_selector, &vertex_iterator);

    // Create the edge selector and edge iterator.
    igraph_es_t edge_selector;
    igraph_eit_t edge_iterator;

    // Get the degree of each node.
    igraph_vector_t node_weights;
    igraph_vector_init(&node_weights, 0);
    igraph_strength(graph, &node_weights, vertex_selector, IGRAPH_IN, IGRAPH_LOOPS, &edge_weights);

    // Test if the vector is as expected.
    // print_vector(&node_weights, "node_weights.test");

    igraph_vit_destroy(&vertex_iterator);
    igraph_vs_destroy(&vertex_selector);
    
	// Loop the phase for a given maximum times.
    for (int count = 0; count < max_iter; count++) {
        // Compute the weight of each cluster.
        igraph_bool_t update = 0;
        igraph_vector_t cluster_tot;
        igraph_vector_init(&cluster_tot, node_num);

        for (int i = 0; i < node_num; i++) {
            igraph_integer_t cluster_id = VECTOR(*clusters)[i];
			VECTOR(cluster_tot)[cluster_id] += VECTOR(node_weights)[i];
        }
        // print_vector(&cluster_tot, "cluster_weights.test");
        
		// // Print out the current modularity.
        // igraph_real_t cur_Q;
        // igraph_modularity(graph, clusters, &cur_Q, &edge_weights);
        // printf("%f\n", cur_Q);

        for (int node_id = 0; node_id < node_num; node_id++) {
            // printf("%d ", node_id);
            igraph_integer_t old_cluster_id = VECTOR(*clusters)[node_id];
            igraph_integer_t new_cluster_id = old_cluster_id;
            igraph_integer_t adj_node_id, adj_cluster_id, adj_edge_id, adj_edge_weight;
            igraph_integer_t node_weight = VECTOR(node_weights)[node_id];
            igraph_real_t max_deltaQ = 0.0;

            // Get the adjancent vertex set of the current node.
            
            igraph_vs_adj(&vertex_selector, node_id, IGRAPH_ALL);
            igraph_vit_create(graph, vertex_selector, &vertex_iterator);

            igraph_es_incident(&edge_selector, node_id, IGRAPH_ALL);
            igraph_eit_create(graph, edge_selector, &edge_iterator);

            igraph_vector_t weights_per_cluster, visited_clusters;
            igraph_vector_init(&weights_per_cluster, node_num);
			igraph_vector_init(&visited_clusters, 0);

            // Compute the sum of the weights of the links from current node to each cluster (using vertex selector cannot get the weight of the adjacent edge)
            // while (!IGRAPH_VIT_END(vertex_iterator)) {
            //     adj_node_id = IGRAPH_VIT_GET(vertex_iterator);
            //     adj_cluster_id = VECTOR(*clusters)[adj_node_id];
			// 	VECTOR(weights_per_cluster)[adj_cluster_id] += 2;
			// 	IGRAPH_VIT_NEXT(vertex_iterator);
            // }

            // Compute the sum of the weights of the links from current node to each cluster
            while (!IGRAPH_EIT_END(edge_iterator)) {
                adj_edge_id = IGRAPH_EIT_GET(edge_iterator);
                adj_edge_weight = EAN(graph, "weight", adj_edge_id);
                igraph_integer_t from, to;
                igraph_edge(graph, adj_edge_id, &from, &to);
                
                // Ignore self loop.
                if (from == to) {
                    IGRAPH_EIT_NEXT(edge_iterator);
                    continue;
                }

                if (from == node_id) {
                    adj_cluster_id = VECTOR(*clusters)[to];
    				VECTOR(weights_per_cluster)[adj_cluster_id] += adj_edge_weight;
                }

                if (to == node_id) {
                    adj_cluster_id = VECTOR(*clusters)[from];
    				VECTOR(weights_per_cluster)[adj_cluster_id] += adj_edge_weight;
                }

                IGRAPH_EIT_NEXT(edge_iterator);
            }

            // Remove the current node from the original cluster.
            VECTOR(cluster_tot)[old_cluster_id] -= node_weight;

            IGRAPH_VIT_RESET(vertex_iterator);      // reset the vertex iterator
            // Loop through all adjacent vertices.
            while (!IGRAPH_VIT_END(vertex_iterator)) {
                adj_node_id = IGRAPH_VIT_GET(vertex_iterator);
                adj_cluster_id = VECTOR(*clusters)[adj_node_id];
				
				// Only compute the delta Q once for a cluster id.
				if (igraph_vector_search(&visited_clusters, 0, adj_cluster_id, NULL) == 0) {
					igraph_vector_push_back(&visited_clusters, adj_cluster_id); 

					// deltaQ = k_{i,in} - (\sum_{tot}(c)*k_i) / m
					igraph_real_t cur_deltaQ = 2 * VECTOR(weights_per_cluster)[adj_cluster_id] - ((node_weight * VECTOR(cluster_tot)[adj_cluster_id]) / weights_sum);

					if (cur_deltaQ > max_deltaQ) {
						new_cluster_id = adj_cluster_id;
						max_deltaQ = cur_deltaQ;
					}

				}

                IGRAPH_VIT_NEXT(vertex_iterator);
            }

            VECTOR(cluster_tot)[new_cluster_id] += node_weight; 
            if (new_cluster_id != old_cluster_id) {
                VECTOR(*clusters)[node_id] = new_cluster_id;
                update = 1;
            }

            // Destroy created things to release memory.
            igraph_vs_destroy(&vertex_selector);
            igraph_vit_destroy(&vertex_iterator);
            igraph_es_destroy(&edge_selector);
            igraph_eit_destroy(&edge_iterator);
			igraph_vector_destroy(&visited_clusters);
            igraph_vector_destroy(&weights_per_cluster);

        }

        // Destroy created vector to release memory.
        igraph_vector_destroy(&cluster_tot);
		
		// See if there is any update.
        if (!update) {
            // printf("The loop time of the first phase: %d\n", count);
            break;
        }
    }
	
	// Relabel the clusters from 0.
	igraph_vector_t unique_clusters;
	get_unique_vector(clusters, &unique_clusters);
	igraph_integer_t cluster_size = igraph_vector_size(&unique_clusters);
    
    for(int i = 0; i < cluster_size; i++) {
        int cluster_id = VECTOR(unique_clusters) [i];
        for (int j = 0; j < igraph_vector_size(clusters); j++) {
            if (VECTOR(*clusters)[j] == cluster_id) {
                VECTOR(*clusters)[j] = i;
            }
        }
    }
    // Destroy created vector to release memory.
	igraph_vector_destroy(&unique_clusters);
    igraph_vector_destroy(&node_weights);
    igraph_vector_destroy(&edge_weights);
}

/**
 * Second phase of Louvain clustering method.
 * In this phase, we rebuild the graph according to the clusters we got
 * from the first phase. Each cluster is a new node, and the weights of 
 * edges between different clusters are computed from the original graph. 
 */
void second_phase(igraph_t* graph, igraph_vector_t* clusters) {
	igraph_vector_t unique_clusters, edge_weights;
	get_unique_vector(clusters, &unique_clusters);
	igraph_integer_t cluster_size = igraph_vector_size(&unique_clusters);
    

    // Create a new full graph.
    igraph_t new_graph;
    igraph_full(&new_graph, cluster_size, IGRAPH_UNDIRECTED, IGRAPH_LOOPS);

    igraph_vector_init(&edge_weights, igraph_ecount(&new_graph));
    SETEANV(&new_graph, "weight", &edge_weights);

    igraph_vector_destroy(&unique_clusters);
    igraph_vector_destroy(&edge_weights);       // this vector can be very large, so release it at once

    igraph_es_t edge_selector;
    igraph_eit_t edge_iterator;

    igraph_es_all(&edge_selector, IGRAPH_EDGEORDER_ID);
    igraph_eit_create(graph, edge_selector, &edge_iterator);

    while (!IGRAPH_EIT_END(edge_iterator)) {
        igraph_integer_t edge_id = IGRAPH_EIT_GET(edge_iterator);
        igraph_real_t old_weight, edge_weight;
        igraph_integer_t from, to, from_cluster_id, to_cluster_id, new_edge_id;

        // Get the end nodes of the edge.
        igraph_edge(graph, edge_id, &from, &to);
        from_cluster_id = VECTOR(*clusters)[from];
        to_cluster_id = VECTOR(*clusters)[to];
        edge_weight = EAN(graph, "weight", edge_id);

        igraph_get_eid(&new_graph, &new_edge_id, from_cluster_id, to_cluster_id, IGRAPH_UNDIRECTED, /*error=*/0);

		// Cannot find the edge in the new graph.
        if (new_edge_id == -1) {
            fprintf(stderr, "%d %d\n", from_cluster_id, to_cluster_id);
            fprintf(stderr, "Cannot find the edge!\n");
            return;
        }

        old_weight = EAN(&new_graph, "weight", new_edge_id);
        SETEAN(&new_graph, "weight", new_edge_id,old_weight+edge_weight);
        
        IGRAPH_EIT_NEXT(edge_iterator);
    }

    igraph_es_destroy(&edge_selector);
    igraph_eit_destroy(&edge_iterator);

    igraph_es_all(&edge_selector, IGRAPH_EDGEORDER_ID);
    igraph_eit_create(&new_graph, edge_selector, &edge_iterator);

    // Delete edges whose weights are 0.
    igraph_vector_t deleted_edges;
    igraph_vector_init(&deleted_edges, 0);

    while (!IGRAPH_EIT_END(edge_iterator)) {
        igraph_integer_t edge_id = IGRAPH_EIT_GET(edge_iterator);
        igraph_integer_t edge_weight = EAN(&new_graph, "weight", edge_id);
        
        if (edge_weight == 0) {
            igraph_vector_push_back(&deleted_edges, edge_id);
        }

        IGRAPH_EIT_NEXT(edge_iterator);
    }

    // Destroy the edge selector and iterator.
    igraph_es_destroy(&edge_selector);
    igraph_eit_destroy(&edge_iterator);

    igraph_es_vector(&edge_selector, &deleted_edges);
    igraph_delete_edges(&new_graph, edge_selector);

    // printf("The number of edges in new graph: %d\n", igraph_ecount(&new_graph));
    // printf("The weight of the first edge is: %.2f\n", EAN(&new_graph, "weight", 0));

    // Test if the total weights changed after the above process.
    // igraph_vector_t new_edge_weights;
    // igraph_vector_init(&new_edge_weights, 0);
	// // igraph_real_t cur_Q;
    // EANV(&new_graph, "weight", &new_edge_weights);
    // if (igraph_vector_contains(&new_edge_weights, 0)) {
    //     printf("Bad things happened...\n");
    // }
    // // printf("The total weights of the new graph is: %.2f\n", igraph_vector_sum(&new_edge_weights)); 
    // igraph_vector_destroy(&new_edge_weights);


    // Destroy the created stuff to release memory.
    igraph_vector_destroy(&deleted_edges);

    // Copy the new graph to the original one.
    igraph_destroy(graph);
    igraph_copy(graph, &new_graph);
    igraph_destroy(&new_graph);

}

/**
 * Apply the Louvain method.
 */
void apply_method(igraph_t* graph, igraph_vector_t* clusters) {
	// Make a copy of the original graph.
	igraph_t tmp_graph;
	igraph_empty(&tmp_graph, /*n=*/ 0, IGRAPH_UNDIRECTED);
	igraph_copy(&tmp_graph, graph);

	// Each node is a cluster at the very first.
	igraph_vector_init_seq(clusters, 0, igraph_vcount(graph));

    igraph_vector_t cur_clusters;			// the results in the first phase
    igraph_real_t best_modularity = 0;
	int loop_count = 0; 					// loop times: used for writing graph 
	
	igraph_bool_t write_to_file = 1; 		// option to save the graph to local file

    while (1) {
		if (write_to_file) {
			// Write current graph to a file in GraphML format.
			char str_count[6];
			sprintf(str_count, "%d", loop_count);
			char out_file_name[30];
			strcpy(out_file_name, "graph_");
			strcat(out_file_name, str_count); strcat(out_file_name, ".graphml");
			FILE* out_file = fopen(out_file_name, "w");
			if (out_file == NULL) {
				fprintf(stderr, "Cannot open file to write the graph!\n");
				return;
			}
			igraph_write_graph_graphml(&tmp_graph, out_file, /*prefixattr=*/ 1);
			fclose(out_file);
		}

		loop_count++; 		// increment the loop count

		// Intialization, every node itself is a cluster at first beginning.
        igraph_vector_init_seq(&cur_clusters, 0, igraph_vcount(&tmp_graph)-1);

		// Run the first phase.
        first_phase(&tmp_graph, &cur_clusters, /*max_iter=*/ 5);
		
		// Reorganize the cluster ids.
		for (int i = 0; i < igraph_vector_size(clusters); i++) {
			igraph_integer_t old_cluster_id = VECTOR(*clusters)[i];
			igraph_integer_t new_cluster_id = VECTOR(cur_clusters)[old_cluster_id];
			VECTOR(*clusters)[i] = new_cluster_id;
		}

		// Compute the current modularity.
        igraph_real_t cur_modularity;
		igraph_modularity(graph, clusters, &cur_modularity, NULL);
		printf("Current modularity: %f\n", cur_modularity);

		// Break the while-loop.
        if(cur_modularity == best_modularity)
            break;
        
		// Run the second phase.
        second_phase(&tmp_graph, &cur_clusters);
		// printf("The number of the edges after the second phase: %d\n", igraph_ecount(&tmp_graph));

		// Update best modularity.
        best_modularity = cur_modularity;

		// Destory used vectors to release memory.
        igraph_vector_destroy(&cur_clusters);
    }
}

