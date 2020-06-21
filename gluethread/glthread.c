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

void delete_node(glthread_t *curr_glthread)
{
	if(!curr_glthread->left){
        if(curr_glthread->right){
            curr_glthread->right->left = NULL;
            curr_glthread->right = 0;
            return;
        }
        return;
    }
    if(!curr_glthread->right){
        curr_glthread->left->right = NULL;
        curr_glthread->left = NULL;
        return;
    }

    curr_glthread->left->right = curr_glthread->right;
    curr_glthread->right->left = curr_glthread->left;
    curr_glthread->left = 0;
    curr_glthread->right = 0;
}
