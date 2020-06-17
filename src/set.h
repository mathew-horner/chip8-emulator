#ifndef SET_H
#define SET_H

#include <stdbool.h>

typedef struct set_t {

} Set;

int add(Set *set, int value);
int remove(Set *set, int value);
bool contains(Set *set, int value);

#endif