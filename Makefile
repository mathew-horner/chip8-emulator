emu:
	mkdir -p dist
	gcc src/main.c -o dist/chip8-emulator

run: emu
	./dist/chip8-emulator