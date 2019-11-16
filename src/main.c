#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MEMORY_SIZE 4096
#define PROGRAM_MEMORY_OFFSET 512

uint8_t memory[MEMORY_SIZE];

typedef enum chip8_error {
    FILE_IO_ERROR = 1,
    ROM_TOO_LARGE
} Chip8Error;

// Provides a default error message for any Chip8Error type.
const char *chip8_error_default_message(Chip8Error error)
{
    switch (error) {
        case FILE_IO_ERROR:
            return "File IO error!";
        case ROM_TOO_LARGE:
            return "ROM file contents cannot fit in memory!";
        default:
            return "";
    }
}

// Attempts to load the contents of a given file pointer into main memory.
Chip8Error try_load_rom_file(FILE *rom)
{
    // Determine the size of ROM for copying purposes and to determine if it
    // is too large to fit in main memory.
    if (fseek(rom, 0, SEEK_END) != 0)
        return FILE_IO_ERROR;

    long size = ftell(rom);
    if (size == -1L)
        return FILE_IO_ERROR;
    if (size > MEMORY_SIZE - PROGRAM_MEMORY_OFFSET)
        return ROM_TOO_LARGE;
    
    // Read the contents of the ROM file into a temporary buffer so it can be
    // copied to main memory.
    uint8_t *rom_buffer = (uint8_t*)malloc(size * sizeof(uint8_t));
    fread(rom_buffer, sizeof(uint8_t), size, rom);

    for (int i = 0; i < size; i++) {
        memory[i + PROGRAM_MEMORY_OFFSET] = rom_buffer[i];
    }

    return 0;
}

// A wrapper function for try_load_rom_file that will exit the program if an
// error occurs loading the ROM.
void load_rom(char *filepath)
{
    FILE *rom = fopen(filepath, "r");
    if (!rom) {
        fclose(rom);
        printf("%s not found!\n", filepath);
        exit(1);
    }
    Chip8Error error = try_load_rom_file(rom);
    fclose(rom);
    if (error != 0) {
        const char *message = chip8_error_default_message(error);
        printf("Error loading rom: %s\n", message);
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: chip8-emulator <file>\n");
        exit(1);
    }

    char *filepath = argv[1];
    load_rom(filepath);
    return 0;
}
