#include <stdio.h>
#include <stdlib.h>

/*
Create a program that allocates an array of integers (as above), frees
them, and then tries to print the value of one of the elements of
the array. Does the program run? What happens when you use
valgrind on it?

It does print the value as expected even though the pointer was "free-ed" already: 

address given: 0x7ffe3541e3f8
data at index 50: 51

valgrind reports no possible memory leaks. This means that `free` tells the OS, 
the memory starting at that pointer is no longer needed, so it can be replaced.
But that doesn't mean it is overwritten right away.
*/

int main() 
{
    int *data = (int *) malloc(100 * sizeof(int));
    printf("address given: %p\n", &data);
    for (size_t i = 0; i < 100; i++)
    {
        data[i] = i + 1;
    }
    free(data);
    printf("data at index 50: %d\n", data[50]);
}

