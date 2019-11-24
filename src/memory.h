#ifndef MEMORY_H

#define MEMORY_H
#define MEMORY_SIZE 4096
#define PROGRAM_OFFSET 512

#include <stdint.h>

typedef struct memory_t {
    uint8_t values[MEMORY_SIZE];
} Memory;

#endif
