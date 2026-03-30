#include <stdio.h>
#include <stdint.h>
#include "fixed_point.h"
#include "test_common.h"
#include "test_q31.h"

#define Q31_FROM_FLOAT(x) \
    ((int32_t)((((x) > 0.9999999995f) ? 0.9999999995f : (((x) < -1.0f) ? -1.0f : (x))) * 2147483648.0f))

typedef struct {
    float a_f;
    float b_f;

    int64_t add_exp;
    int64_t sub_exp;
    int64_t mul_exp;
    int64_t div_exp;
} fp31_test_case_t;

static const fp31_test_case_t q31_tests[] = {
    {
        .a_f = 0.5f,
        .b_f = 0.25f,
        .add_exp = Q31_FROM_FLOAT(0.75f),
        .sub_exp = Q31_FROM_FLOAT(0.25f),
        .mul_exp = Q31_FROM_FLOAT(0.125f),
        .div_exp = Q31_FROM_FLOAT(1.0f),
    },
    {
        .a_f = -0.5f,
        .b_f = 0.75f,
        .add_exp = Q31_FROM_FLOAT(0.25f),
        .sub_exp = Q31_FROM_FLOAT(-1.25f),
        .mul_exp = Q31_FROM_FLOAT(-0.375f),
        .div_exp = Q31_FROM_FLOAT(-0.666667f),
    },
    {
        .a_f = 0.9f,
        .b_f = 0.9f,
        .add_exp = Q31_FROM_FLOAT(1.8f),
        .sub_exp = Q31_FROM_FLOAT(0.0f),
        .mul_exp = Q31_FROM_FLOAT(0.81f),
        .div_exp = Q31_FROM_FLOAT(1.0f),
    },
};

static const size_t q31_test_count = sizeof(q31_tests) / sizeof(q31_tests[0]);

void run_q31_math_tests(void) {
    printf("\n=== Q31 MATH CORRECTNESS TESTS ===\n");

    for (size_t i = 0; i < q31_test_count; ++i) {
        const fp31_test_case_t* t = &q31_tests[i];

        fixed_point_t a = fp_from_float(t->a_f, FP_Q31);
        fixed_point_t b = fp_from_float(t->b_f, FP_Q31);

        fixed_point_t sum  = fp_add(a, b);
        fixed_point_t diff = fp_sub(a, b);
        fixed_point_t prod = fp_mul(a, b);
        fixed_point_t quot = fp_div(a, b);

        printf("\nCase %zu: a = %f, b = %f\n", i, t->a_f, t->b_f);

        fp_expect_int("add", sum.value.q31,  (int32_t)t->add_exp);
        fp_expect_int("sub", diff.value.q31, (int32_t)t->sub_exp);
        fp_expect_int_tol("mul", prod.value.q31, t->mul_exp, 128);
        fp_expect_int_tol("div", quot.value.q31, t->div_exp, 8192);
    }
}