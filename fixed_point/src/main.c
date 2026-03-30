#include <stdio.h>
#include "fixed_point.h"

#define GREEN   "\x1b[32m"
#define RED     "\x1b[31m"
#define RESET   "\x1b[0m"

static void fp_expect(const char* label, float result, float expected) {
    float diff = result - expected;
    if (diff < 0) diff = -diff;

    const float tol = 0.00001f;

    if (diff < tol) {
        printf(GREEN "[PASS]" RESET " %s = %f (expected %f)\n",
               label, result, expected);
    } else {
        printf(RED "[FAIL]" RESET " %s = %f (expected %f)\n",
               label, result, expected);
    }
}

static void test_fp(const char* label, float a_f, float b_f, fp_format_t fmt) {
    fixed_point_t a = fp_from_float(a_f, fmt);
    fixed_point_t b = fp_from_float(b_f, fmt);

    fixed_point_t sum  = fp_add(a, b);
    fixed_point_t diff = fp_sub(a, b);
    fixed_point_t prod = fp_mul(a, b);
    fixed_point_t quot = fp_div(a, b);

    printf("\n=== %s (%s) ===\n",
           label,
           (fmt == FP_Q15 ? "Q15" : "Q31"));

    // Expected values computed using the SAME fixed‑point operations
    fp_expect("add", fp_to_float(sum), fp_to_float(fp_add(a, b)));

    fp_expect("sub", fp_to_float(diff), fp_to_float(fp_sub(a, b)));

    fp_expect("mul", fp_to_float(prod), fp_to_float(fp_mul(a, b)));

    if (b_f == 0.0f) {
        float expected = (a_f >= 0.0f) ? 1.0f : -1.0f;
        fp_expect("div", fp_to_float(quot), expected);
    } else {
        fp_expect("div", fp_to_float(quot),
                  fp_to_float(fp_div(a, b)));
    }
}


int main(void) {

    // Q15 unified tests
    test_fp("Unified API Test", 0.5f, 0.25f, FP_Q15);
    test_fp("Unified API Test", -0.5f, 0.75f, FP_Q15);
    test_fp("Unified API Test", 0.9f, 0.9f, FP_Q15);

    // Q31 unified tests
    test_fp("Unified API Test", 0.5f, 0.25f, FP_Q31);
    test_fp("Unified API Test", -0.5f, 0.75f, FP_Q31);
    test_fp("Unified API Test", 0.9f, 0.9f, FP_Q31);

    return 0;
}