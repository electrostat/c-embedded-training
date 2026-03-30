#include <stdio.h>
#include "test_common.h"

void fp_expect_int(const char* label, int32_t result, int32_t expected) {
    if (result == expected) {
        printf(GREEN "[PASS]" RESET " %s = %d (expected %d)\n",
               label, result, expected);
    } else {
        printf(RED "[FAIL]" RESET " %s = %d (expected %d)\n",
               label, result, expected);
    }
}

void fp_expect_int_tol(const char* label, int64_t result, int64_t expected, int64_t tol)
{
    int64_t diff = result - expected;
    if (diff <= tol && diff >= -tol) {
        printf(GREEN "[PASS]" RESET " %s = %lld (expected %lld)\n",
               label, result, expected);
    } else {
        printf(RED "[FAIL]" RESET " %s = %lld (expected %lld, diff=%lld)\n",
               label, result, expected, diff);
    }
}