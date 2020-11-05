#ifndef __COMM__
#define __COMM__

#define MAX_PACKET_BUFFER_SIZE   2048


// declaring because this filr don't know about these structures
typedef struct node_ node_t;
typedef struct interface_ interface_t;
typedef struct glthread_ glthread_t;

/* API to send the packet out of the interface.
 * Nbr node must receieve the packet on other end
 * of the link*/
int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface);

/*API to receive packet from interface*/		// entry point to data link layer
int pkt_receive(node_t *node, interface_t *interface, 
            char *pkt, unsigned int pkt_size);

/* API to flood the packet out of all interfaces
 * of the node*/
 int
send_pkt_flood(node_t *node, interface_t *exempted_intf, 
                char *pkt, unsigned int pkt_size);

#endif
