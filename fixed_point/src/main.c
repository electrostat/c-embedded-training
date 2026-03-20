#include <stdio.h>
#include "Q15.h"

void test_add(float a_f, float b_f) {
    q15_t a = q15_from_float(a_f);
    q15_t b = q15_from_float(b_f);

    q15_t result = q15_add(a, b);

    float result_f = q15_to_float(result);

    printf("ADD TEST: %f + %f = %f\n", a_f, b_f, result_f);
}

void test_mul(float a_f, float b_f) {
    q15_t a = q15_from_float(a_f);
    q15_t b = q15_from_float(b_f);

    q15_t result = q15_mul(a, b);

    float result_f = q15_to_float(result);

    printf("MUL TEST: %f * %f = %f\n", a_f, b_f, result_f);
}

void test_sub(float a_f, float b_f) {
    q15_t a = q15_from_float(a_f);
    q15_t b = q15_from_float(b_f);

    q15_t result = q15_sub(a, b);

    float result_f = q15_to_float(result);

    printf("Sub TEST: %f + %f = %f\n", a_f, b_f, result_f);
}

int main(void) {
    printf("=== Q15 ADDITION TESTS ===\n");
    test_add(0.5f, 0.25f);
    test_add(-0.5f, 0.75f);
    test_add(0.9f, 0.9f);      // saturation test
    test_add(-1.0f, -0.5f);    // negative saturation

    printf("\n=== Q15 MULTIPLICATION TESTS ===\n");
    test_mul(0.5f, 0.5f);
    test_mul(-0.5f, 0.5f);
    test_mul(0.9f, 0.9f);      // saturation test
    test_mul(-1.0f, -1.0f);    // edge case

    printf("\n=== Q15 SUBTRACTION TESTS ===\n");
    test_sub(0.5f, 0.25f);
    test_sub(-0.5f, 0.75f);
    test_sub(0.9f, -0.9f);     // saturation
    test_sub(-1.0f, 0.5f);     // negative saturation

    return 0;
}