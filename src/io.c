#include "io.h"

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
    if (size > MEMORY_SIZE - PROGRAM_OFFSET)
        return ROM_TOO_LARGE;
    
    rewind(rom);

    // Read the contents of the ROM file into a temporary buffer so it can be
    // copied to main memory.
    uint8_t *rom_buffer = (uint8_t*)malloc(size * sizeof(uint8_t));
    int read = fread(rom_buffer, sizeof(uint8_t), size, rom);

    for (int i = 0; i < size; i++)
        memory[PROGRAM_OFFSET + i] = rom_buffer[i];

    free(rom_buffer);
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
