
#include "graph.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>


graph_t *create_new_graph(char *name)
{
	graph_t *graph= calloc(1,sizeof(graph_t));
	
	// topo->topology_name=topology_name    =>> strings in C are not assignable;
	// strncpy(dest,src,size);
	
	strncpy(graph->topology_name,name,32);      
	graph->topology_name[31] = '\0';
	
	initialize_glthread(&(graph->node_list));
	return graph;
}

node_t *create_graph_node(graph_t *graph,char *name)
{
	node_t *node= calloc(1,sizeof(node_t));
	strncpy(node->node_name,name,NODE_NAME_SIZE);
	node->node_name[NODE_NAME_SIZE-1]= '\0';
	
	init_node_nw_prop(&(node->node_nw_prop));		//initializing network nodes.
	
	// initially router has no interface that is holding any link, so initializing with NULL;
	//initialize_node_interfaces( node);		
	
	initialize_glthread( &(node->graph_glue));
	add_node_front( &graph->node_list, &node->graph_glue );
	return node;
}

void insert_link_bw_two_nodes(node_t *node1, node_t *node2,char *_from, char *_to, unsigned int cost)
{
	link_t *link = calloc(1,sizeof(link_t));
	
	// link structure has interface srructure as its member , so setting the interface properties using link.
	
	strncpy(link->intf1.intf_name,_from,IF_NAME_SIZE);
	link->intf1.intf_name[IF_NAME_SIZE-1] = '\0';
	strncpy(link->intf2.intf_name,_to,IF_NAME_SIZE);
	link->intf2.intf_name[IF_NAME_SIZE-1] = '\0';
	
	link->intf1.link=link;		// setting back link pointer into the interface link member;
	link->intf2.link=link;		// this will help in determining the neighbour of the each interface;
	
	link->intf1.att_node=node1;
	link->intf2.att_node=node2;
	
	link->cost=cost;
	
	int empty_interface_slot;
	
	empty_interface_slot= get_available_interface(node1);
	node1->intf[empty_interface_slot] = &link->intf1;
	
	empty_interface_slot= get_available_interface(node2);
	node2->intf[empty_interface_slot] = &link->intf2;
	
	//initializing network interface.
	init_intf_nw_prop(&link->intf1.intf_nw_props);
	init_intf_nw_prop(&link->intf2.intf_nw_props);
	
	/*Now Assign Random generated Mac address to the Interfaces*/
	interface_assign_mac_address(&link->intf1);
	interface_assign_mac_address(&link->intf2);
}

void print_graph(graph_t *graph)
{
	printf("Topology Name: %s\n",graph->topology_name);
	node_t *node=NULL;
	glthread_t *curr=NULL;
	ITERATE_GRAPH_BEGINS(&(graph->node_list),node)
	{
		print_node(node);
	}ITERATE_GRAPH_ENDS;
}

void print_node(node_t *node)
{
	unsigned int i;
    	interface_t *intf;

    	printf("Device Name = %s : \n", node->node_name);
    	for( i=0 ; i < MAX_INTF_PER_NODE; i++){
        
        intf = node->intf[i];
        if(!intf) break;
        print_interface(intf);
    }
}

void print_interface(interface_t *intf)
{
	link_t *link=intf->link;
	node_t *neighbour_node = get_neighbour_node(intf);
	
	printf("Interface Name : %s ::\n\t Local Node : %s, Neighbour Node : %s, cost = %u\n",intf->intf_name,intf->att_node->node_name,
	neighbour_node->node_name, link->cost); 
}






