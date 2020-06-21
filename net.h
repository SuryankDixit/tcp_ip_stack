
#ifndef __NET__
#define __NET__

#include<stdbool.h>
#include <memory.h>
#include "utils.h"

typedef struct graph_ graph_t;			// these declarations are necessary as net.h don't know about these structures.
typedef struct interface_ interface_t;
typedef struct node_ node_t;
typedef struct arp_table_ arp_table_t;

#pragma pack (push,1)
typedef struct ip_add_{
	unsigned char ip_add[16];
}ip_add_t;

typedef struct mac_add_ {
	unsigned char mac[6];
} mac_add_t;
#pragma pack(pop)

typedef struct node_network_prop_{

	bool is_loopback_ip_configured;
	ip_add_t loopback_ip;
	arp_table_t *arp_table;
	
}node_network_prop_t;

typedef struct interface_network_prop{

	mac_add_t mac_add;		// each interface will have Mac address, so add while MAC while creating interfaces.
	
	bool_t is_ip_configured;
	ip_add_t ip_add;
	int mask;
	
}interface_network_prop_t;

extern void init_arp_table(arp_table_t **arp_table);	//since the function is defined in laye2.c/.h;

static inline void init_node_nw_prop(node_network_prop_t *node_nw_prop) {

    node_nw_prop->is_loopback_ip_configured = FALSE;
    memset(node_nw_prop->loopback_ip.ip_add, 0, 16);
    init_arp_table(&(node_nw_prop->arp_table));
}


static inline void init_intf_nw_prop(interface_network_prop_t *intf_nw_props) {

    memset(intf_nw_props->mac_add.mac , 0 , 7);
    intf_nw_props->is_ip_configured = FALSE;
    memset(intf_nw_props->ip_add.ip_add, 0, 16);
    intf_nw_props->mask = 0;
}

void interface_assign_mac_address(interface_t *interface);

bool_t node_set_loopback_address(node_t *node,char *ip_add);

bool_t node_set_interface_ip_address(node_t *node,char* intf_name,char *ip_add,int mask);

void print_network_graph(graph_t *topo);

void print_network_node(node_t *node);

void print_network_interface(interface_t *interface);

char* pkt_buffer_shift_right(char *pkt,unsigned int pkt_size, unsigned int total_buffer_size);

unsigned int ip_addr_char_to_int(char *ip_addr);

void ip_addr_int_to_char(unsigned int ip_addr, char *ip_add_str);

interface_t *node_get_matching_subnet_interface(node_t *node, char *ip_addr);


#endif
