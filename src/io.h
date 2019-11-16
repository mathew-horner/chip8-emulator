#ifndef IO_H
#define IO_H

typedef enum chip8_error Chip8Error;

void load_rom(char *filepath);
Chip8Error try_load_rom_file(FILE *rom);

#endif
