/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <emscripten.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "windefs.h"

#define _JS_ARG_0 ""
#define _JS_ARG_1 "$0"
#define _JS_ARG_2 "$0,$1"
#define _JS_ARG_3 "$0,$1,$2"
#define _JS_ARG_4 "$0,$1,$2,$3"
#define _JS_ARG_5 "$0,$1,$2,$3,$4"
#define _JS_ARG_6 "$0,$1,$2,$3,$4,$5"
#define _JS_ARG_7 "$0,$1,$2,$3,$4,$5,$6"
#define _JS_ARG_8 "$0,$1,$2,$3,$4,$5,$6,$7"
#define _JS_ARG_9 "$0,$1,$2,$3,$4,$5,$6,$7,$8"
#define _JS_ARG_10 "$0,$1,$2,$3,$4,$5,$6,$7,$8,$9"
#define _JS_ARG(...) _EM_ASM_CONCATENATE(_JS_ARG_, _EM_ASM_COUNT_ARGS(__VA_ARGS__))

#define JS_CALL_INT(code, ...) emscripten_asm_const_int(CODE_EXPR("return " code "(" _JS_ARG(__VA_ARGS__) ")") _EM_ASM_PREP_ARGS(__VA_ARGS__))
#define JS_ASYNC_CALL_INT(code, ...) emscripten_asm_const_int(CODE_EXPR("{return Asyncify.handleAsync(function(){ return " code "(" _JS_ARG(__VA_ARGS__) ")})}") _EM_ASM_PREP_ARGS(__VA_ARGS__))
#define JS_CALL(code, ...) (void)JS_CALL_INT(code, __VA_ARGS__)
#define JS_ASYNC_CALL(code, ...) (void)JS_ASYNC_CALL_INT(code, __VA_ARGS__)
