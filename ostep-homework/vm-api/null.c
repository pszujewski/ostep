#include <stdio.h>
/*
    First, write a simple program called null.c that creates a pointer
    to an integer, sets it to NULL, and then tries to dereference it. Compile this into an executable called null. What happens when you
    run this program?

    When I run this program, surprisingly I get an address like this:

    from stdout... 
    
    address: 0x7ffd9f1ce540
*/

int main() 
{
    int *x = NULL;
    printf("address: %p\n", &x);
}

