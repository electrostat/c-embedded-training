#include <stdio.h>
#include <stdint.h>
#include "fixed_point.h"

// ANSI colors
#define GREEN   "\x1b[32m"
#define RED     "\x1b[31m"
#define RESET   "\x1b[0m"

#define Q15_FROM_FLOAT(x) \
    ((int16_t)((((x) > 1.0f) ? 1.0f : (((x) < -1.0f) ? -1.0f : (x))) * 32768.0f))


// PASS/FAIL for integer comparisons
static void fp_expect_int(const char* label, int32_t result, int32_t expected) {
    if (result == expected) {
        printf(GREEN "[PASS]" RESET " %s = %d (expected %d)\n",
               label, result, expected);
    } else {
        printf(RED "[FAIL]" RESET " %s = %d (expected %d)\n",
               label, result, expected);
    }
}

// A single math test case
typedef struct {
    float a_f;
    float b_f;

    int32_t add_exp;
    int32_t sub_exp;
    int32_t mul_exp;
    int32_t div_exp;
} fp_test_case_t;

// Q15 scale
#define Q15_TEST_SCALE 32768.0f
#define Q15_SAT_MAX  32767
#define Q15_SAT_MIN -32768

// Define Q15 math-correctness test vectors
static const fp_test_case_t q15_tests[] = {
    // 0.5 and 0.25
    {
        .a_f = 0.5f,
        .b_f = 0.25f,
        .add_exp = Q15_FROM_FLOAT(0.75f),
        .sub_exp = Q15_FROM_FLOAT(0.25f),
        .mul_exp = Q15_FROM_FLOAT(0.125f),
        .div_exp = Q15_FROM_FLOAT(1.0f),
    },
    // -0.5 and 0.75
    {
        .a_f = -0.5f,
        .b_f =  0.75f,
        .add_exp = Q15_FROM_FLOAT(0.25f),
        .sub_exp = Q15_FROM_FLOAT(-1.25f),   // saturates to -1.0
        .mul_exp = Q15_FROM_FLOAT(-0.375f),
        .div_exp = Q15_FROM_FLOAT(-0.666667f),
    },
    // 0.9 and 0.9
    {
        .a_f = 0.9f,
        .b_f = 0.9f,
        .add_exp = Q15_FROM_FLOAT(1.8f),     // saturates to +1.0
        .sub_exp = Q15_FROM_FLOAT(0.0f),
        .mul_exp = Q15_FROM_FLOAT(0.80997f),
        .div_exp = Q15_FROM_FLOAT(1.0f),
    },
};

static const size_t q15_test_count = sizeof(q15_tests) / sizeof(q15_tests[0]);

// Run Q15 math correctness tests
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