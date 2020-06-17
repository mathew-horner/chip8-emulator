#include "set.h"
#include <stdlib.h>

void initialize(Set *set)
{
    set->values = (int *)malloc(INITIAL_SET_SIZE * sizeof(int));
    set->size = INITIAL_SET_SIZE;
}

int add(Set *set, int value)
{
    if (set->count == set->size)
        double_size(set);
    set->values[set->count] = value;
    set->count++;
}

int remove(Set *set, int value)
{
    set->values[set->count - 1] = 0;
    set->count--;
}

bool contains(Set *set, int value)
{
    for (int i = 0; i < set->count; i++) {
        if (set->values[i] == value)
            return true;
    }

    return false;
}

void destroy(Set *set)
{
    free(set->values);
    set->size = 0;
    set->count = 0;
}

void double_size(Set *set)
{
    int *new = (int *)malloc(set->size * 2 * sizeof(int));
    for (int i = 0; i < set->count; i++)
        new[i] = set->values[i];

    set->size *= 2;
    free(set->values);
    set->values = new;
}
