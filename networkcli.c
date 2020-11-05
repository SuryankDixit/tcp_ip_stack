
#include "CommandParser/libcli.h"	// header files that support libcli library
#include "CommandParser/cmdtlv.h"
#include "cmdcodes.h"
#include "graph.h"
#include <stdio.h>

extern graph_t *topo; 

/*General Validations*/
int
validate_node_extistence(char *node_name){

    node_t *node = get_node_by_node_name(topo, node_name);
    if(node)
        return VALIDATION_SUCCESS;
    printf("Error : Node %s do not exist\n", node_name);
    return VALIDATION_FAILED;
}


// Second arguement is the pointer to the buffer, which will hold the leaf param values passed by user.

int show_network_topology_handler(param_t *param, 
				   ser_buff_t *tlv_buf, /*This is the buffer in which all leaf values in the command are present*/
				   op_mode enable_or_disable)
{
	int cmd_code = -1;
	cmd_code = EXTRACT_CMD_CODE(tlv_buf);	// Macro defined in libcli
	
	switch(cmd_code){

        case CMDCODE_SHOW_NETWORK_TOPOLOGY:
            					print_network_graph(topo);
            					break;
        default: ;
    	}
    return 0;
}

typedef struct arp_table_ arp_table_t;
extern void
print_arp_table(arp_table_t *arp_table);
static int show_arp_handler(param_t *param, ser_buff_t *tlv_buf, 
                    op_mode enable_or_disable){

    node_t *node;
    char *node_name;
    tlv_struct_t *tlv = NULL;
    
    TLV_LOOP_BEGIN(tlv_buf, tlv){

        if(strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
            node_name = tlv->value;

    }TLV_LOOP_END;

    node = get_node_by_node_name(topo, node_name);
    print_arp_table(node->node_nw_prop.arp_table);    
    return 0;
}

typedef struct mac_table_ mac_table_t;
extern void
dump_mac_table(mac_table_t *mac_table);
static int
show_mac_handler(param_t *param, ser_buff_t *tlv_buf,
                    op_mode enable_or_disable){

    node_t *node;
    char *node_name;
    tlv_struct_t *tlv = NULL;
    
    TLV_LOOP_BEGIN(tlv_buf, tlv){

        if(strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
            node_name = tlv->value;

    }TLV_LOOP_END;

    node = get_node_by_node_name(topo, node_name);
    dump_mac_table(NODE_MAC_TABLE(node));
    return 0;

}


extern void
send_arp_broadcast_request(node_t *node,
                           interface_t *oif,
                           char *ip_addr);
static int
arp_handler(param_t *param, ser_buff_t *tlv_buf,
                op_mode enable_or_disable){

    node_t *node;
    char *node_name;
    char *ip_addr;
    tlv_struct_t *tlv = NULL;

    TLV_LOOP_BEGIN(tlv_buf, tlv){

        if(strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
            node_name = tlv->value;
        else if(strncmp(tlv->leaf_id, "ip-address", strlen("ip-address")) ==0)
            ip_addr = tlv->value;
    } TLV_LOOP_END;

    node = get_node_by_node_name(topo, node_name);
    //printf("HII-CLI");
    send_arp_broadcast_request(node, NULL, ip_addr);
    return 0;
}


// This is the first function, that needs to be executed to make the CLI tree

void network_init_cli()
{
	init_libcli();		// libcli needs to be initialized in the begining
	
	// This code is the necessary part to initialie hooks using libcli;
	// hooks are the root of the CLI tree, i.e. First layer of params;
	
	param_t *show   = libcli_get_show_hook();	
	param_t *debug  = libcli_get_debug_hook();
	param_t *config = libcli_get_config_hook();
	param_t *clear  = libcli_get_clear_hook();
	param_t *run    = libcli_get_run_hook();
	
	{
		/* show topology */
		
		// Each commented part is the guideline to use libcli library
		
		static param_t topology;    /*Get the param_t variable, either a static memory or heap memory, not stack memory*/
		
		init_param( &topology,		/*Address of the current param*/ 
			    CMD,		/*CMD for command param, LEAF for leaf param*/  // (TYPE OF PARAM)
			    "topology",	/*Name of the param, this string is what is displayed in command line*/
			    show_network_topology_handler,	/*Since this is complete command, it should invoke application routine. Pass 								the pointer to that routine here.*/
			    0,			/*Applicable only for LEAF params. Always NULL for CMD param*/  //(VALIDATION)
			    INVALID,		/*leaf param value type.*/
			    0,			/*Leaf id. Applicable only for LEAF param. Give some name to leaf-params. It is this string 						that we will parse in application code to find the value passed by the user*/ // (NULL FOR 							CMD PARAMS)
			    "Dump Complete Network Topology");	/*Help String*/
		
		libcli_register_param(show,&topology);	/*Add topology leaf param as suboption of show param. Note that: 
								show --> topology has been chained*/
		/*The below API should be called for param upto which the command is supposed to invoke application callback rouine. 
              * The CMDCODE_SHOW_NODE code is sent to application using which we find which command was triggered, and accordingly what 
             * are expected leaf params we need to parse.*/
								
		set_param_cmd_code(&topology, CMDCODE_SHOW_NETWORK_TOPOLOGY);
		
		{
		    /*show node*/    
		     static param_t node;
		     init_param(&node, CMD, "node", 0, 0, INVALID, 0, "\"node\" keyword");
		     libcli_register_param(show, &node);
		     //libcli_register_display_callback(&node, display_graph_nodes);
		     {
		        /*show node <node-name>*/ 
		         static param_t node_name;
		         init_param(&node_name, LEAF, 0, 0, validate_node_extistence, STRING, "node-name", "Node Name");
		         libcli_register_param(&node, &node_name);
		         {
		            /*show node <node-name> arp*/
		            static param_t arp;
		            init_param(&arp, CMD, "arp", show_arp_handler, 0, INVALID, 0, "\"arp\" keyword");
		            libcli_register_param(&node_name, &arp);
		            set_param_cmd_code(&arp, CMDCODE_SHOW_NODE_ARP_TABLE);
		         }
		           
		     }
		 } 
	}
	
	
	    {
        /*run node*/
        static param_t node;
        init_param(&node, CMD, "node", 0, 0, INVALID, 0, "\"node\" keyword");
        libcli_register_param(run, &node);
        //libcli_register_display_callback(&node, display_graph_nodes);
        {
            /*run node <node-name>*/
            static param_t node_name;
            init_param(&node_name, LEAF, 0, 0, validate_node_extistence, STRING, "node-name", "Node Name");
            libcli_register_param(&node, &node_name);
            {
                /*run node <node-name> ping */
                static param_t ping;
                init_param(&ping, CMD, "ping" , 0, 0, INVALID, 0, "Ping utility");
                libcli_register_param(&node_name, &ping);
            }
            {
                /*run node <node-name> resolve-arp*/    
                static param_t resolve_arp;
                init_param(&resolve_arp, CMD, "resolve-arp", 0, 0, INVALID, 0, "Resolve ARP");
                libcli_register_param(&node_name, &resolve_arp);
                {
                    /*run node <node-name> resolve-arp <ip-address>*/    
                    static param_t ip_addr;
                    init_param(&ip_addr, LEAF, 0, arp_handler, 0, IPV4, "ip-address", "Nbr IPv4 Address");
                    libcli_register_param(&resolve_arp, &ip_addr);
                    set_param_cmd_code(&ip_addr, CMDCODE_RUN_ARP);
                }
            }
        }
    }
}
