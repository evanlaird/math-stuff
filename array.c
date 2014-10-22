#include "array.h"

int *array_int(unsigned int length, bool zeroed)
{
    int *arr = malloc(sizeof(*arr) * length);
    return arr;
}
