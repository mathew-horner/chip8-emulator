SOURCE_FILES = src/cpu.c src/debugger.c src/display.c src/emulator.c src/errors.c src/io.c
TEST_FILES = tests/test_cpu.c tests/test_emulator.c

emu:
	mkdir -p dist
	gcc -o dist/chip8-emulator $(SOURCE_FILES) src/main.c -lSDL2 -g

test:
	mkdir -p build
	gcc -o build/test $(TEST_FILES) $(SOURCE_FILES) -lcriterion -lSDL2
	./build/test