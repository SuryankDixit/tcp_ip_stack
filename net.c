#include "graph.h"
#include "utils.h"
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<assert.h>


unsigned int hash_code(void *ptr, unsigned int size){
    unsigned int value=0, i =0;
    char *str = (char*)ptr;
    while(i < size)
    {
        value += *str;
        value*=97;
        str++;
        i++;
    }
    return value;
}


/*Heuristics, Assign a unique mac address to interface*/
void interface_assign_mac_address(interface_t *interface){

    node_t *node = interface->att_node;
    
    if(!node)
        return;

    unsigned int hash_code_val = 0;
    hash_code_val = hash_code(node->node_name, NODE_NAME_SIZE);
    hash_code_val *= hash_code(interface->intf_name, IF_NAME_SIZE);
    memset(interface->intf_nw_props.mac_add.mac, 0, sizeof(interface->intf_nw_props.mac_add.mac));
    memcpy(interface->intf_nw_props.mac_add.mac, (char *)&hash_code_val, sizeof(unsigned int));
}


bool_t node_set_loopback_address(node_t *node,char *ip_add)
{
	//assert(ip_add);
	strncpy(node->node_nw_prop.loopback_ip.ip_add,ip_add,16);
	node->node_nw_prop.is_loopback_ip_configured= TRUE;
	node->node_nw_prop.loopback_ip.ip_add[15]='\0';
	return TRUE;
}

bool_t node_set_interface_ip_address(node_t *node,char* intf_name,char *ip_add,int mask)
{
	interface_t *interface= get_interface_by_name(node,intf_name);
	if(interface==NULL) return FALSE;
		//assert(0);	// means the condition is false and the code wriiten after this is unreachable. Also, it helps in debugging.
	
	strncpy(interface->intf_nw_props.ip_add.ip_add,ip_add,16);
	interface->intf_nw_props.ip_add.ip_add[15]='\0';
	interface->intf_nw_props.is_ip_configured = TRUE;
	interface->intf_nw_props.mask=mask;
	return TRUE;
}

void print_network_interface(interface_t *interface)
{
	print_interface(interface);
	
	if(interface->intf_nw_props.is_ip_configured){
		printf("\t IP Address : %s/%d\t",interface->intf_nw_props.ip_add.ip_add,interface->intf_nw_props.mask);
		}
	else{
		printf("\t IP Address : %s/%d\t","NIL",0);
	}
	
	printf("\t MAC : %u:%u:%u:%u:%u:%u\n", 
        interface->intf_nw_props.mac_add.mac[0], interface->intf_nw_props.mac_add.mac[1],
        interface->intf_nw_props.mac_add.mac[2], interface->intf_nw_props.mac_add.mac[3],
        interface->intf_nw_props.mac_add.mac[4], interface->intf_nw_props.mac_add.mac[5]);
	//printf("MAC ADDRESS : %s\n",interface->intf_nw_props.mac_add.mac);
}

void print_network_node(node_t *node)
{
	unsigned int i;
    	interface_t *intf;

    	printf("\n\nDevice Name = %s : ", node->node_name);
    	if(node->node_nw_prop.is_loopback_ip_configured)
    		printf("\tLoopBack IP Address: %s/32\n",node->node_nw_prop.loopback_ip.ip_add);
    		
    	for( i=0 ; i < MAX_INTF_PER_NODE; i++){
		intf = node->intf[i];
		if(!intf) break;
		print_network_interface(intf);
	    }
}

void print_network_graph(graph_t *graph)
{
	printf("Topology Name: %s\n",graph->topology_name);
	node_t *node=NULL;
	glthread_t *curr=NULL;
	ITERATE_GRAPH_BEGINS(&(graph->node_list),node)
	{
		print_network_node(node);
	}ITERATE_GRAPH_ENDS;
}



