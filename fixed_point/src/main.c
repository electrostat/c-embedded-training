#include <stdio.h>
#include "Q15.h"
#include "Q31.h"

void test_q15_add(float a_f, float b_f, float expected) {
    q15_t a = q15_from_float(a_f);
    q15_t b = q15_from_float(b_f);

    q15_t result = q15_add(a, b);
    float result_f = q15_to_float(result);

    float diff = result_f - expected;
    if (diff < 0) diff = -diff;

    if (diff < 0.0001f)
        printf("[PASS] %f + %f = %f\n", a_f, b_f, result_f);
    else
        printf("[FAIL] %f + %f = %f (expected %f)\n", a_f, b_f, result_f, expected);
}

void test_q15_mul(float a_f, float b_f, float expected) {
    q15_t a = q15_from_float(a_f);
    q15_t b = q15_from_float(b_f);

    q15_t result = q15_mul(a, b);
    float result_f = q15_to_float(result);

    float diff = result_f - expected;
    if (diff < 0) diff = -diff;

    if (diff < 0.0001f)
        printf("[PASS] %f + %f = %f\n", a_f, b_f, result_f);
    else
        printf("[FAIL] %f + %f = %f (expected %f)\n", a_f, b_f, result_f, expected);
}

void test_q15_sub(float a_f, float b_f, float expected) {
    q15_t a = q15_from_float(a_f);
    q15_t b = q15_from_float(b_f);

    q15_t result = q15_sub(a, b);
    float result_f = q15_to_float(result);

    float diff = result_f - expected;
    if (diff < 0) diff = -diff;

    if (diff < 0.0001f)
        printf("[PASS] %f - %f = %f\n", a_f, b_f, result_f);
    else
        printf("[FAIL] %f - %f = %f (expected %f)\n", a_f, b_f, result_f, expected);
}

void test_q15_div(float a_f, float b_f, float expected) {
    q15_t a = q15_from_float(a_f);
    q15_t b = q15_from_float(b_f);

    q15_t result = q15_div(a, b);
    float result_f = q15_to_float(result);

    float diff = result_f - expected;
    if (diff < 0) diff = -diff;

    if (diff < 0.0001f)
        printf("[PASS] %f / %f = %f\n", a_f, b_f, result_f);
    else
        printf("[FAIL] %f / %f = %f (expected %f)\n",
               a_f, b_f, result_f, expected);
}

void test_q31_add(float a_f, float b_f, float expected) {
    q31_t a = q31_from_float(a_f);
    q31_t b = q31_from_float(b_f);

    q31_t result = q31_add(a, b);
    float result_f = q31_to_float(result);

    float diff = result_f - expected;
    if (diff < 0) diff = -diff;

    if (diff < 0.0000001f)
        printf("[PASS] %f + %f = %f\n", a_f, b_f, result_f);
    else
        printf("[FAIL] %f + %f = %f (expected %f)\n",
               a_f, b_f, result_f, expected);
}

void test_q31_sub(float a_f, float b_f, float expected) {
    q31_t a = q31_from_float(a_f);
    q31_t b = q31_from_float(b_f);

    q31_t result = q31_sub(a, b);
    float result_f = q31_to_float(result);

    float diff = result_f - expected;
    if (diff < 0) diff = -diff;

    if (diff < 0.0000001f)
        printf("[PASS] %f - %f = %f\n", a_f, b_f, result_f);
    else
        printf("[FAIL] %f - %f = %f (expected %f)\n",
               a_f, b_f, result_f, expected);
}

void test_q31_mul(float a_f, float b_f, float expected) {
    q31_t a = q31_from_float(a_f);
    q31_t b = q31_from_float(b_f);

    q31_t result = q31_mul(a, b);
    float result_f = q31_to_float(result);

    float diff = result_f - expected;
    if (diff < 0) diff = -diff;

    if (diff < 0.0000001f)
        printf("[PASS] %f * %f = %f\n", a_f, b_f, result_f);
    else
        printf("[FAIL] %f * %f = %f (expected %f)\n",
               a_f, b_f, result_f, expected);
}

void test_q31_div(float a_f, float b_f, float expected) {
    q31_t a = q31_from_float(a_f);
    q31_t b = q31_from_float(b_f);

    q31_t result = q31_div(a, b);
    float result_f = q31_to_float(result);

    float diff = result_f - expected;
    if (diff < 0) diff = -diff;

    if (diff < 0.0000001f)
        printf("[PASS] %f / %f = %f\n", a_f, b_f, result_f);
    else
        printf("[FAIL] %f / %f = %f (expected %f)\n",
               a_f, b_f, result_f, expected);
}


int main(void) {
    printf("=== Q15 ADDITION TESTS ===\n");
    test_q15_add(0.5f, 0.25f, 0.75f);
    test_q15_add(-0.5f, 0.75f, 0.25f);
    test_q15_add(0.9f, 0.9f, 0.999969f);   // saturation
    test_q15_add(-1.0f, -0.5f, -1.0f);     // saturation

    printf("\n=== Q15 MULTIPLICATION TESTS ===\n");
    test_q15_mul(0.5f, 0.5f, 0.25f);
    test_q15_mul(-0.5f, 0.5f, -0.25f);
    test_q15_mul(0.9f, 0.9f, 0.81f);
    test_q15_mul(-1.0f, -1.0f, 0.999969f); // saturation

    printf("\n=== Q15 SUBTRACTION TESTS ===\n");
    test_q15_sub(0.5f, 0.25f, 0.25f);
    test_q15_sub(-0.5f, 0.75f, -1.0f);
    test_q15_sub(0.9f, -0.9f, 0.999969f);     // saturation
    test_q15_sub(-1.0f, 0.5f, -1.0f);     // negative saturation

    printf("\n=== Q15 DIVISION TESTS ===\n");
    test_q15_div(0.5f, 0.5f, 1.0f);
    test_q15_div(0.25f, 0.5f, 0.5f);
    test_q15_div(-0.5f, 0.25f, -1.0f);       // saturates to -1.0
    test_q15_div(0.9f, 0.1f, 0.999969f);          // saturates to +0.999969
    test_q15_div(0.5f, 0.0f, 0.999969f);     // divide by zero → +max

    printf("\n=== Q31 ADDITION TESTS ===\n");
    test_q31_add(0.5f, 0.25f, 0.75f);
    test_q31_add(-0.5f, 0.75f, 0.25f);
    test_q31_add(0.9f, 0.9f, 0.9999999995f);   // saturation
    test_q31_add(-1.0f, -0.5f, -1.0f);         // saturation

    printf("\n=== Q31 SUBTRACTION TESTS ===\n");
    test_q31_sub(0.5f, 0.25f, 0.25f);
    test_q31_sub(-0.5f, 0.75f, -1.0f);         // saturation
    test_q31_sub(0.9f, -0.9f, 0.9999999995f);  // saturation
    test_q31_sub(-1.0f, 0.5f, -1.0f);          // saturation

    printf("\n=== Q31 MULTIPLICATION TESTS ===\n");
    test_q31_mul(0.5f, 0.5f, 0.25f);
    test_q31_mul(-0.5f, 0.5f, -0.25f);
    test_q31_mul(0.9f, 0.9f, 0.81f);
    test_q31_mul(-1.0f, -1.0f, 0.9999999995f); // saturation

    printf("\n=== Q31 DIVISION TESTS ===\n");
    test_q31_div(0.5f, 0.5f, 1.0f);
    test_q31_div(0.25f, 0.5f, 0.5f);
    test_q31_div(-0.5f, 0.25f, -1.0f);         // saturates (true result -2.0)
    test_q31_div(0.9f, 0.1f, 0.9999999995f);   // saturates (true result 9.0)
    test_q31_div(0.5f, 0.0f, 0.9999999995f);   // divide by zero → +max

    return 0;
}