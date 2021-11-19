#include <stdio.h>
#include <stdlib.h>

/*
Write a simple program that allocates memory using malloc() but
forgets to free it before exiting. What happens when this program
runs? Can you use gdb to find any problems with it? How about
valgrind (again with the --leak-check=yes flag)?
*/

int main() 
{
    int *x = (int *) malloc(10 * sizeof(int));
    printf("address given: %p\n", &x);
    x[0] = 100;
    x[1] = 200;
}

/*

Valgrind output:

➜  vm-api git:(master) ✗ valgrind --leak-check=yes ./a.out
==5726== Memcheck, a memory error detector
==5726== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==5726== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
==5726== Command: ./a.out
==5726== 
address given: 0x1ffefff830
==5726== 
==5726== HEAP SUMMARY:
==5726==     in use at exit: 40 bytes in 1 blocks
==5726==   total heap usage: 2 allocs, 1 frees, 1,064 bytes allocated
==5726== 
==5726== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==5726==    at 0x4C31B0F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==5726==    by 0x10871A: main (q4.c:13)
==5726== 
==5726== LEAK SUMMARY:
==5726==    definitely lost: 40 bytes in 1 blocks
==5726==    indirectly lost: 0 bytes in 0 blocks
==5726==      possibly lost: 0 bytes in 0 blocks
==5726==    still reachable: 0 bytes in 0 blocks
==5726==         suppressed: 0 bytes in 0 blocks
==5726== 
==5726== For counts of detected and suppressed errors, rerun with: -v
==5726== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
*/

