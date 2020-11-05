#include "graph.h"
#include <stdio.h>
#include "CommandParser/libcli.h"

extern graph_t *build_first_topo();
extern graph_t *build_simple_l2_switch_topo();
extern void nw_init_cli();
graph_t *topo=NULL;



int main()
{
	network_init_cli();
	// topo = build_linear_topo();
	topo = build_simple_l2_switch_topo();
	print_graph(topo);
	
	/*sleep(2);	// giving some time to start the thread;
	
	
	node_t *node = get_node_by_node_name(topo,"Router0");
	interface_t *intf= get_interface_by_name(node,"eth0/0");
	char msg[30]="Hello,how are you?\0";
	send_pkt_out(msg,strlen(msg),intf);
	*/
	
	start_shell();
	return 0;
}
