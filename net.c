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
	ITERATE_GRAPH_BEGINS(&(graph->node_list),curr)
	{
        node = graph_glue_to_node(curr);
		print_network_node(node);
	}ITERATE_GRAPH_ENDS;
}

char* pkt_buffer_shift_right(char *pkt,unsigned int pkt_size, unsigned int total_buffer_size)
{
	 char *temp = NULL;
	 bool_t need_temp_memory = FALSE;

	    if(pkt_size * 2 > total_buffer_size){
		need_temp_memory = TRUE;
	    }
	    
	    if(need_temp_memory){
		temp = calloc(1, pkt_size);
		memcpy(temp, pkt, pkt_size);
		memset(pkt, 0, total_buffer_size);
		memcpy(pkt + (total_buffer_size - pkt_size), temp, pkt_size);
		free(temp);
		return pkt + (total_buffer_size - pkt_size);
	    }
	    
	    memcpy(pkt + (total_buffer_size - pkt_size), pkt, pkt_size);
	    memset(pkt, 0, pkt_size);
	    return pkt + (total_buffer_size - pkt_size);
}

//interface_t *node_get_matching_subnet_interface(node_t *node, char *ip_addr){

    /*int i = 0;
    interface_t *intf;

    char intf_addr[16];
    /*char mask;
    char intf_subnet[16];
    char subnet2[16];*/
    
	/*printf("HII-subnet\n");
	
	int j;
	for(j=0;j<strlen(ip_addr);j++)
		printf("%c",ip_addr[j]);
	printf("\n");
	//printf("\n %s->\n",*ip_addr);
    for( ; i < MAX_INTF_PER_NODE; i++){
    
        intf = node->intf[i];
        if(!intf) return NULL;

        if(intf->intf_nw_props.is_ip_configured == FALSE)
            continue;
        
        strncpy(intf_addr,intf->intf_nw_props.ip_add.ip_add,16);
        
        for(j=0;j<strlen(intf_addr);j++)
		printf("%c",intf_addr[j]);
	printf("\n");
        
        //printf("%s\n",*intf_addr);
       /* mask = intf->intf_nw_props.mask;

        memset(intf_subnet, 0 , 16);
        memset(subnet2, 0 , 16);
        apply_mask(intf_addr, mask, intf_subnet);
        apply_mask(ip_addr, mask, subnet2);*/
        
        /*if(strcmp(intf_addr, ip_addr) == 0)
            return intf;
    }*/
   /* unsigned int i = 0;
    interface_t *intf;

    char *intf_addr = NULL;
    char mask;
    char intf_subnet[16];
    char subnet2[16];

    for( ; i < MAX_INTF_PER_NODE; i++){
    
        intf = node->intf[i];
        if(!intf) return NULL;

        if(intf->intf_nw_props.is_ip_configured == FALSE)
            continue;
        
        intf_addr = intf->intf_nw_props.ip_add.ip_add;
        mask = intf->intf_nw_props.mask;

        memset(intf_subnet, 0 , 16);
        memset(subnet2, 0 , 16);
        apply_mask(intf_addr, mask, intf_subnet);
        apply_mask(ip_addr, mask, subnet2);
        
        if(strncmp(intf_subnet, subnet2, 16) == 0)
            return intf;
    }
}*/

interface_t *
node_get_matching_subnet_interface(node_t *node, char *ip_addr){

    unsigned int i = 0;
    interface_t *intf;

    char *intf_addr = NULL;
    char mask;
    char intf_subnet[16];
    char subnet2[16];

    for( ; i < MAX_INTF_PER_NODE; i++){
    
        intf = node->intf[i];
        if(!intf) return NULL;

        if(intf->intf_nw_props.is_ip_configured == FALSE)
            continue;
        
        intf_addr = intf->intf_nw_props.ip_add.ip_add;
        mask = intf->intf_nw_props.mask;

        memset(intf_subnet, 0 , 16);
        memset(subnet2, 0 , 16);
        apply_mask(intf_addr, mask, intf_subnet);
        apply_mask(ip_addr, mask, subnet2);
        
        if(strncmp(intf_subnet, subnet2, 16) == 0){
            return intf;
        }
    }
}
