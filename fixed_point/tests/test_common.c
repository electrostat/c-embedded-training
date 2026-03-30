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