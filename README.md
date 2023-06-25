# Memory-management - Custom malloc() and free() functions in C

In the C standard library, the malloc and free functions are available for allocating and freeing memory. IN
in this assignment, the task is to implement a custom version of memory allocation and deallocation/free.

The **memory_alloc** function is intended to provide services similar to the standard malloc. Therefore, the input parameters are the size of the requested continuous block of memory, and the function returns to it: a pointer to the successfully allocated chunk of free memory that has been reserved, or NULL when the contiguous memory of the requested size cannot be reserved.

The **memory_free** function is used to free a reserved block of memory, similar to the free function. Function function returns 0 if the memory block was freed (the function ran successfully), otherwise it returns 1. You can assume that
parameter will always be a valid pointer, returned from previous calls to the memory_alloc function, which has not yet been freed.

The **memory_check** function is used to check if the parameter (pointer) is a valid pointer that was in any of the previous calls returned by the memory_alloc function and has not yet been released by the
memory_free. The function returns 0 if the pointer is invalid, otherwise it returns 1.

The **memory_init** function is used to initialize managed free memory. Assume that the function is called just once before all other calls to memory_alloc, memory_free, and memory_check. See testing below. As an input parameter to the function comes a block of memory that you can use for organizing and also allocate free memory. Your functions cannot use global variables except for one global variable to remember the memory pointer that enters the memory_init function. Pointers, allocated by your memory_alloc function must come exclusively from the block of memory that was allocated to the function
memory_init

The **memory_alloc** function is intended to provide services similar to the standard malloc. Therefore, the input parameters are the size of the requested continuous block of memory, and the function returns to it: a pointer to the successfully allocated chunk of free memory that has been reserved, or NULL when the contiguous memory of the requested size cannot be reserved.

The **memory_free** function is used to free a reserved block of memory, similar to the free function. Function function returns 0 if the memory block was freed (the function ran successfully), otherwise it returns 1. You can assume that
parameter will always be a valid pointer, returned from previous calls to the memory_alloc function, which has not yet been freed.

The **memory_check** function is used to check if the parameter (pointer) is a valid pointer that was in any of the previous calls returned by the memory_alloc function and has not yet been released by the
memory_free. The function returns 0 if the pointer is invalid, otherwise it returns 1.

The **memory_init** function is used to initialize managed free memory. Assume that the function is called just once before all other calls to memory_alloc, memory_free, and memory_check. See testing below. As an input parameter to the function comes a block of memory that you can use for organizing and also allocate free memory. Your functions cannot use global variables except for one global variable to remember the memory pointer that enters the memory_init function. Pointers, allocated by your memory_alloc function must come exclusively from the block of memory that was allocated to the function
memory_init

I search for a free block using the **best-fit** method. The temp indicator is used to traversal of the entire memory. I compare the sizes of the free blocks with the variable **bestsize** in which I store the size of the smallest block. When a block is found with a smaller size than the previous size, I redirect the **best** pointer to to the header of the new block. If a block is found that is exactly the same size as the requested, the loop stops and returns a pointer to the header of that block. Since the entire block needs to be checked, the best-fit algorithm has a constant time complexity **O(n)**. After finding a sufficiently large block, I check whether the block can be split into 2 parts.

**Function int memory_free(void* valid_ptr)**

The function receives a single pointer as an argument that points to the 1st byte of the payload, which I redirect and the 1st byte of the header and typecast to the header type. Then I check the right neighbor of the block. If it is free and not NULL, the block I merge the block with its right neighbor. Thus, the new block size will be its original size plus the header size and the payload size of the right neighbor. Then I check the neighbor on the left side, if it is not NULL and is free, I redirect the pointers so that all 3 blocks are concatenated and their header is the header of the left neighbor.
**Time complexity: O(1)**

**Function int memory_check(void* ptr)**
This function is used to check the block headers. I will go through the entire block when the finds the block header that the function received as an argument, it returns 1, otherwise 0.
**Time complexity: min = O(1) max = O(n)**
