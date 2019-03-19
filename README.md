# Better Malloc

### The basic idea of this project was to understand how a simple memory
### allocator works using basic system calls.
 <br> The implementation must pass through some basic guidelines:
1) When requesting memory from the OS, we must use mmap (rather than sbrk).
2) Although a real memory allocator requests more memory from the OS whenever it
can't satisfy a request from the user, our memory allocator must call mmap only one
time (when it is first initialized).
3) The choice for Data Structures to maintain the free list and policy for choosing the
chunk of memory was upto us.
4) The memory allocator should be more flexible in how the user can specify what
memory should be freed.

# Program Descriptions:-

1. mem.cpp contains all the function definitions.
2. driver.cpp is just for the debugging purpose and contains main function.
3. mem.h contains all the function declarations.
4. To run the program execute run.sh
