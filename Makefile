sources = disco.c emscripten.c eventi.c handler.c lavoro.c message.c proteggi.c readkey.c scooter.c scuola.c tabimg.c telefono.c tempo.c tipa.c vestiti.c zarrosim.c

build:
	emcc -O1 $(sources) -o zarrosim.js -sASYNCIFY \
		-s 'ASYNCIFY_IMPORTS=["emscripten_asm_const_int"]' \
		-s 'EXPORTED_RUNTIME_METHODS=["ccall","setValue"]' \
		-s 'EXPORTED_FUNCTIONS=["_main"]'

format:
	clang-format -i *.h *.c
	js-beautify -r resources/dialogs/js/twcc.js
