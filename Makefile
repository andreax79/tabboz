sources = disco.c emscripten.c eventi.c handler.c lavoro.c proteggi.c readkey.c scooter.c scuola.c telefono.c tempo.c tipa.c vestiti.c zarrosim.c

build:
	emcc $(sources) -o zarrosim.js -sASYNCIFY

format:
	clang-format -i *.h *.c
