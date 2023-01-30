#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mapreduce.h"

int main(int argc, char *argv[])
{
    assert(argc == 2);
    long result = MR_DefaultHashPartition(argv[1], 10);
    printf("Result: %ld\n", result);
}