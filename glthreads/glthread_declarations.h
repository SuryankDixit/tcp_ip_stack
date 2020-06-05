//#ifndef __A__
//#define __A__

// Structue for glued doubly linked list;

typedef struct glthread_node{
	struct glthread_node *left;
	struct glthread_node *right;
}node;

typedef struct glthread{
	node *head;
	unsigned int offset;	
}list;


// this preprocessor directive returns the offset of the desired field of structure;

#define offsetof(struct_name, field_name)   \
    ((int)&((struct_name *)0)->field_name)
    
#define INITIALIZE_LINK( link )	\
	link->left = NULL; 		\
	link->right = NULL;
	
/*#define ITERATE_LIST(list_ptr,struct_type,s)	\
	node *temp=NULL;		\
	for(temp=list_ptr->head;temp!=NULL;temp=temp->right){	\
		s=(struct_type *)((char *)temp-list_ptr->offset);	\
		print_details(s);	\
		}	*/
		
#define ITERATE_GL_THREADS_BEGIN(lstptr, struct_type, s)      \
{                                                               \
    node *_glnode = NULL;             \
    for(_glnode = lstptr->head; _glnode!=NULL; _glnode = _glnode->right){      \
        s = (struct_type *)((char *)_glnode - lstptr->offset);

#define ITERATE_GL_THREADS_ENDS }}	


//function declarations

void initialize_list( list *ptr, unsigned int offset_val );
void list_insert(list *list_ptr,node *ptr);
void print_list(list *list_ptr);
void list_delete(list *list_ptr,node *ptr);

//#endif
