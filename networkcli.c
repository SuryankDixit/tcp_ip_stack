#include "CommandParser/libcli.h"	// header files that support libcli library
#include "CommandParser/cmdtlv.h"
#include "cmdcodes.h"
#include "graph.h"
#include <stdio.h>

extern graph_t *topo; 
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
	}
}
