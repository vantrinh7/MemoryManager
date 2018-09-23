/**
 * This program creates a memory allocator with Mem_Init, Mem_Alloc, Mem_Free and Mem_Dump
 * @author Emma - Van
 * @version Apr 10, 2018
 **/

#include "mem.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAGIC 123456789

// Node structure includes size, magic number, next and previous pointers
typedef struct node_t {
  int size;
  int magic;
  struct node_t *next;
  struct node_t *prev;
} node_t;

// Pointer to the head of the free list
node_t *head;

/**
 * This method initializes memory allocator by getting a large block of memory using mmap
 * @param int sizeOfRegion the requested size of region
 * @return pointer to the beginning of this region (head pointer)
 **/
void *Mem_Init (int sizeOfRegion) {
  int requestSize;

  // Calculate the requestSize to be multiple of a page (this is required by the mmap function).
  // If size requested is not divisible by page size, round up
  if ((sizeOfRegion % getpagesize()) != 0) {
    int multiple = (int)(sizeOfRegion/getpagesize()) + 1;
    requestSize = multiple * getpagesize();
  } else {
    requestSize = sizeOfRegion;
  }

  // open the /dev/zero device
  int fd = open("/dev/zero", O_RDWR);
  
  // requestSize (in bytes) needs to be evenly divisible by the page size
  head =  mmap(NULL, requestSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (head == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  // close the device 
  close(fd);

  // Create a header for head and fill in the fields of header
  head->size = requestSize - sizeof(node_t);
  head->magic = 0;
  head->next = 0;
  head->prev = 0;
  
  return head;
}

/**
 * This method searches for a free and big enough memory block 
 * within a free list (that a functions as a doubly linked list)
 * and allocate the block.
 *
 * @param int size requested size
 * @return pointer to the allocated block
 **/
void *Mem_Alloc (int size) {
  
  void *ptr = NULL; // Pointer to allocated block
  node_t *headPrev; // Pointer to previous of head
  
  while (head != NULL) {
    // When a block is free
    if ((head->magic == 0)) {
      
      // When a block is larger than or equal to size, allocate it
      if (head->size >= size){
 
	// Save the old size
	int oldSize = head->size;

	// Save the old next
	node_t *oldNext = head->next;

	// Save the old previous
	headPrev = head->prev;
  
	// Update the header
	head->size = size;
	head->magic = MAGIC;
	head->next = 0;
	head->prev = 0;

	// Increment pointer to point to after the header
	ptr = head + 1;
	
	// If block is larger than size plus header, split into a new head
	if (oldSize > size + sizeof(node_t)){

	  // New head points to after the allocated chunk
	  char *p = (char *)head;
	  p = p + sizeof(node_t) + size;
	  head = (node_t *)p;

	  // Create a new header with original values
	  head->size = oldSize - sizeof(node_t) - size;
	  head->magic = 0;
	  head->next = oldNext;

	  // Set relationship between new head and old previous value
	  head->prev = headPrev;
	  if (headPrev != NULL) {
	    headPrev->next = head;
	  }
	}
	// If block is smaller than size plus header, make the next free chunk the head
	else {	  
	  head = oldNext;

	  // Set relationship between new head and old previous value
	  head->prev = headPrev;
	  if (headPrev != NULL) {
	    headPrev->next = head;
	  }
	}
	break;
      }
    }
    headPrev = head; // Save current block before moving on to the next
    head = head->next; // Move to the next block in the free list 
  }

  // After searching through the whole list
  // If there is more than one free block in the list
  if (headPrev != NULL) {
    head = headPrev;

    // Iterate all the way back to beginning of free list to restore head
    while (head->prev != NULL) {
      head = head->prev;
    }
  }
  return ptr;
}

/**
 * This method frees a memory block and add it to the beginning of the free list
 * 
 * @param void *ptr pointer to the block that needs to be freed
 * @return 0 if successful or pointer is null, -1 if unsuccessful 
 * (unsuccessful when tryin to free a block that wasn't allocated)
 **/
int Mem_Free (void *ptr) {
  if (ptr == NULL) {
    return 0;
  }
  else {
    // Subtract the width of the header to get new head   
    node_t *newHead = ptr - sizeof(node_t);

    // Check the magic number to determine if this region was allocated before
    if (newHead->magic == MAGIC) {
      
      // Update magic number to be 0 to signify the region is free
      newHead->magic = 0;

      // Save old head value into next of new head
      newHead->next = head;

      // Save new head into previous of old head
      head->prev = newHead;

      // Now save newHead into head variable
      head = newHead;

      // Make the pointer null
      ptr = NULL;
      
      return 0;
    }
    return -1;
  }  
}

/** 
 * Mem_Dump is a function that printing out the header for each block
 * of allocated memory in the free list - used for debugging.
 **/
void Mem_Dump() {

  printf("\nFree memory: \n");
  
  // Temporary variable to point to the block in each iteration
  struct node_t* temp = head;
  
  // Loop through all blocks in free list and print out address and size
  // Loop until the next address is 0. this is stored in temp + 4.
  while (temp != NULL) {
    printf("address: %p\n\n", temp);
    printf("size: %d\n\n", temp->size);
    printf("\n");
    temp = (temp->next);
  }
}
