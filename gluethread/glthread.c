#include "glthread.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

void initialize_glthread(glthread_t *glthread)
{
	glthread->left=NULL;
	glthread->right=NULL;
}

void add_node_front(glthread_t *head, glthread_t *node)
{
	if(head->right==NULL)
	{
		head->right=node;
		node->left=head;
		return;
	}
	
	glthread_t *temp=NULL;
	temp=head->right;
	head->right=node;
	node->left=head;
	node->right=temp;
	temp->left=node;
	return;
}
