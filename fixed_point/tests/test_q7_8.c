#include <stdio.h>
#include <stdint.h>
#include "fixed_point.h"
#include "test_common.h"
#include "test_q7_8.h"

// Q7.8 conversion macro
#define Q7_8_FROM_FLOAT(x) \
    ((int16_t)((((x) > 127.99609375f) ? 127.99609375f : \
               (((x) < -128.0f) ? -128.0f : (x))) * 256.0f))


// A single math test case
typedef struct {
    float a_f;
    float b_f;

    int32_t add_exp;
    int32_t sub_exp;
    int32_t mul_exp;
    int32_t div_exp;
} fp_test_case_t;

// Q7.8 test vectors
static const fp_test_case_t q7_8_tests[] = {
    {
        .a_f = 0.5f,
        .b_f = 0.25f,
        .add_exp = Q7_8_FROM_FLOAT(0.75f),
        .sub_exp = Q7_8_FROM_FLOAT(0.25f),
        .mul_exp = Q7_8_FROM_FLOAT(0.125f),
        .div_exp = Q7_8_FROM_FLOAT(2.0f),
    },
    {
        .a_f = -0.5f,
        .b_f =  0.75f,
        .add_exp = Q7_8_FROM_FLOAT(0.25f),
        .sub_exp = Q7_8_FROM_FLOAT(-1.25f),
        .mul_exp = Q7_8_FROM_FLOAT(-0.375f),
        .div_exp = Q7_8_FROM_FLOAT(-0.666667f),
    },
    {
        .a_f = 0.9f,
        .b_f = 0.9f,
        .add_exp = Q7_8_FROM_FLOAT(1.8f),
        .sub_exp = Q7_8_FROM_FLOAT(0.0f),
        .mul_exp = Q7_8_FROM_FLOAT(0.80997f),
        .div_exp = Q7_8_FROM_FLOAT(1.0f),
    },
};

static const size_t q7_8_test_count = sizeof(q7_8_tests) / sizeof(q7_8_tests[0]);

void run_q7_8_math_tests(void) {
    printf("\n=== Q7.8 MATH CORRECTNESS TESTS ===\n");

    for (size_t i = 0; i < q7_8_test_count; ++i) {
        const fp_test_case_t* t = &q7_8_tests[i];

        fixed_point_t a = fp_from_float(t->a_f, FP_Q7_8);
        fixed_point_t b = fp_from_float(t->b_f, FP_Q7_8);

        fixed_point_t sum  = fp_add(a, b);
        fixed_point_t diff = fp_sub(a, b);
        fixed_point_t prod = fp_mul(a, b);
        fixed_point_t quot = fp_div(a, b);

        printf("\nCase %zu: a = %f, b = %f\n", i, t->a_f, t->b_f);

        fp_expect_int("add", sum.value.q7_8,  (int16_t)t->add_exp);
        fp_expect_int("sub", diff.value.q7_8, (int16_t)t->sub_exp);
        fp_expect_int_tol("mul", prod.value.q7_8, (int16_t)t->mul_exp, 1);
        fp_expect_int("div", quot.value.q7_8, (int16_t)t->div_exp);
    }
}
