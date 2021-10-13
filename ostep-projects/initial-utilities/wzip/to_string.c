#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

// https://stackoverflow.com/questions/31403210/undefined-reference-to-log10-function

int main()
{
    int num = 42;
    int enough = (int)((ceil(log10(num)) + 1) * sizeof(char));
    char str[enough];
    sprintf(str, "%d", 42);

    return EXIT_SUCCESS;
}