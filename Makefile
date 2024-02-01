build:
	emcc disco.c emscripten.c eventi.c lavoro.c proteggi.c readkey.c scooter.c scuola.c telefono.c tempo.c tipa.c vestiti.c zarrosim.c -o zarrosim.js -sASYNCIFY

format:
	clang-format -i *.h *.c
