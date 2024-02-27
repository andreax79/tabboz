// clang-format off
#ifdef DEBUG
#define DEBUG_PRINTF(...) printf("DEBUG: " __VA_ARGS__)
#else
#define DEBUG_PRINTF(...) do {} while (0)
#endif
// clang-format on
