#ifndef __LAYER2__
#define __LAYER2__

#include "../net.h"
#include "../gluethread/glthread.h"

#define ETH_HDR_SIZE_EXCL_PAYLOAD   \
    (sizeof(ethernet_hdr_t) - sizeof(((ethernet_hdr_t *)0)->payload))

#define ETH_FCS(eth_hdr_ptr, payload_size)  \
    (*(unsigned int *)(((char *)(((ethernet_hdr_t *)eth_hdr_ptr)->payload) + payload_size)))


#pragma pack(push,1)

typedef struct arp_hdr_{

    short hw_type;          /*1 for ethernet cable*/
    short proto_type;       /*0x0800 for IPV4*/
    char hw_addr_len;       /*6 bytes for MAC*/
    char proto_addr_len;    /*4 bytes for IPV4*/
    short op_code;          /*req(1) or reply(2)*/
    mac_add_t src_mac;      /*MAC of Outgoing Interface interface*/
    unsigned int src_ip;        /*IP of OIF*/
    mac_add_t dst_mac;      /*?*/
    unsigned int dst_ip;        /*IP for which ARP is being resolved*/
} arp_hdr_t;

typedef struct ethernet_hdr_{

    mac_add_t dst_mac;
    mac_add_t src_mac;
    short type;
    char payload[248];  /*Max allowed 1500*/
    unsigned int FCS;
} ethernet_hdr_t;
#pragma pack(pop)

static inline bool_t 
l2_frame_recv_qualify_on_interface(interface_t *interface,ethernet_hdr_t *ethernet_hdr){

    /* Presence of IP address on interface makes it work in L3 mode,
     * while absence of IP-address automatically make it work in
     * L2 mode. For interfaces working in L2 mode (L2 switch interfaces),
     * We should accept all frames. L2 switch never discards the frames
     * based on MAC addresses*/

    if(!interface->intf_nw_props.is_ip_configured)
        return FALSE;

    /*Return TRUE if receiving machine must accept the frame*/
    if(memcmp(interface->intf_nw_props.mac_add.mac, ethernet_hdr->dst_mac.mac, sizeof(mac_add_t)) == 0){

        return TRUE;
    }

    if(IS_MAC_BROADCAST_ADDR(ethernet_hdr->dst_mac.mac)){

        return TRUE;
    }

    return FALSE;
}

// ARP TABLE DATA STRUCTURE;
typedef struct arp_table_{		// Max number of entries in arp table= number of neighbours

    glthread_t arp_entries;		// it has left and right pointer which will be glued with arp_glue each time;
} arp_table_t;

typedef struct arp_entry_{

    ip_add_t ip_addr;   /*key*/
    mac_add_t mac_addr;
    char oif_name[IF_NAME_SIZE];
    glthread_t arp_glue;
} arp_entry_t;

void init_arp_table(arp_table_t **arp_table);

void print_arp_table(arp_table_t *arp_table);

void send_arp_broadcast_request( node_t *node, interface_t *oif, char *ip_add);

void send_arp_reply_msg(ethernet_hdr_t *ethernet_hdr, interface_t *oif);

void process_arp_reply_msg(node_t *node, interface_t *iif,ethernet_hdr_t *ethernet_hdr);

void process_arp_broadcast_request(node_t *node, interface_t *iif, ethernet_hdr_t *ethernet_hdr);


#endif

