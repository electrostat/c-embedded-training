#include "test_q15.h"
#include "test_q31.h"
#include "test_q7_8.h"

int main(void) {
    run_q15_math_tests();
    run_q31_math_tests();
    run_q7_8_math_tests();
    return 0;
}