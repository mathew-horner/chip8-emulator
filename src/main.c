#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "io.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: chip8-emulator <file>\n");
        exit(1);
    }

    char *filepath = argv[1];
    load_rom(filepath);
    start_cpu();
    return 0;
}
