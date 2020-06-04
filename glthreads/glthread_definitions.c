#include<stdio.h>
#include<stdlib.h>
#include"glthread_declarations.h"

void initialize_list( list *ptr, unsigned int offset_val )
{
	ptr->head=NULL;
	ptr->offset=offset_val;
}

void list_insert(list *list_ptr,node *ptr)
{
	ptr->left=NULL;
	ptr->right=NULL;
	if(list_ptr->head==NULL)
	{
		list_ptr->head=ptr;
		return;
	}
	node* temp=list_ptr->head;
	temp->left=ptr;
	ptr->right=temp;
	list_ptr->head=ptr;
}

void list_delete(list *list_ptr,node *ptr)
{
	if(list_ptr->head==ptr){
		list_ptr->head=ptr->right;
	}
	if(list_ptr->head==NULL || ptr==NULL)
		return;
	if(ptr->left!=NULL)
		ptr->left->right=ptr->right;
	if(ptr->right!=NULL)
		ptr->right->left=ptr->left;
	
	//free(ptr);
	return;
}

/*void print_list(list *list_ptr)
{
	node* temp=NULL;
	student *s=NULL;
	for(temp=list_ptr->head;temp!=NULL;temp=temp->right)
	{
	    s=(student*)((char*)temp-list_ptr->offset);
	    printf("Student name = %s\n", s->name);
	    printf("Roll No. = %u\n", s->roll_no);
	    printf("Course = %s\n", s->course);
	    printf("Current Year = %u\n", s->year);
	}
}*/
