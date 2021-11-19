#include <stdio.h>
#include <stdlib.h>

/*
Write a program that creates an array of integers called data of size
100 using malloc; then, set data[100] to zero. What happens
when you run this program? What happens when you run this
program using valgrind? Is the program correct?

The program runs and "works" with no reported errors. But there is un-freed memory.
*/

int main() 
{
    int *data = (int *) malloc(100 * sizeof(int));
    printf("address given: %p\n", &data);
    data[100] = 0;
}

