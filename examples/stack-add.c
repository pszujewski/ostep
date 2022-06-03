#include <stdio.h>

int main(int argc, char *argv[])
{
    int stack = 0;
    printf("%p\n", &stack); // Get the virtual address of the stack for this process.
    return 0;
}
