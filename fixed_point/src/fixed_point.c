#include "fixed_point.h"
#include <stdio.h>
#include <assert.h>

// Internal helper: ensure both operands share the same format
static void fp_assert_same_format(fixed_point_t a, fixed_point_t b) {
    assert(a.format == b.format);
}

// Conversion from float to unified fixed_point_t
fixed_point_t fp_from_float(float x, fp_format_t fmt) {
    fixed_point_t out;
    out.format = fmt;

    switch (fmt) {
        case FP_Q15:
            out.value.q15 = q15_from_float(x);
            break;

        case FP_Q31:
            out.value.q31 = q31_from_float(x);
            break;

        case FP_Q7_8:
            out.value.q7_8 = q7_8_from_float(x);
            break;

        default:
            // Unknown format: zero out
            out.value.q31 = 0;
            break;
    }

    return out;
}


// Conversion from unified fixed_point_t to float
float fp_to_float(fixed_point_t x) {
    switch (x.format) {
        case FP_Q15:
            return q15_to_float(x.value.q15);

        case FP_Q31:
            return q31_to_float(x.value.q31);

        case FP_Q7_8:
            return q7_8_to_float(x.value.q7_8);

        default:
            return 0.0f;
    }
}

fixed_point_t fp_add(fixed_point_t a, fixed_point_t b) {
    fp_assert_same_format(a, b);

    fixed_point_t out;
    out.format = a.format;

    switch (a.format) {
        case FP_Q15:
            out.value.q15 = q15_add(a.value.q15, b.value.q15);
            break;

        case FP_Q31:
            out.value.q31 = q31_add(a.value.q31, b.value.q31);
            break;

        case FP_Q7_8:
            out.value.q7_8 = q7_8_add(a.value.q7_8, b.value.q7_8);
            break;

        default:
            out.value.q31 = 0;
            break;
    }

    return out;
}

fixed_point_t fp_sub(fixed_point_t a, fixed_point_t b) {
    fp_assert_same_format(a, b);

    fixed_point_t out;
    out.format = a.format;

    switch (a.format) {
        case FP_Q15:
            out.value.q15 = q15_sub(a.value.q15, b.value.q15);
            break;

        case FP_Q31:
            out.value.q31 = q31_sub(a.value.q31, b.value.q31);
            break;

        case FP_Q7_8:
            out.value.q7_8 = q7_8_sub(a.value.q7_8, b.value.q7_8);
            break;

        default:
            out.value.q31 = 0;
            break;
    }

    return out;
}

fixed_point_t fp_mul(fixed_point_t a, fixed_point_t b) {
    fp_assert_same_format(a, b);

    fixed_point_t out;
    out.format = a.format;

    switch (a.format) {
        case FP_Q15:
            out.value.q15 = q15_mul(a.value.q15, b.value.q15);
            break;

        case FP_Q31:
            out.value.q31 = q31_mul(a.value.q31, b.value.q31);
            break;

        case FP_Q7_8:
            out.value.q7_8 = q7_8_mul(a.value.q7_8, b.value.q7_8);
            break;

        default:
            out.value.q31 = 0;
            break;
    }

    return out;
}

fixed_point_t fp_div(fixed_point_t a, fixed_point_t b) {
    fp_assert_same_format(a, b);

    fixed_point_t out;
    out.format = a.format;

    switch (a.format) {
        case FP_Q15:
            out.value.q15 = q15_div(a.value.q15, b.value.q15);
            break;

        case FP_Q31:
            out.value.q31 = q31_div(a.value.q31, b.value.q31);
            break;

        case FP_Q7_8:
            out.value.q7_8 = q7_8_div(a.value.q7_8, b.value.q7_8);
            break;

        default:
            out.value.q31 = 0;
            break;
    }

    return out;
}