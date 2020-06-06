
#include "graph.h"
#include <stdio.h>

graph_t *build_first_topo()
{

/*

						  +----------+
					      0/4 |          |0/0
				 +----------------+   R0_re  +---------------------------+
				 |                |          |                           |
				 |                +----------+                           |
				 |                                                       |
				 |                                                       |
				 |                                                       |
				 |                                                       |
				 |0/5                                                    |0/1
			     +---+---+                                              +----+-----+
			     |       |0/3                                        0/2|          |
			     | R2_re +----------------------------------------------+    R1_re |
			     |       |                                              |          |
			     +-------+                                              +----------+
*/

	graph_t *topo= create_new_graph("Generic Graph Topology");  	// Graph creation
	
	node_t *node_R0=create_graph_node(topo , "Router0");		// Creating and adding nodes in the graph
	node_t *node_R1=create_graph_node(topo , "Router1");
	node_t *node_R2=create_graph_node(topo , "Router2");
	
	insert_link_bw_two_nodes(node_R0,node_R1,"eth0/0","eth0/1",1);
	insert_link_bw_two_nodes(node_R1,node_R2,"eth0/2","eth0/3",1);
	insert_link_bw_two_nodes(node_R2,node_R0,"eth0/5","eth0/4",1);
	
	return topo;
}
