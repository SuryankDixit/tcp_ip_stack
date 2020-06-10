#ifndef __GLTHREAD__
#define __GLTHREAD__


#define offsetof(struct_name, field_name)   \
    ((int)&((struct_name *)0)->field_name)


typedef struct glthread_{
	struct glthread_ *left;
	struct glthread_ *right;
}glthread_t;

void initialize_glthread(glthread_t *glthread);

void add_node_front(glthread_t *head, glthread_t *node);

#define ITERATE_GRAPH_BEGINS(head,node)	\
{						\
	glthread_t *curr = NULL;                                                                 \
        curr = (head)->right;                                                      		    \
        for(; curr!= NULL; curr = curr->right){                                        		    \
        	node = (node_t *)((char *)curr - offsetof(node_t,graph_glue));
		
#define ITERATE_GRAPH_ENDS }}	

#endif
