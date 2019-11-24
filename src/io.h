#ifndef IO_H
#define IO_H

typedef enum chip8_error Chip8Error;
typedef struct memory_t Memory;

void load_rom(Memory *memory, char *filepath);
Chip8Error try_load_rom_file(Memory *memory, FILE *rom);

#endif
