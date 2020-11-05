#ifndef __GRAPH__
#define __GRAPH

#include "gluethread/glthread.h"
#include "net.h"
#include "stddef.h"

#define MAX_INTF_PER_NODE 10
#define NODE_NAME_SIZE 16
#define IF_NAME_SIZE 16

// FORWARD DECLARTIONS:
typedef struct interface_ interface_t;
typedef struct link_ link_t;


typedef struct graph_{
	char topology_name[32];
	glthread_t node_list;
}graph_t;

								//				  _______________
typedef struct node_{						//			 ________|_______________|______________
	char node_name[NODE_NAME_SIZE];			//			|___|___|___|___|___|___|___|___|___|___|
	interface_t *intf[MAX_INTF_PER_NODE];	   //POINTER USAGE OF STRUCT:			 |_______|________|
    	glthread_t graph_glue;		
    	node_network_prop_t node_nw_prop;	
    	unsigned int udp_port_number;		//Unique Port Number.
    	int udp_sock_fd;			// Socket file descriptor.		
};
GLTHREAD_TO_STRUCT(graph_glue_to_node, node_t, graph_glue);


typedef struct interface_{
	char intf_name[IF_NAME_SIZE];
	node_t *att_node;			// POINTER USAGE OF STRUCT:
	link_t *link;				// POINTER USAGE OF STRUCT:
	interface_network_prop_t intf_nw_props;
}interface_t;


typedef struct link_{
	interface_t intf1;			// COMPELETE USAGE OF STRUCT:
	interface_t intf2;			// COMPELETE USAGE OF STRUCT:
	unsigned int cost;
}link_t;


graph_t *create_new_graph(char *topology_name);

node_t *create_graph_node(graph_t *graph,char *node_name);

void insert_link_bw_two_nodes(node_t *node1, node_t *node2,char * from, char *to, unsigned int cost);

void print_graph(graph_t *graph);

void print_node(node_t *node);

void print_interface(interface_t *intf);

int find_empty_slot(interface_t *intf);

graph_t *build_first_topo();

graph_t *build_linear_topo();

/*inline void initialize_node_interfaces(node_t *node)
{
	int i;
	for(i=0;i<MAX_INTF_PER_NODE;i++)
		node->intf[i]=NULL;
}*/

static inline int get_available_interface(node_t *node)	//why inline functions are static?
{
	int i;
	for(i=0;i<MAX_INTF_PER_NODE;i++)
	{
		if(node->intf[i]!=NULL)
			continue;
		return i;
	}
	return -1;
}

static inline node_t *get_neighbour_node(interface_t *intf)
{
	link_t *link=intf->link;
	if(&link->intf1 == intf)
        	return link->intf2.att_node;
    	else
        	return link->intf1.att_node;
}

static inline interface_t *get_interface_by_name(node_t *node, char *interface_name)
{
	int i=0;
	interface_t *interface;
	for(i=0;i<MAX_INTF_PER_NODE;i++)
	{
		interface=node->intf[i];
		if(!interface) return NULL;
		if(strncmp(interface_name,interface->intf_name,IF_NAME_SIZE)==0){
			return interface;
		}
	}
	return NULL;
}

static inline node_t * get_node_by_node_name(graph_t *topo, char *node_name){

    node_t *node;
    glthread_t *curr;    

    ITERATE_GRAPH_BEGINS(&topo->node_list, curr){

		node = graph_glue_to_node(curr);
        if(strncmp(node->node_name, node_name, strlen(node_name)) == 0)
            return node;
    } ITERATE_GRAPH_ENDS;
    return NULL;
}


#endif
