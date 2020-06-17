#ifndef SET_H
#define SET_H

#include <stdbool.h>

#define INITIAL_SET_SIZE 4

typedef struct set_t {
    int *values;
    int size;
    int count;
} Set;

void initialize(Set *set);
int add(Set *set, int value);
int remove(Set *set, int value);
bool contains(Set *set, int value);
void destroy(Set *set);

#endif