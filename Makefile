sources = disco.c emscripten.c eventi.c handler.c lavoro.c message.c proteggi.c readkey.c scooter.c scuola.c tabimg.c telefono.c tempo.c tipa.c vestiti.c zarrosim.c

# CFLAGS += -Wall -Wextra -Wpedantic \
#          -Wformat=2 -Wno-unused-parameter -Wshadow \
#          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
#          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs

build:
	emcc $(sources) -o zarrosim.js \
		$(CFLAGS) \
		-s ASYNCIFY \
		-s ASSERTIONS=2 \
		-s SAFE_HEAP=1 \
		-s STACK_OVERFLOW_CHECK=2 \
		-s DEMANGLE_SUPPORT=1 \
		-s 'ASYNCIFY_IMPORTS=["emscripten_asm_const_int"]' \
		-s 'EXPORTED_RUNTIME_METHODS=["ccall", "setValue"]' \
		-s 'EXPORTED_FUNCTIONS=["_main", "_PostMessage", "_PrintMessages", "_GetHandle"]'

format:
	clang-format -i *.h *.c
	js-beautify -r resources/dialogs/js/twcc.js
