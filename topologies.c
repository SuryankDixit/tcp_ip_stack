
#include "graph.h"
#include <stdio.h>

extern void network_start_pkt_receiver_thread(graph_t *topo);

graph_t *build_first_topo()
{

/*
                              122.1.1.0
                            +-----------+
		              0/4 |   R0_re   | 0/0
		 +----------------+           +--------------------------+
		 |     40.1.1.1/24| 	     |20.1.1.1/24               |
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


graph_t *build_linear_topo()
{
	graph_t *topo= create_new_graph("Linear Topology");  	// Graph creation
	
	node_t *H1=create_graph_node(topo , "H1");		// Creating and adding nodes in the graph
	node_t *H2=create_graph_node(topo , "H2");
	node_t *H3=create_graph_node(topo , "H3");
	
	insert_link_bw_two_nodes(H1,H2,"eth0/1","eth0/2",1);
	insert_link_bw_two_nodes(H2,H3,"eth0/3","eth0/4",1);
	
	node_set_loopback_address(H1,"122.1.1.0");
	node_set_interface_ip_address(H1, "eth0/1", "10.1.1.1", 24);
    	
    	node_set_loopback_address(H2, "122.1.1.1");
   	node_set_interface_ip_address(H2, "eth0/2", "10.1.1.2", 24);
        node_set_interface_ip_address(H2, "eth0/3", "20.1.1.2", 24);
        
        node_set_loopback_address(H3, "122.1.1.2");
    	node_set_interface_ip_address(H3, "eth0/4", "20.1.1.1", 24);
    	
    	network_start_pkt_receiver_thread(topo);	// this function starts listening on the UDP-Sockets.
	return topo;
}




graph_t *
build_simple_l2_switch_topo(){

	#if 0             
                                       +-----------+
                                       |  H4       |
                                       | 122.1.1.4 |
                                       +----+------+
                                            |eth0/7 - 10.1.1.3/24       
                                            |       
                                            |eth0/1
                                       +----+----+                        +--------+
       +---------+                     |         |                        |        |
       |         |10.1.1.2/24          |   L2Sw  |eth0/2       10.1.1.1/24|  H3    |
       |  H1     +---------------------+         +------------------------+122.1.1.3|
       |122.1.1.1|eth0/5         eth0/4|         |                 eth0/6 |        |
       + --------+                     |         |                        |        |
                                       +----+----+                        +--------+
                                            |eth0/3     
                                            |
                                            |
                                            |
                                            |10.1.1.4/24
                                            |eth0/8
                                      +----++------+
                                      |            |
                                      |   H2       |
                                      |122.1.1.2   |
                                      |            |
                                      +------------+

#endif


    graph_t *topo = create_new_graph("Simple L2 Switch Demo graph");
    node_t *H1 = create_graph_node(topo, "H1");
    node_t *H2 = create_graph_node(topo, "H2");
    node_t *H3 = create_graph_node(topo, "H3");
    node_t *H4 = create_graph_node(topo, "H4");
    node_t *L2SW = create_graph_node(topo, "L2SW");

    printf("%s",H1->node_name);

    insert_link_bw_two_nodes(H1, L2SW, "eth0/5", "eth0/4", 1);
    insert_link_bw_two_nodes(H2, L2SW, "eth0/8", "eth0/3", 1);
    insert_link_bw_two_nodes(H3, L2SW, "eth0/6", "eth0/2", 1);
    insert_link_bw_two_nodes(H4, L2SW, "eth0/7", "eth0/1", 1);

    // node_set_device_type(H1, L3_ROUTER);
    node_set_loopback_address(H1, "122.1.1.1");
    node_set_interface_ip_address(H1, "eth0/5", "10.1.1.2", 24);
    
    // node_set_device_type(H2, L3_ROUTER);
    node_set_loopback_address(H2, "122.1.1.2");
    node_set_interface_ip_address(H2, "eth0/8", "10.1.1.4", 24);

    // node_set_device_type(H3, L3_ROUTER);
    node_set_loopback_address(H3, "122.1.1.3");
    node_set_interface_ip_address(H3, "eth0/6", "10.1.1.1", 24);
    
    // node_set_device_type(H4, L3_ROUTER);
    node_set_loopback_address(H4, "122.1.1.4");
    node_set_interface_ip_address(H4, "eth0/7", "10.1.1.3", 24);
    
    // node_set_device_type(L2SW, L2_SWITCH);
    node_set_intf_l2_mode(L2SW, "eth0/1", ACCESS);
    node_set_intf_l2_mode(L2SW, "eth0/2", ACCESS);
    node_set_intf_l2_mode(L2SW, "eth0/3", ACCESS);
    node_set_intf_l2_mode(L2SW, "eth0/4", ACCESS);

    network_start_pkt_receiver_thread(topo);

     printf("\n%s\n",topo->topology_name);
     // print_node(H1);
     // print_node(H4);
     // print_node(H3);
     // print_node(H2);
     // print_node(L2SW);
     print_graph(topo);

    return topo;
}