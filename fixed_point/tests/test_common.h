#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdint.h>

#define GREEN   "\x1b[32m"
#define RED     "\x1b[31m"
#define RESET   "\x1b[0m"

// Shared PASS/FAIL helper
void fp_expect_int(const char* label, int32_t result, int32_t expected);

#endif