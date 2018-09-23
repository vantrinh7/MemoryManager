# Memory Manager

This is a program that manages the virtual address space of the heap memory in the operating system. This memory manager has three tasks: ask the operating system to expand the heap portion of a process' address space, allocate the given memory space to the calling process, and add the memory space back to a list of free memory chunks when the process is finished. 

(pic)

Each memory block has a header, followed by a space that is either allocated or free. The header includes two pieces of information: the size of the free chunk and a field of number. In an allocated chunk, this field contains a "magic number" - an arbitrarily designated number used to identify an allocated space. In a free chunk, this field contains the address of the next item in the list of free memory blocks. 

An example of this process is, inititally, the user requests 4KB (or 4096 bytes) from the operating system for the heap. Since each field in the header is an integer, its size is 4 bytes. Thus, the total size of the header is 8 bytes. The free space is 4096 - 8 = 4088 bytes, which is saved into the "size" field of the header. `head` is the pointer to the beginning of the free list, and it points to the header of this chunk. 

(pic)

Suppose 100 bytes is allocated and identified by the pointer `ptr`. The header then shows the size of 100 and the arbitrary number of 1234567. `head` pointer points to after the allocated chunk, with a new header that shows the size (4088 - 100 - 8 = 3980) and the address of the next free chunk (since it is the only free chunk, the address is 0).

(pic)

Suppose two more 100-byte chunks are allocated (left figure below). The size of the free chunk is now 3764 (= 3980 - 100 - 8 - 100 - 8). If the second chunk is freed, its "next" field will contain the address of the pointer to next free chunk (right figure below).

(pic) (pic)
As a result, we have a two allocated chunks identified by magic number, and list of 2 free chunks of size 100 bytes and 3764 bytes.

On a side note, the memory manager operates entirely within the virtual address space of a single process. It does not know about the physical pages allocated to this process, the mapping from virtual addresses to physical addresses, or about any other processes that are running.

## Requesting memory space

## Allocating memory

## Freeing Memory

## Debugging

## File description


(Image source: [Operating System - Three Easy Pieces](http://pages.cs.wisc.edu/~remzi/OSTEP/), Chapter 17).
