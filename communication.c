
#include "communication.h"
#include "graph.h"
//#include "Layer2/layer2.h"
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h> /*for struct hostent*/

static char recv_buffer[MAX_PACKET_BUFFER_SIZE];
static char send_buffer[MAX_PACKET_BUFFER_SIZE];

static unsigned int udp_port_number = 40000;

static unsigned int 
get_next_udp_port_number(){
    
    return udp_port_number++;
}


void
init_udp_socket(node_t *node){

    if(node->udp_port_number)
        return;
    
    node->udp_port_number = get_next_udp_port_number();
     
    int udp_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    if(udp_sock_fd == -1){
        printf("Socket Creation Failed for node %s\n", node->node_name);
        return;   
    }
    
    
    struct sockaddr_in node_addr;
    
    /*
    AF_INET is an address family that is used to designate the type of addresses that your socket can communicate with (in thiscase,       Internet Protocol v4 addresses). When you create a socket, you have to specify its address family, and then you can only use addresses of that type with the socket. 
    */
    node_addr.sin_family      = AF_INET;
    
    node_addr.sin_port        = node->udp_port_number;	//A 16-bit port number in Network Byte Order.
    
    /*
    If the (sin_addr.s_addr) field is set to the constant INADDR_ANY, as defined in netinet/in.h, the caller is requesting that the socket be bound to all network interfaces on the host. Subsequently, UDP packets and TCP connections from all interfaces (which match the bound name) are routed to the application. This becomes important when a server offers a service to multiple networks. By leaving the address unspecified, the server can accept all UDP packets and TCP connection requests made for its port, regardless of the network interface on which the requests arrived.
    */
    node_addr.sin_addr.s_addr = INADDR_ANY;
    
    
    // Typecasting sockaddr_in into soc
    
    if (bind(udp_sock_fd, (struct sockaddr *)&node_addr, sizeof(struct sockaddr)) == -1) {
        printf("Error : socket bind failed for Node %s\n", node->node_name);
        return;
    }

    node->udp_sock_fd = udp_sock_fd;
}

// USING SOCKET PROGRAMING , ROUTING DATA FROM ONE PORT TO ANOTHER PORT.

static int _send_pkt_out(int sock_fd, char *pkt_data, unsigned int pkt_size, unsigned int dst_udp_port_no){

    int rc;
    struct sockaddr_in dest_addr;
    
    /*ethernet_hdr_t *ethernet_hdr = (ethernet_hdr_t *)pkt_data;
    arp_hdr_t *arp_hdr = (arp_hdr_t *)(ethernet_hdr->payload);
    printf("%d\n",arp_hdr->op_code);
    */
    
    pkt_data= (char* )pkt_data;
   
    struct hostent *host = (struct hostent *) gethostbyname("127.0.0.1"); 	//loopback IP;
    dest_addr.sin_family = AF_INET;										
    dest_addr.sin_port = dst_udp_port_no;
    dest_addr.sin_addr = *((struct in_addr *)host->h_addr);

    rc = sendto(	sock_fd, 				// sendto function sends data to the underlying OS
    			pkt_data, pkt_size, 
    			0, 
        		(struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
    
    return rc;
}

/*Public APIs to be used by the other modules*/
int send_pkt_out(char *pkt, unsigned int pkt_size, interface_t *interface){

    int rc = 0;

    node_t *sending_node = interface->att_node;		// structure of interface;
    node_t *nbr_node = get_neighbour_node(interface);	// defined in graph.h;
    
    if(!nbr_node)
        return -1;

    unsigned int dst_udp_port_no = nbr_node->udp_port_number;		
    
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );	// socket is created because data is transmitted using socket from one node 										to other node using UDP protocol	

    if(sock < 0){
        printf("Error : Sending socket Creation failed , errno = %d", errno);
        return -1;
    }
    
    // other interface is required to know on which interface , data is received.
    
    interface_t *other_interface = (&interface->link->intf1 == interface) ? &interface->link->intf2 : &interface->link->intf1;

    memset(send_buffer, 0, MAX_PACKET_BUFFER_SIZE);	// every time set to 0 because there might be some data from the last packet;

    char *pkt_with_aux_data = send_buffer;	// stores data + receiving interface

    strncpy(pkt_with_aux_data, other_interface->intf_name, IF_NAME_SIZE);	// initial bytes of packet is for INTERFACE

    pkt_with_aux_data[IF_NAME_SIZE] = '\0';	// Interface name terminated.

    memcpy(pkt_with_aux_data + IF_NAME_SIZE, pkt, pkt_size);	// copying data from the packet sent by user after interface name and 								interface name will be seggregated after.

    rc = _send_pkt_out(	sock, 
    				pkt_with_aux_data, 
    				pkt_size + IF_NAME_SIZE, 
                     		dst_udp_port_no	);		

    close(sock);
    return rc; 	// Bytes of data sent;
}

//ENTRY POINT INTO DATA LINK LAYER FROM THE PHYSICAL LAYER;
 
extern void layer2_frame_recv(node, interface, pkt, pkt_size ); //for scope of the functio in this file.
 
int pkt_receive(node_t *node, 		// Actual representation of a node receiving packet from neighbouring node
		interface_t *interface,
            	char *pkt, 
            	unsigned int pkt_size){

    
    //printf("Message Received: %s\nNode: %s\nInterface: %s\n",pkt,node->node_name,interface->intf_name);
    
    /*
    Make room in the packet buffer by shifting the data toward right so that tcp/ip stack can append more hdrs to the packet 
    as required 
    */
      
    pkt = pkt_buffer_shift_right(pkt, pkt_size, MAX_PACKET_BUFFER_SIZE - IF_NAME_SIZE);			//defined in net.c
    
    /*Do further processing of the pkt here*/
    layer2_frame_recv(node, interface, pkt, pkt_size );		//defined in layer2.c 
    
    return 0;
} 


static void _pkt_receive(	node_t *receving_node, 	// This API will seggregate the data sent by OS ;
            			char *pkt_with_aux_data, 
           			unsigned int pkt_size){
            
    char *recv_intf_name = pkt_with_aux_data;		// recv_intf_name contains only the interface name;
    interface_t *recv_intf = get_interface_by_name(receving_node, recv_intf_name);	//defined in graph.h

    if(!recv_intf){
        printf("Error : Pkt received on unknown interface %s on node %s\n", 
                    recv_intf->intf_name, receving_node->node_name);
        return;
    }

    pkt_receive(	receving_node, 
    			recv_intf, 
    			pkt_with_aux_data + IF_NAME_SIZE, 
               	pkt_size - IF_NAME_SIZE	);
}

static void * _network_start_pkt_receiver_thread(void *arg){		//void *_network_start_pkt_receiver_thread(void *topo)

    node_t *node=NULL;
    glthread_t *curr;
    	
    fd_set active_sock_fd_set,		// Sets that will store the UDP sockets file descriptor of all the nodes.
           backup_sock_fd_set;
    
    int sock_max_fd = 0;
    int bytes_recvd = 0;
    
    graph_t *topo = (void *)arg;

    int addr_len = sizeof(struct sockaddr);

    FD_ZERO(&active_sock_fd_set);		// initialize set of file descriptors to 0
    FD_ZERO(&backup_sock_fd_set);
    
    struct sockaddr_in sender_addr;
    
    ITERATE_GRAPH_BEGINS(&topo->node_list, node){

        if(!node->udp_sock_fd) 		//checking whether socket_descriptor exist or not,
            continue;

        if(node->udp_sock_fd > sock_max_fd)		// Maintaining maximum socket_fd
            sock_max_fd = node->udp_sock_fd;	// Requirement of the Select function to find the max numerical value of the function 								descriptor.

        FD_SET(node->udp_sock_fd, &backup_sock_fd_set);	// just like memcpy, here copying udp descriptors in backup_socket_fd
            
    }ITERATE_GRAPH_ENDS;

    while(1){					//  start listening on all these UDP file descriptor infinite times;

        memcpy(&active_sock_fd_set, &backup_sock_fd_set, sizeof(fd_set));	//Since the file descriptor sets are modified by select, the 										sets need to be re-initialized for the next select call.

        select(sock_max_fd + 1, &active_sock_fd_set, NULL, NULL, NULL); //Monitoring all the UDP descriptors using select system call;
        
        /*
        Select system call remains blocked until any of the file descriptor is not set. Once any one or more than one file descriptor gets 	 set,select system call unblocks and execute the remaining code;
        */

        ITERATE_GRAPH_BEGINS(&topo->node_list, node){	// starts checking which descriptor has received the request:

            if(FD_ISSET(node->udp_sock_fd, &active_sock_fd_set)){	// checking whether descriptor is activated or not.
    
                memset(recv_buffer, 0, MAX_PACKET_BUFFER_SIZE);
                
                bytes_recvd = recvfrom (	node->udp_sock_fd, 
                				(char *)recv_buffer, 
                        			MAX_PACKET_BUFFER_SIZE, 	
                        			0, 
                        			(struct sockaddr *)&sender_addr, 	// for storing senders address,
                        			&addr_len	);
                
                _pkt_receive(node, recv_buffer, bytes_recvd);  // pointer to receiving node, recieving buffer, and message size;
            }
            
         }ITERATE_GRAPH_ENDS;
    }
}


void
network_start_pkt_receiver_thread(graph_t *topo){

    pthread_attr_t attr;
    pthread_t recv_pkt_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(	&recv_pkt_thread, 			//
    			&attr, 				//
        	        _network_start_pkt_receiver_thread,  //A function to run in the thread. The function must return void * and take a 								void * argument. parameter is a function pointer.
                    	(void *)topo	);		// a pointer to the data that function will receive.
}

