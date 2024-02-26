format:
	clang-format -i *.h *.c

test:
	emcc -DDEBUG=1 \
		-sWASM=0 \
		-sASSERTIONS=2 \
		-sSTACK_OVERFLOW_CHECK=2 \
		-sSAFE_HEAP=1 \
		tests/test_handler.c handler.c && node a.out.js
	rm -f a.out.js
