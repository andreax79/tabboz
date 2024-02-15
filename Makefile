sources = disco.c emscripten.c eventi.c handler.c lavoro.c proteggi.c readkey.c scooter.c scuola.c tabimg.c telefono.c tempo.c tipa.c vestiti.c zarrosim.c

build:
	emcc $(sources) -o zarrosim.js -sASYNCIFY -s 'ASYNCIFY_IMPORTS=["emscripten_asm_const_int"]'

format:
	clang-format -i *.h *.c
	js-beautify -r resources/dialogs/js/twcc.js
