emu:
	mkdir -p dist
	gcc src/main.c src/cpu.c src/errors.c src/graphics.c src/io.c -o dist/chip8-emulator -lSDL2

run: emu
	./dist/chip8-emulator
