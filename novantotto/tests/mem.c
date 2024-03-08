// emcc mem.c -o mem.js

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char          *LPSTR;
typedef unsigned short WORD;
typedef unsigned long  ULONG_PTR;
typedef unsigned long  DWORD_PTR;

#define LOWORD(l) ((WORD)((DWORD_PTR)(l)&0xffff))
#define HIWORD(l) ((WORD)(((DWORD_PTR)(l) >> 16) & 0xffff))
#define MAKEINTRESOURCE(i) ((LPSTR)(ULONG_PTR)LOWORD(i))
#define IS_NAME_RESOURCE(l) (HIWORD((unsigned long)l) != 0)
#define IS_INT_RESOURCE(l) (HIWORD((unsigned long)l) == 0)

const char *static1 = "test";
const char *static2 = "test2";

int main(int argc, char **argv)
{
    void *d1 = malloc(10);
    void *d2 = "test3";

    printf("addr static1: 0x%8x %d\n", (unsigned int)static1, IS_NAME_RESOURCE(static1));
    printf("addr static2: 0x%8x %d\n", (unsigned int)static2, IS_NAME_RESOURCE(static2));
    printf("addr d1:      0x%8x %d\n", (unsigned int)d1, IS_NAME_RESOURCE(d1));
    printf("addr d2:      0x%8x %d\n", (unsigned int)d2, IS_NAME_RESOURCE(d2));

    LPSTR d3 = "test string";
    printf("addr d3:      0x%8x %d\n", (unsigned int)d3, IS_NAME_RESOURCE(d3));
    LPSTR d4 = MAKEINTRESOURCE(0xfedc);
    printf("addr d4:      0x%8x %d\n", (unsigned int)d4, IS_NAME_RESOURCE(d4));
    LPSTR d5 = MAKEINTRESOURCE(0xffff);
    printf("addr d5:      0x%8x %d\n", (unsigned int)d5, IS_NAME_RESOURCE(d5));

    LPSTR d6 = strdup(d3);
    printf("addr d6:      0x%8x %d\n", (unsigned int)d6, IS_NAME_RESOURCE(d6));
    LPSTR d7 = strdup(d4);
    printf("addr d7:      0x%8x %d\n", (unsigned int)d7, IS_NAME_RESOURCE(d7));
}
