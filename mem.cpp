#include <bits/stdc++.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include "mem.h"

//error showing
#define handle_error(msg) \
       	do { perror(msg);  } while (0)

using namespace std;	

void *ptr=NULL;


//structure for free and allocated nodes
struct node						
{
	void *dataptr;  
	int size;       
};


//freelist nodes start and end ptrs
node *freelist_start = NULL;		
node *freelist_end=NULL;

//allocatedlist nodes start and end ptrs
node *allocatedlist_start = NULL;		
node *allocatedlist_end=NULL;				// point to last header in alloc list


int freenode_count=1;						//number of free nodes in freelist
int allocatednode_count=0;					//number of nodes in allocatedlist
int current_space;						    //current space 
int sizegarbage=0;                          //for collecting garbage


//function to request memory using mmap
int Mem_Init(int sizeOfRegion)			
{	
    if(sizeOfRegion<=0)
    {	
    	handle_error("Invalid size...!");
    	return -1;
    	
    }
     sizegarbage=sizeOfRegion;
    
    int fd = open("/dev/zero", O_RDWR);   // size (in bytes) needs to be evenly divisible by the page size
    ptr = mmap(NULL, 7*sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0); 		// getting memory from OS
    if (ptr == MAP_FAILED) 
    { 
       handle_error("MMAP Error...!");
       return -1;
    } 
    current_space=sizeOfRegion;
    freelist_start=(node*)(ptr + 7*sizeOfRegion - sizeof(node));
    allocatedlist_start=(node*)(ptr + sizeOfRegion);
    allocatedlist_start->size=0;
    freelist_start->dataptr=ptr;
    freelist_start->size=sizeOfRegion;
    
    freelist_end=freelist_start;
    allocatedlist_end=allocatedlist_start;
    close(fd); 
    return 1;    
}
 


int Mem_GetSize(void *ptr1)
{
    node *currentnode=allocatedlist_start;
	unsigned long long int low,high;
	
	//traverse the allocated list to find the particular allocated node
	for(int i=0;i<allocatednode_count;i++)				
	{
		
		low=(uintptr_t)currentnode->dataptr;
		high=low + currentnode->size;

		if((uintptr_t)low<=(uintptr_t)ptr1 && (uintptr_t)ptr1 < (uintptr_t)high)
		{
			return currentnode->size;		
		}
		currentnode++;
	}
	return -1;

}


//traverse the allocated list to find the particular allocated node
int Mem_IsValid(void *user_ptr)
{
	node *currentnode=allocatedlist_start;
	unsigned long long int low,high;			
	for(int i=0;i<allocatednode_count;i++)
	{
		low=(uintptr_t)currentnode->dataptr;
		high=low + currentnode->size;
		if((uintptr_t)low<=(uintptr_t)user_ptr && (uintptr_t)user_ptr<(uintptr_t)high)
			return 1;
		currentnode++;
	}
	return 0;
} 

void *Mem_Alloc(int size)
{
	// if free space is less than required
	if(size>current_space)			
	{
		handle_error("Not enough space available...!\n");
		return NULL;
	}	
	
	//find first large enough node to satisfy user request
	
	node *firstlarge_node=freelist_start;
	node *currentnode=firstlarge_node;

	// pointer to be returned to user
	void *result_ptr=firstlarge_node->dataptr;			
	int largenode_size=firstlarge_node->size;
	
	// first fit to search free node
	for(int i=1;i<=freenode_count;i++)				
	{	
		
		if(size < currentnode->size && firstlarge_node->dataptr > currentnode->dataptr)
			firstlarge_node=currentnode;
		
		currentnode--;
	}
	
	result_ptr=firstlarge_node->dataptr;
	node *newallocated_node;
	
	// if space is partially allocated from a free node
	if(size<firstlarge_node->size)				
	{	
		
		firstlarge_node->size-=size;			
		firstlarge_node->dataptr+=size;			
		
		if(allocatedlist_start->size != 0)			
			newallocated_node=allocatedlist_end +1;
		
		else
			newallocated_node=allocatedlist_start;
		
		newallocated_node->size=size;
		newallocated_node->dataptr=result_ptr;

		// increase num of allocated nodes
		allocatednode_count++;						
		allocatedlist_end=newallocated_node;

		// update current space
		current_space-=size;					
	}
	// if space is completely allocated of a free node
	else if(size==firstlarge_node->size)		
	{
		
		if(allocatedlist_start->size != 0)			
			newallocated_node=allocatedlist_end + 1;
		else
			newallocated_node=allocatedlist_start;

		newallocated_node->size=size;
		newallocated_node->dataptr=result_ptr;
		allocatednode_count++;
		allocatedlist_end=newallocated_node;
		current_space-=size;
		
		if(firstlarge_node!=freelist_end)
		{			
			firstlarge_node->dataptr=freelist_end->dataptr;
			firstlarge_node->size=freelist_end->size;
		}
		
		// increase num of free nodes
		freelist_end++;						
		if(freenode_count>1)
			freenode_count--;
		else if(freenode_count==1)
		{
			freelist_start->dataptr=NULL;
			freelist_start->size=0;
		}	

		
	}

	// if biggest node is less than size required	
	else							
	{
		handle_error("Not enough space available...!\n");
		return NULL;
	}	
	return result_ptr;
}



int Mem_Free(void *ptr1)
{
	if(ptr1==NULL)
		return -1;

	//checking whether ptr provided by user is within the valid range
	if(!Mem_IsValid(ptr1))				
	{	
		handle_error("Invalid Pointer...!\n");
		
		return -1;
	}	
	node *currentnode=allocatedlist_start;
	unsigned long long int low,high;
	
	//traversing the allocated list to find the particular allocated node
	for(int i=0;i<allocatednode_count;i++)		
	{
		
		low=(uintptr_t)currentnode->dataptr;
		high=low + currentnode->size;

		if((uintptr_t)low<=(uintptr_t)ptr1 && (uintptr_t)ptr1<(uintptr_t)high)
			break;
		currentnode++;	
	}
	int actual_size=currentnode->size;
	currentnode->size=(uintptr_t)ptr1 - (uintptr_t)low;
	
	node *newfree_node;
	//partially freeing a node
	if(currentnode->size!=1)						
	{	
		newfree_node=freelist_end;											
		newfree_node--;
		newfree_node->size = actual_size - currentnode->size;
	}

	//fully freeing a node
	else								
	{
		newfree_node=freelist_end - 1; 
		newfree_node->size = actual_size;
		currentnode->size=allocatedlist_end->size;
		currentnode->dataptr=allocatedlist_end->dataptr;
		allocatedlist_end--;
		allocatednode_count--;
	}	
	newfree_node->dataptr=ptr1;
	freenode_count++;
	freelist_end=newfree_node;
	current_space+=newfree_node->size;

	//merging the adjacent free nodes

	node *ptr=(node*)newfree_node;
	int low22=(uintptr_t)ptr->dataptr;
	int high22=low22 + ptr->size;
	currentnode=freelist_start;
	int low1;
	for (int i = 0; i < freenode_count; ++i)			
	{
		low1=(uintptr_t)currentnode->dataptr;
		if(high22==low1)						
		{
			ptr->size+=currentnode->size;		
			currentnode->size=freelist_end->size;	
			currentnode->dataptr=freelist_end->dataptr;
			freelist_end++;
			freenode_count--;						
			break;
		}	
		currentnode--;
	}
	
	ptr=(node*)newfree_node;
	low22=(uintptr_t)ptr->dataptr;
	high22=low22 + ptr->size;
	currentnode=freelist_start;
	int high1;
	for (int i = 0; i < freenode_count; ++i)			
	{
		high1=(uintptr_t)currentnode->dataptr + currentnode->size;
		if(high1 == low22)						
		{
			currentnode->size+=ptr->size;		
			freelist_end++;
			freenode_count--;						
			break;
		}
		currentnode--;
	}
	return 1;
}

void garbage_collector()
{
	sbrk(-sizegarbage);
	cout<<"Memory allocated is cleared...!"<<endl;
}




