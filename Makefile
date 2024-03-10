chip8:
	gcc -Wall -o chip8_em cpu.c -ggdb


clean:
	rm chip8_em
