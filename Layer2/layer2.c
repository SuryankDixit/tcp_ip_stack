#include "graph.h"
#include <stdio.h>
#include "layer2.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <assert.h>
//#include <utils.h>

void layer2_frame_recv(node_t *node, interface_t *interface,char *pkt, unsigned int pkt_size){

    ethernet_hdr_t *ethernet_hdr = (ethernet_hdr_t *)pkt;
    
    if(l2_frame_recv_qualify_on_interface(interface, ethernet_hdr) == FALSE){
        
        printf("L2 Frame Rejected");
        return;
    }

    printf("L2 Frame Accepted\n");

	/*Handle Reception of a L2 Frame on L3 Interface*/
    if(IS_INTF_L3_MODE(interface)){
		switch(ethernet_hdr->type){

			case ARP_MSG:
				{
					/*Can be ARP Broadcast or ARP reply*/
					arp_hdr_t *arp_hdr = (arp_hdr_t *)(ethernet_hdr->payload);
					// printf("%d\n",arp_hdr->op_code);
					switch(arp_hdr->op_code){
						case ARP_BROAD_REQ:
							// printf("hello1\n");
							process_arp_broadcast_request(node, interface, ethernet_hdr);
							break;
						case ARP_REPLY:
							// printf("hello2\n");
							process_arp_reply_msg(node, interface, ethernet_hdr);
							break;
						default:
							break;
					}
				}
				break;
			default:
				//promote_pkt_to_layer3(node, interface, pkt, pkt_size);
				break;
		}
	}
	else if(IF_L2_MODE(interface) == ACCESS ||
                IF_L2_MODE(interface) == TRUNK){

        l2_switch_recv_frame(interface, pkt, pkt_size);
    }
    else
        return; /*Do nothing, drop the packet*/
}

void init_arp_table(arp_table_t **arp_table){

    *arp_table = calloc(1, sizeof(arp_table_t));
    initialize_glthread(&((*arp_table)->arp_entries));
}

arp_entry_t* arp_table_lookup(arp_table_t *arp_table, char* ip_add)
{
	
	glthread_t *arp_entry= &(arp_table->arp_entries);
	glthread_t *temp= arp_entry->right;
	for(;temp!=NULL;temp=temp->right)
	{
		arp_entry_t *arp =(arp_entry_t *)((char *)temp - offsetof(arp_entry_t,arp_glue));
		if(strncmp(arp->ip_addr.ip_add, ip_add, 16) == 0)		// compares null terminated string
            		return arp;
	}
	return NULL;
}

void delete_arp_table_entry(arp_table_t *arp_table, char* ip_add)
{
	arp_entry_t *arp_entry= arp_table_lookup(arp_table,ip_add);
	
	if(!arp_entry)
        return;

	delete_node(&arp_entry->arp_glue);	//defined in glthread.c
	free(arp_entry);
}

bool_t arp_table_entry_add( arp_table_t *arp_table, arp_entry_t *arp_entry)
{
	arp_entry_t *old_arp_entry = arp_table_lookup( arp_table, arp_entry->ip_addr.ip_add );
	
	if(old_arp_entry && (memcmp(arp_entry, old_arp_entry, sizeof(arp_entry)==0)))	// compare bytes 
		return FALSE;
	if(old_arp_entry)
		delete_arp_table_entry(arp_table, arp_entry->ip_addr.ip_add);
	
	initialize_glthread(&(arp_entry->arp_glue));
	add_node_front(&(arp_table->arp_entries),&(arp_entry->arp_glue));	// in glthread.c
	return TRUE;
}

void arp_table_update_from_arp_reply( arp_table_t *arp_table,
					arp_hdr_t *arp_hdr,		// replied arp header
					interface_t *iif	)
{

	/* 1. => inet_ntop(AF_INET, &src_ip, &arp_entry->ip_addr.ip_addr, 16);
	   inet_ntop - convert IPv4 and IPv6 addresses from binary to text form;
	   
	   2. => AF_INET => IPv4 network address in the dotted-decimal format, "ddd.ddd.ddd.ddd".
	   
	   3. => htonl => converts host byte order into the network byte order. Useful when we send data from one endian to other endian.
	   		   To make simulation real we are using this.	
	   
	   
	*/
	
	unsigned int src_ip;
	assert( arp_hdr->op_code == ARP_REPLY);
	arp_entry_t *arp_entry = calloc(1,sizeof(arp_entry_t));
	
	src_ip = htonl(arp_hdr->src_ip);
	
        inet_ntop(AF_INET, &src_ip, &arp_entry->ip_addr.ip_add, 16);
        
        
        arp_entry->ip_addr.ip_add[15] = '\0';
        memcpy(arp_entry->mac_addr.mac, arp_hdr->src_mac.mac, sizeof(mac_add_t));
        strncpy(arp_entry->oif_name, iif->intf_name, IF_NAME_SIZE);
        
        bool_t flag=arp_table_entry_add(arp_table,arp_entry);
        if(flag==FALSE)
        	free(arp_entry);
        
}

void print_arp_table(arp_table_t *arp_table){

        glthread_t *arp_entry= &(arp_table->arp_entries);
	glthread_t *temp= arp_entry->right;
	for(;temp!=NULL;temp=temp->right)
	{
	
		arp_entry_t *arp =(arp_entry_t *)((char *)temp - offsetof(arp_entry_t,arp_glue));
		 printf("IP : %s, MAC : %u:%u:%u:%u:%u:%u, Outgoing Interface = %s\n", 
									    arp->ip_addr.ip_add, 
									    arp->mac_addr.mac[0], 
									    arp->mac_addr.mac[1], 
									    arp->mac_addr.mac[2], 
									    arp->mac_addr.mac[3], 
									    arp->mac_addr.mac[4], 
									    arp->mac_addr.mac[5], 
									    arp->oif_name);
	}
}

void send_arp_broadcast_request( node_t *node, interface_t *oif, char *ip_add)
{
	// In layer 2: oif = NULL ,because CLI only passes node name and ip address;
	
	unsigned int payload_size = sizeof(arp_hdr_t);
	//printf("HII-2");
	
	// since payload is ARPHEADER in layer 2 ethernet header;
	ethernet_hdr_t *ethernet_hdr = calloc(1, ETH_HDR_SIZE_EXCL_PAYLOAD+payload_size);  
	
	if(oif==NULL){
		oif = node_get_matching_subnet_interface(node, ip_add);
		if(!oif){
            		printf("Error : %s : No eligible subnet for ARP resolution for Ip-address : %s",node->node_name, ip_add);
            		return;
        	}
	}
	
	//printf("HII");
	
	// STEP 1 : Prepare ethernet hdr
	layer2_fill_with_broadcast_mac( ethernet_hdr->dst_mac.mac);		// utils.h
	memcpy( ethernet_hdr->src_mac.mac, oif->intf_nw_props.mac_add.mac, sizeof(mac_add_t));
	ethernet_hdr->type = ARP_MSG ; 
	
	// STEP 2 : Prepare ARP hdr:
	arp_hdr_t *arp_hdr= (arp_hdr_t*)ethernet_hdr->payload;
	arp_hdr->hw_type= 1;
	arp_hdr->proto_type=0x0800;
	arp_hdr->hw_addr_len= sizeof(mac_add_t);
	arp_hdr->proto_addr_len= 4;
	arp_hdr->op_code= ARP_BROAD_REQ;
	
	memcpy(arp_hdr->src_mac.mac, oif->intf_nw_props.mac_add.mac, sizeof(mac_add_t));

        inet_pton(AF_INET, oif->intf_nw_props.ip_add.ip_add, &arp_hdr->src_ip);		// converting IP to equivalent integer;
        arp_hdr->src_ip = htonl(arp_hdr->src_ip);			// converting host bytes to network bytes;

        memset(arp_hdr->dst_mac.mac, 0,  sizeof(mac_add_t));

        inet_pton(AF_INET, ip_add, &arp_hdr->dst_ip);		// converting IP to equivalent integer;
        arp_hdr->dst_ip = htonl(arp_hdr->dst_ip);			// converting host bytes to network bytes;

        // ethernet_hdr->FCS = 0; /*Not used*/
    
        ETH_FCS(ethernet_hdr, sizeof(arp_hdr_t)) = 0;		// using like this because payload size is variable;

        /*STEP 3 : Now dispatch the ARP Broadcast Request Packet out of interface*/
        send_pkt_out((char *)ethernet_hdr,  ETH_HDR_SIZE_EXCL_PAYLOAD + sizeof(arp_hdr_t), oif);

        free(ethernet_hdr);
}

void send_arp_reply_msg(ethernet_hdr_t *ethernet_hdr, interface_t *oif)
{
	//printf("ARP_REPLY");
	arp_hdr_t *arp_hdr = (arp_hdr_t *)(ethernet_hdr->payload);

    	ethernet_hdr_t *ethernet_hdr_reply = (ethernet_hdr_t *)calloc(1, sizeof(ethernet_hdr_t) + sizeof(arp_hdr_t));
    	
    	memcpy( ethernet_hdr_reply->dst_mac.mac, arp_hdr->src_mac.mac, sizeof(mac_add_t));
    	memcpy( ethernet_hdr_reply->src_mac.mac, oif->intf_nw_props.mac_add.mac, sizeof(mac_add_t));
    	ethernet_hdr_reply->type= ARP_MSG;
    	
    	arp_hdr_t *arp_hdr_reply= (arp_hdr_t *)(ethernet_hdr_reply->payload);
    	arp_hdr_reply->hw_type = 1;
    	arp_hdr_reply->proto_type = 0x0800;
    	arp_hdr_reply->hw_addr_len= sizeof(mac_add_t);
	arp_hdr_reply->proto_addr_len= 4;
	arp_hdr_reply->op_code= ARP_REPLY;
	
	/*memcpy( arp_hdr_reply->src_mac.mac, oif->intf_nw_props.mac_add.mac, sizeof(mac_add_t));
	inet_pton(AF_INET, oif->intf_nw_props.ip_add.ip_add, &arp_hdr_reply->src_ip);	// converting IP to equivalent integer;
        arp_hdr_reply->src_ip = htonl(arp_hdr->src_ip);			// converting host bytes to network bytes;

	memcpy( arp_hdr_reply->src_mac.mac, arp_hdr->dst_mac.mac, sizeof(mac_add_t));
	arp_hdr_reply->dst_ip=arp_hdr->src_ip;*/
	memcpy(arp_hdr_reply->src_mac.mac, oif->intf_nw_props.mac_add.mac, sizeof(mac_add_t));

    inet_pton(AF_INET, oif->intf_nw_props.ip_add.ip_add, &arp_hdr_reply->src_ip);
    arp_hdr_reply->src_ip =  htonl(arp_hdr_reply->src_ip);

    memcpy(arp_hdr_reply->dst_mac.mac, arp_hdr->src_mac.mac, sizeof(mac_add_t));
    arp_hdr_reply->dst_ip = arp_hdr->src_ip;
	
	//unsigned int total_pkt_size = ETH_HDR_SIZE_EXCL_PAYLOAD + sizeof(arp_hdr_t);
	
	//char *shifted_pkt_buffer = pkt_buffer_shift_right((char *)ethernet_hdr_reply, 
                               //total_pkt_size, sizeof(ethernet_hdr_t) + sizeof(arp_hdr_t));

	
	//send_pkt_out(shifted_pkt_buffer, total_pkt_size,oif);
	send_pkt_out((char *)ethernet_hdr_reply, sizeof(ethernet_hdr_t) + sizeof(arp_hdr_t),oif);

	free(ethernet_hdr_reply);  
}

void process_arp_reply_msg(node_t *node, interface_t *iif,ethernet_hdr_t *ethernet_hdr){

	printf("%s : ARP reply msg received on interface %s of node %s\n",__FUNCTION__, iif->intf_name , iif->att_node->node_name);

	arp_table_update_from_arp_reply( node->node_nw_prop.arp_table,(arp_hdr_t *)(ethernet_hdr->payload), iif);    
}


void process_arp_broadcast_request(node_t *node, interface_t *iif, ethernet_hdr_t *ethernet_hdr)
{
	printf("%s : ARP Broadcast messagesg recvd on interface %s of node %s\n",__FUNCTION__, iif->intf_name , iif->att_node->node_name); 
	
	char dest_ip[16];
	arp_hdr_t *arp_hdr = (arp_hdr_t *)(ethernet_hdr->payload);
	
	unsigned int arp_dst_ip = htonl(arp_hdr->dst_ip);	//network bytes;

    	inet_ntop(AF_INET, &arp_dst_ip, dest_ip, 16);		// converts unsigned IP into string form(a.b.c.d);
    	dest_ip[15] = '\0';
    
    	if(strncmp(iif->intf_nw_props.ip_add.ip_add, dest_ip, 16)){
        
        printf("%s : ARP Broadcast req msg dropped, Dst IP address did not match", 
                node->node_name );
        return;
    }

   send_arp_reply_msg(ethernet_hdr, iif);
}


/*Interface config APIs for L2 mode configuration*/

void
interface_set_l2_mode(node_t *node, 
                      interface_t *interface, 
                      char *l2_mode_option){

    intf_l2_mode_t intf_l2_mode;

    if(strncmp(l2_mode_option, "access", strlen("access")) == 0){
        intf_l2_mode = ACCESS;    
    }
    else if(strncmp(l2_mode_option, "trunk", strlen("trunk")) ==0){
        intf_l2_mode = TRUNK;
    }
    else{
        assert(0);
    }

    /*Case 1 : if interface is working in L3 mode, i.e. IP address is configured.
     * then disable ip address, and set interface in L2 mode*/
    if(IS_INTF_L3_MODE(interface)){
        interface->intf_nw_props.is_ipadd_config_backup = TRUE;
        interface->intf_nw_props.is_ip_configured = FALSE;

        IF_L2_MODE(interface) = intf_l2_mode;
        return;
    }

    /*Case 2 : if interface is working neither in L2 mode or L3 mode, then
     * apply L2 config*/
    if(IF_L2_MODE(interface) == L2_MODE_UNKNOWN){
        IF_L2_MODE(interface) = intf_l2_mode;
        return;
    }

    /*case 3 : if interface is operating in same mode, and user config same mode
     * again, then do nothing*/
    if(IF_L2_MODE(interface) == intf_l2_mode){
        return;
    }

    /*case 4 : if interface is operating in access mode, and user config trunk mode,
     * then overwrite*/
    if(IF_L2_MODE(interface) == ACCESS &&
            intf_l2_mode == TRUNK){
        IF_L2_MODE(interface) = intf_l2_mode;
        return;
    }

    /* case 5 : if interface is operating in trunk mode, and user config access mode,
     * then overwrite, remove all vlans from interface, user must enable vlan again 
     * on interface*/
    if(IF_L2_MODE(interface) == TRUNK &&
           intf_l2_mode == ACCESS){

        IF_L2_MODE(interface) = intf_l2_mode;

        unsigned int i = 0;

        // for ( ; i < MAX_VLAN_MEMBERSHIP; i++){
        //     interface->intf_nw_props.vlans[i] = 0;
        // }
    }
}




/*APIs to be used to create topologies*/
void
node_set_intf_l2_mode(node_t *node, char *intf_name, 
                        intf_l2_mode_t intf_l2_mode){

    interface_t *interface = get_interface_by_name(node, intf_name);
    assert(interface);

    interface_set_l2_mode(node, interface, intf_l2_mode_str(intf_l2_mode));
}






