#ifndef SET_H
#define SET_H

#include <stdbool.h>

#define INITIAL_SET_SIZE 4

typedef struct set_t {
    int *values;
    int size;
    int count;
} Set;

void set_initialize(Set *set);
void set_add(Set *set, int value);
int set_remove(Set *set, int value);
bool set_contains(Set *set, int value);
void set_destroy(Set *set);

#endif