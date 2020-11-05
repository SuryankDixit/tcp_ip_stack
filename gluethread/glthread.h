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

#define BASE(glthreadptr)   ((glthreadptr)->right)

#define ITERATE_GRAPH_BEGINS(glthreadptrstart, glthreadptr)	\
{						\
	 glthread_t *_glthread_ptr = NULL;                                                              \
    glthreadptr = BASE(glthreadptrstart);                                                          \
    for(; glthreadptr!= NULL; glthreadptr = _glthread_ptr){                                        \
        _glthread_ptr = (glthreadptr)->right;
		
#define ITERATE_GRAPH_ENDS }}	

#define GLTHREAD_TO_STRUCT(fn_name, structure_name, field_name)                        \
    static inline structure_name * fn_name(glthread_t *glthreadptr){                   \
        return (structure_name *)((char *)(glthreadptr) - (char *)&(((structure_name *)0)->field_name)); \
    }


#endif
