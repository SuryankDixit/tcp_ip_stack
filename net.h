
#ifndef __NET__
#define __NET__

#include<stdbool.h>
#include <memory.h>
#include "utils.h"

/* Device IDS */
#define L3_ROUTER   (1 << 0)
#define L2_SWITCH   (1 << 1)
#define HUB         (1 << 2)

typedef struct graph_ graph_t;			// these declarations are necessary as net.h don't know about these structures.
typedef struct interface_ interface_t;
typedef struct node_ node_t;
typedef struct arp_table_ arp_table_t;
typedef struct mac_table_ mac_table_t;

typedef enum{

    ACCESS,
    TRUNK,
    L2_MODE_UNKNOWN
} intf_l2_mode_t;

static inline char *
intf_l2_mode_str(intf_l2_mode_t intf_l2_mode){

    switch(intf_l2_mode){
        case ACCESS:
            return "access";
        case TRUNK:
            return "trunk";
        default:
            return "L2_MODE_UNKNWON";
    }
}


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
	unsigned int flags;/* Used to find various device types capabilities of
     * the node and other features*/
	arp_table_t *arp_table;
	mac_table_t *mac_table; 
	
}node_network_prop_t;

extern void init_arp_table(arp_table_t **arp_table);	//since the function is defined in laye2.c/.h;
extern void init_mac_table(mac_table_t **mac_table);

static inline void init_node_nw_prop(node_network_prop_t *node_nw_prop) {

	node_nw_prop->flags = 0;
    node_nw_prop->is_loopback_ip_configured = FALSE;
    memset(node_nw_prop->loopback_ip.ip_add, 0, 16);
    init_arp_table(&(node_nw_prop->arp_table));
}

typedef struct interface_network_prop{

	mac_add_t mac_add;		// each interface will have Mac address, so add while MAC while creating interfaces.
	intf_l2_mode_t  intf_l2_mode;   /*if IP-address is configured on this interface, then this should be set to UNKNOWN*/
	// unsigned int vlans[MAX_VLAN_MEMBERSHIP];    /*If the interface is operating in Trunk mode, it can be a member of these many vlans*/
	bool_t is_ipadd_config_backup;

	bool_t is_ip_configured;
	ip_add_t ip_add;
	int mask;
	
}interface_network_prop_t;

static inline void init_intf_nw_prop(interface_network_prop_t *intf_nw_props) {

    memset(intf_nw_props->mac_add.mac , 0 , sizeof(intf_nw_props->mac_add.mac));
    intf_nw_props->intf_l2_mode = L2_MODE_UNKNOWN;
	// memset(intf_nw_props->vlans, 0, sizeof(intf_nw_props->vlans));


	intf_nw_props->is_ip_configured = FALSE;
    memset(intf_nw_props->ip_add.ip_add, 0, 16);
    intf_nw_props->mask = 0;
}


#define MAX_VLAN_MEMBERSHIP 10



/*GET shorthand Macros*/
#define IF_MAC(intf_ptr)   ((intf_ptr)->intf_nw_props.mac_add.mac)
#define IF_IP(intf_ptr)    ((intf_ptr)->intf_nw_props.ip_add.ip_add)

#define NODE_LO_ADDR(node_ptr) (node_ptr->node_nw_prop.loopback_ip.ip_add)
#define NODE_ARP_TABLE(node_ptr)    (node_ptr->node_nw_prop.arp_table)
#define NODE_MAC_TABLE(node_ptr)    (node_ptr->node_nw_prop.mac_table)
#define IF_L2_MODE(intf_ptr)    (intf_ptr->intf_nw_props.intf_l2_mode)
#define IS_INTF_L3_MODE(intf_ptr)   (intf_ptr->intf_nw_props.is_ip_configured == TRUE)




void interface_assign_mac_address(interface_t *interface);

/*APIs to set Network Node properties*/
bool_t node_set_device_type(node_t *node, unsigned int F);
bool_t node_set_loopback_address(node_t *node,char *ip_add);
bool_t node_set_interface_ip_address(node_t *node,char* intf_name,char *ip_add,int mask);

/*Dumping Functions to dump network information
 * on nodes and interfaces*/
void print_network_graph(graph_t *topo);
void print_network_node(node_t *node);
void print_network_interface(interface_t *interface);

char* pkt_buffer_shift_right(char *pkt,unsigned int pkt_size, unsigned int total_buffer_size);
unsigned int ip_addr_char_to_int(char *ip_addr);
void ip_addr_int_to_char(unsigned int ip_addr, char *ip_add_str);

interface_t *node_get_matching_subnet_interface(node_t *node, char *ip_addr);


#endif
