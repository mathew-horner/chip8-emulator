#ifndef IO_H
#define IO_H

typedef enum chip8_error Chip8Error;

void load_rom(uint8_t *memory, char *filepath);
Chip8Error try_load_rom_file(uint8_t *memory, FILE *rom);

#endif
