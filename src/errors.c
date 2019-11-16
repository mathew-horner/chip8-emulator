#include "errors.h"

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
