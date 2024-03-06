test: test-handler test-message test-registry test-profile

.PHONY: test-handler
test-handler:
	@emcc \
		-sWASM=0 \
		-sASSERTIONS=2 \
		-sSTACK_OVERFLOW_CHECK=2 \
		-sSAFE_HEAP=1 \
		tests/test_handler.c handler.c property.c && node a.out.js
	@rm -f a.out.js

.PHONY: test-message
test-message:
	@emcc \
		-sWASM=0 \
		-sASSERTIONS=2 \
		-sSTACK_OVERFLOW_CHECK=2 \
		-sSAFE_HEAP=1 \
		tests/test_message.c message.c handler.c property.c && node a.out.js
	@rm -f a.out.js

.PHONY: test-registry
test-registry:
	@emcc \
		-sWASM=0 \
		-sASSERTIONS=2 \
		-sSTACK_OVERFLOW_CHECK=2 \
		-sSAFE_HEAP=1 \
		tests/test_registry.c registry.c handler.c property.c
	@mv a.out.js a.out.js_
	@cat tests/LocalStorage.js a.out.js_ > a.out.js
	@node a.out.js
	@rm -f a.out.js a.out.js_

.PHONY: test-profile
test-profile:
	@emcc \
		-sWASM=0 \
		-sASSERTIONS=2 \
		-sSTACK_OVERFLOW_CHECK=2 \
		-sSAFE_HEAP=1 \
		tests/test_profile.c profile.c
	@node a.out.js
	@rm -f a.out.js

format:
	clang-format -i *.h *.c tests/*.c
