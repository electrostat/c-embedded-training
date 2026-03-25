//32‑bit signed integer math
#include <stdint.h>

typedef int32_t q31_t;
#define Q31_FRACTIONAL_BITS 31
#define Q31_SCALE (1LL << Q31_FRACTIONAL_BITS)
#define Q31_MAX  0x7FFFFFFF
#define Q31_MIN  0x80000000

//functions
q31_t q31_from_float(float x);
float q31_to_float(q31_t x);
q31_t q31_add(q31_t a, q31_t b);
q31_t q31_mul(q31_t a, q31_t b);
q31_t q31_sub(q31_t a, q31_t b);
q31_t q31_div(q31_t a, q31_t b);