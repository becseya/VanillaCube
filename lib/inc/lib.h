#ifndef __VANILLA_CUBE_LIB_H
#define __VANILLA_CUBE_LIB_H 1

#include <stdint.h>

// compiler attributes
#define ALWAYS_INLINE inline __attribute__((always_inline))

// helper macros
#define N_ONES(n)         ((1 << (n + 1)) - 1)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#endif // __VANILLA_CUBE_LIB_H
