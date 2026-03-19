//16‑bit signed integer math
#include <stdint.h>

typedef int16_t q15_t;
#define Q15_FRACTIONAL_BITS 15
#define Q15_SCALE (1 << Q15_FRACTIONAL_BITS)