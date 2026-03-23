//16‑bit signed integer math
#include <stdint.h>

typedef int16_t q15_t;
#define Q15_FRACTIONAL_BITS 15
#define Q15_SCALE (1 << Q15_FRACTIONAL_BITS)

q15_t q15_from_float(float x);
float q15_to_float(q15_t x);
q15_t q15_add(q15_t a, q15_t b);
q15_t q15_mul(q15_t a, q15_t b);
q15_t q15_sub(q15_t a, q15_t b);
q15_t q15_div(q15_t a, q15_t b);