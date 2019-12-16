#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "debugger.h"
#include "emulator.h"
#include "io.h"

#define FRAME_RATE 60
#define FRAME_INTERVAL_NS 1000000000 / FRAME_RATE
#define INSTRUCTIONS_PER_FRAME 9

void repl_loop(Emulator *emulator)
{
    char input[5];
    printf("You are running the Chip-8 Emulator in REPL mode.\n");
    while (1) {
        printf(">>> ");
        scanf("%s", input);
        uint16_t instruction = (uint16_t)strtol(input, NULL, 16);
        execute_instruction(emulator, instruction);
        render_frame(&(emulator->display));
    }
}

void debugger_loop(Emulator *emulator)
{
    Debugger debugger;
    debugger.emulator = emulator;
    debugger.break_address_count = 0;

    bool pass = false;
    char *input;
    size_t n;

    printf("You are running the Chip-8 Emulator in Debug mode.\n");

    while (1) {
        if (!pass || should_break(&debugger)) {
            pass = false; 
            printf(">>> ");
            int size = (int)getline(&input, &n, stdin);
            input[size - 1] = '\0';
            
            DebuggerCommand command;
            int error = parse_debugger_command(input, &command);

            if (error != 0) {
                // TODO: Print out specific error here.
                printf("Error parsing command!\n");
                continue;
            }

            if (command.type == EXIT) break;
            if (command.type == CONTINUE) {
                pass = true;
                execute_next_instruction(emulator);
                continue;
            }

            execute_debugger_command(&debugger, &command);
            destroy_debugger_command(&command);
        } else {
            execute_next_instruction(emulator);
        }

        decrement_dt(&(emulator->cpu));
        render_frame(&(emulator->display));
    }
}

// Sleeps long enough to fill up the rest of the frame interval. (As to maintain a constant frame rate).
void prerender_sleep(double elapsed)
{
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = FRAME_INTERVAL_NS - elapsed;
    nanosleep(&sleep_time, NULL);
}

void execution_loop(Emulator *emulator)
{
    clock_t frame_start;
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        frame_start = clock() / CLOCKS_PER_SEC;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_1:
                        emulator->key_state[1] = true;
                        break;
                    case SDLK_2:
                        emulator->key_state[2] = true;
                        break;
                    case SDLK_3:
                        emulator->key_state[3] = true;
                        break;
                    case SDLK_4:
                        emulator->key_state[12] = true;
                        break;
                    case SDLK_q:
                        emulator->key_state[4] = true;
                        break;
                    case SDLK_w:
                        emulator->key_state[5] = true;
                        break;
                    case SDLK_e:
                        emulator->key_state[6] = true;
                        break;
                    case SDLK_r:
                        emulator->key_state[13] = true;
                        break;
                    case SDLK_a:
                        emulator->key_state[7] = true;
                        break;
                    case SDLK_s:
                        emulator->key_state[8] = true;
                        break;
                    case SDLK_d:
                        emulator->key_state[9] = true;
                        break;
                    case SDLK_f:
                        emulator->key_state[14] = true;
                        break;
                    case SDLK_z:
                        emulator->key_state[10] = true;
                        break;
                    case SDLK_x:
                        emulator->key_state[0] = true;
                        break;
                    case SDLK_c:
                        emulator->key_state[11] = true;
                        break;
                    case SDLK_v:
                        emulator->key_state[15] = true;
                        break;
                }
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_1:
                        emulator->key_state[1] = false;
                        break;
                    case SDLK_2:
                        emulator->key_state[2] = false;
                        break;
                    case SDLK_3:
                        emulator->key_state[3] = false;
                        break;
                    case SDLK_4:
                        emulator->key_state[12] = false;
                        break;
                    case SDLK_q:
                        emulator->key_state[4] = false;
                        break;
                    case SDLK_w:
                        emulator->key_state[5] = false;
                        break;
                    case SDLK_e:
                        emulator->key_state[6] = false;
                        break;
                    case SDLK_r:
                        emulator->key_state[13] = false;
                        break;
                    case SDLK_a:
                        emulator->key_state[7] = false;
                        break;
                    case SDLK_s:
                        emulator->key_state[8] = false;
                        break;
                    case SDLK_d:
                        emulator->key_state[9] = false;
                        break;
                    case SDLK_f:
                        emulator->key_state[14] = false;
                        break;
                    case SDLK_z:
                        emulator->key_state[10] = false;
                        break;
                    case SDLK_x:
                        emulator->key_state[0] = false;
                        break;
                    case SDLK_c:
                        emulator->key_state[11] = false;
                        break;
                    case SDLK_v:
                        emulator->key_state[15] = false;
                        break;
                }
            }
        }
        
        for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++)
            execute_next_instruction(emulator);
        
        decrement_dt(&(emulator->cpu));
        clock_t now = clock() / CLOCKS_PER_SEC;
        prerender_sleep((double)(now - frame_start));
        render_frame(&(emulator->display));
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: chip8-emulator <file>\n");
        exit(1);
    }

    Emulator emulator;
    srand(time(0));

    if (strcmp(argv[1], "--repl") == 0) {
        initialize_display(&(emulator.display));
        repl_loop(&emulator);
    } else {
        char *filepath = argv[1];
        initialize_emulator(&emulator);
        load_rom(emulator.memory, filepath);

        if (argc > 2 && strcmp(argv[2], "--debug") == 0)
            debugger_loop(&emulator);
        else
            execution_loop(&emulator);
    }

    destroy_display(&(emulator.display));
    return 0;
}
