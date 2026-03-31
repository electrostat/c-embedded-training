#ifndef Q7_8_H
#define Q7_8_H

#include <stdint.h>

typedef int16_t q7_8_t;
#define Q7_8_FRACTIONAL_BITS 15
#define Q7_8_SCALE (1 << Q7_8_FRACTIONAL_BITS)

q7_8_t q7_8_from_float(float x);
float q7_8_to_float(q7_8_t x);
q7_8_t q7_8_add(q7_8_t a, q7_8_t b);
q7_8_t q7_8_sub(q7_8_t a, q7_8_t b);
q7_8_t q7_8_mul(q7_8_t a, q7_8_t b);
q7_8_t q7_8_div(q7_8_t a, q7_8_t b);

#endif