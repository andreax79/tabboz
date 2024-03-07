sources = disco.c \
		  eventi.c \
		  lavoro.c \
		  proteggi.c \
		  readkey.c \
		  scooter.c \
		  scuola.c \
		  tabimg.c \
		  telefono.c \
		  tempo.c \
		  tipa.c \
		  vestiti.c \
		  zarrosim.c \
		  novantotto/*.c

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
		-s 'EXPORTED_FUNCTIONS=["_main", "_GetDlgItem", "_PostMessage", "_PrintMessages", "_AllocateDlgItem", "_GetTickCount", "_WinMainStartup"]'

format:
	clang-format -i *.h *.c novantotto/*.h novantotto/*.c novantotto/tests/*.c
	js-beautify -r novantotto/js/novantotto.js

test:
	make -C novantotto test

export:
	RESOURCES_DIR="./resources" ./novantotto/export/export.sh *.RES

dialogs:
	RESOURCES_DIR="./resources/" ./novantotto/export/rc.sh resources/dialogs/dialogs.rc
