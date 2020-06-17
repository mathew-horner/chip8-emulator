#include "set.h"
#include <stdlib.h>

void double_size(Set *set)
{
    int *new = (int *)malloc(set->size * 2 * sizeof(int));
    for (int i = 0; i < set->count; i++)
        new[i] = set->values[i];

    set->size *= 2;
    free(set->values);
    set->values = new;
}

void set_initialize(Set *set)
{
    set->values = (int *)malloc(INITIAL_SET_SIZE * sizeof(int));
    set->size = INITIAL_SET_SIZE;
    for (int i = 0; i < set->size; i++)
        set->values[i] = -1;
}

void set_add(Set *set, int value)
{
    // todo: duplicate check
    if (set->count == set->size)
        double_size(set);
    set->values[set->count] = value;
    set->count++;
}

int set_remove(Set *set, int value)
{
    for (int i = 0; i < set->size; i++) {
        if (set->values[i] == value) {
            set->values[i] = -1;
            set->count--;
            return 0;
        }
    }

    return 1;
}

bool set_contains(Set *set, int value)
{
    for (int i = 0; i < set->size; i++) {
        if (set->values[i] == value)
            return true;
    }

    return false;
}

void set_destroy(Set *set)
{
    free(set->values);
    set->size = 0;
    set->count = 0;
}

