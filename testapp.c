#include "graph.h"

int main()
{
	graph_t *topo = build_first_topo();
	print_graph(topo);
	return 0;
}
