#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "memory.h"

void load_rom(char *filepath);
Chip8Error try_load_rom_file(FILE *rom);

#endif
