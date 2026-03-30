#include <stdio.h>
#include <stdint.h>
#include "fixed_point.h"
#include "test_common.h"
#include "test_q15.h"

// Q15 conversion macro
#define Q15_FROM_FLOAT(x) \
    ((int16_t)((((x) > 1.0f) ? 1.0f : (((x) < -1.0f) ? -1.0f : (x))) * 32768.0f))

// A single math test case
typedef struct {
    float a_f;
    float b_f;

    int32_t add_exp;
    int32_t sub_exp;
    int32_t mul_exp;
    int32_t div_exp;
} fp_test_case_t;

// Q15 test vectors
static const fp_test_case_t q15_tests[] = {
    {
        .a_f = 0.5f,
        .b_f = 0.25f,
        .add_exp = Q15_FROM_FLOAT(0.75f),
        .sub_exp = Q15_FROM_FLOAT(0.25f),
        .mul_exp = Q15_FROM_FLOAT(0.125f),
        .div_exp = Q15_FROM_FLOAT(1.0f),
    },
    {
        .a_f = -0.5f,
        .b_f =  0.75f,
        .add_exp = Q15_FROM_FLOAT(0.25f),
        .sub_exp = Q15_FROM_FLOAT(-1.25f),
        .mul_exp = Q15_FROM_FLOAT(-0.375f),
        .div_exp = Q15_FROM_FLOAT(-0.666667f),
    },
    {
        .a_f = 0.9f,
        .b_f = 0.9f,
        .add_exp = Q15_FROM_FLOAT(1.8f),
        .sub_exp = Q15_FROM_FLOAT(0.0f),
        .mul_exp = Q15_FROM_FLOAT(0.80997f),
        .div_exp = Q15_FROM_FLOAT(1.0f),
    },
};

static const size_t q15_test_count = sizeof(q15_tests) / sizeof(q15_tests[0]);

void run_q15_math_tests(void) {
    printf("\n=== Q15 MATH CORRECTNESS TESTS ===\n");

    for (size_t i = 0; i < q15_test_count; ++i) {
        const fp_test_case_t* t = &q15_tests[i];

        fixed_point_t a = fp_from_float(t->a_f, FP_Q15);
        fixed_point_t b = fp_from_float(t->b_f, FP_Q15);

        fixed_point_t sum  = fp_add(a, b);
        fixed_point_t diff = fp_sub(a, b);
        fixed_point_t prod = fp_mul(a, b);
        fixed_point_t quot = fp_div(a, b);

        printf("\nCase %zu: a = %f, b = %f\n", i, t->a_f, t->b_f);

        fp_expect_int("add", sum.value.q15,  (int16_t)t->add_exp);
        fp_expect_int("sub", diff.value.q15, (int16_t)t->sub_exp);
        fp_expect_int("mul", prod.value.q15, (int16_t)t->mul_exp);
        fp_expect_int("div", quot.value.q15, (int16_t)t->div_exp);
    }
}
