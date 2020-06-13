
#include "graph.h"
#include <stdio.h>

extern void network_start_pkt_receiver_thread(graph_t *topo);

graph_t *build_first_topo()
{

/*
				    122.1.1.0
				   +----------+
		              0/4 |   R0_re   | 0/0
		 +----------------+           +--------------------------+
		 |     40.1.1.1/24| 	       |20.1.1.1/24               |
		 |                +-----------+                          |
		 |                                                       |
		 |                                                       |
		 |                                                       |
		 |40.1.1.2/24                                            |20.1.1.2/24
		 |0/5                                                    |0/1
	     +---+---+                                              +----+-----+
	     |       |0/3                                        0/2|          |
	     | R2_re +----------------------------------------------+    R1_re |
	     |       |30.1.1.2/24                        30.1.1.1/24|          |
	     +-------+                                              +----------+
	     122.1.1.1							122.1.1.2

*/

	graph_t *topo= create_new_graph("Generic Graph Topology");  	// Graph creation
	
	node_t *node_R0=create_graph_node(topo , "Router0");		// Creating and adding nodes in the graph
	node_t *node_R1=create_graph_node(topo , "Router1");
	node_t *node_R2=create_graph_node(topo , "Router2");
	
	insert_link_bw_two_nodes(node_R0,node_R1,"eth0/0","eth0/1",1);
	insert_link_bw_two_nodes(node_R1,node_R2,"eth0/2","eth0/3",1);
	insert_link_bw_two_nodes(node_R2,node_R0,"eth0/5","eth0/4",1);
	
	node_set_loopback_address(node_R0,"122.1.1.0");
	node_set_interface_ip_address(node_R0, "eth0/4", "40.1.1.1", 24);
    	node_set_interface_ip_address(node_R0, "eth0/0", "20.1.1.1", 24);
    	
    	node_set_loopback_address(node_R1, "122.1.1.1");
   	node_set_interface_ip_address(node_R1, "eth0/1", "20.1.1.2", 24);
        node_set_interface_ip_address(node_R1, "eth0/2", "30.1.1.1", 24);
        
        node_set_loopback_address(node_R2, "122.1.1.2");
    	node_set_interface_ip_address(node_R2, "eth0/3", "30.1.1.2", 24);
    	node_set_interface_ip_address(node_R2, "eth0/5", "40.1.1.2", 24);
    	
    	network_start_pkt_receiver_thread(topo);	// this function starts listening on the UDP-Sockets.
	
	return topo;
}
