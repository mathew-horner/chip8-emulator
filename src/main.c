#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cpu.h"
#include "io.h"
#include "graphics.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: chip8-emulator <file>\n");
        exit(1);
    }

    char *filepath = argv[1];
    load_rom(filepath);

    initialize_cpu();
    initialize_renderer();

    while (1) {
        execute_next_instruction();
        render_frame();
    }

    destroy_renderer();
    return 0;
}
