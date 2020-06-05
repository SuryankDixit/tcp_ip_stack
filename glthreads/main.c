#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include"glthread_declarations.h"

typedef struct student_details {
	char name[30] ;
	unsigned int roll_no;
	char course[30];
	unsigned int year;
	node link;
} student;


void print_details(student *s)
{
	printf("Student name = %s\n", s->name);
	printf("Roll No. = %u\n", s->roll_no);
 	printf("Course = %s\n", s->course);
 	printf("Graduating Year = %u\n", s->year);
}

int main(int argc, char **argv)
{
	student *s1;   		// student s1;
	s1 = calloc(1,sizeof(student));
	strncpy(s1->name,"Nakul",strlen("Nakul"));
	s1->roll_no=32;
	strncpy(s1->course,"B.Tech CSE-OSOS",strlen("B.Tech CSE-OSOS"));
	s1->year=2022;
	INITIALIZE_LINK((&s1->link));
	
	student *s2;   		
	s2 = calloc(1,sizeof(student));
	strncpy(s2->name,"Ankit",strlen("Ankit"));
	s2->roll_no=21;
	strncpy(s2->course,"B.Tech CSE-CYBER",strlen("B.Tech CSE-CYBER"));
	s2->year=2022;
	INITIALIZE_LINK((&s2->link));
	
	student *s3;   		
	s3 = calloc(1,sizeof(student));
	strncpy(s3->name,"Prateek",strlen("Prateek"));
	s3->roll_no=203;
	strncpy(s3->course,"B.Tech CSE-CCVT",strlen("B.Tech CSE-CCVT"));
	s3->year=2022;
	INITIALIZE_LINK((&s3->link));
	
	
	
	// Creating List;
	
	list *list_ptr = calloc( 1,sizeof(list) );
	initialize_list( list_ptr,offsetof( student,link ) );
	
	
	
	// Insert objects(Application data) into list;
	
	list_insert( list_ptr, &(s1->link));
	list_insert( list_ptr, &(s2->link));
	list_insert( list_ptr, &(s3->link));
	
	student *s=NULL;
	//ITERATE_LIST(list_ptr,student,s)
	ITERATE_GL_THREADS_BEGIN(list_ptr, student, s){

        print_details(s);
    } ITERATE_GL_THREADS_ENDS;

	
	list_delete(list_ptr,&(s3->link));
	//ITERATE_LIST(list_ptr,student,s);
	ITERATE_GL_THREADS_BEGIN(list_ptr, student, s){

        print_details(s);
    } ITERATE_GL_THREADS_ENDS;

	
	
	return 0;
}
