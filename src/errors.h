#ifndef ERRORS_H
#define ERRORS_H

typedef enum chip8_error {
    FILE_IO_ERROR = 1,
    ROM_TOO_LARGE
} Chip8Error;

const char *chip8_error_default_message(Chip8Error error);

#endif
