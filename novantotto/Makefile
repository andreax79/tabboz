test: test-handler test-message

.PHONY: test-handler
test-handler:
	emcc -DDEBUG=1 \
		-sWASM=0 \
		-sASSERTIONS=2 \
		-sSTACK_OVERFLOW_CHECK=2 \
		-sSAFE_HEAP=1 \
		tests/test_handler.c handler.c property.c && node a.out.js
	rm -f a.out.js

.PHONY: test-message
test-message:
	emcc -DDEBUG=1 \
		-sWASM=0 \
		-sASSERTIONS=2 \
		-sSTACK_OVERFLOW_CHECK=2 \
		-sSAFE_HEAP=1 \
		tests/test_message.c message.c handler.c property.c && node a.out.js
	rm -f a.out.js

format:
	clang-format -i *.h *.c tests/*.c
