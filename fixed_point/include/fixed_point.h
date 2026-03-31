#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>
#include "Q15.h"
#include "Q31.h"
#include "Q7_8.h"

// Supported fixed‑point formats
typedef enum {
    FP_Q15,
    FP_Q31,
    FP_Q7_8
    // add future formats here
} fp_format_t;

// Unified fixed‑point type
typedef struct {
    fp_format_t format;
    union {
        q15_t q15;
        q31_t q31;
        q7_8_t q7_8;
    } value;
} fixed_point_t;

// Conversion
fixed_point_t fp_from_float(float x, fp_format_t fmt);
float fp_to_float(fixed_point_t x);

// Arithmetic
fixed_point_t fp_add(fixed_point_t a, fixed_point_t b);
fixed_point_t fp_sub(fixed_point_t a, fixed_point_t b);
fixed_point_t fp_mul(fixed_point_t a, fixed_point_t b);
fixed_point_t fp_div(fixed_point_t a, fixed_point_t b);

#endif