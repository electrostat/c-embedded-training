#include "Q31.h"

q31_t q31_from_float(float x) {
    // Clamp to representable range
    if (x >= 1.0f)  x = (float)((double)Q31_MAX / (double)Q31_SCALE);
    if (x <= -1.0f) x = -1.0f;

    // Scale using 64-bit intermediate
    int64_t temp = (int64_t)(x * (double)Q31_SCALE);

    // Saturate
    if (temp > Q31_MAX) temp = Q31_MAX;
    if (temp < Q31_MIN) temp = Q31_MIN;

    return (q31_t)temp;
}

float q31_to_float(q31_t x) {
    //real_value = stored_value/2^15
    return (float)((double)x / (double)Q31_SCALE);
}

q31_t q31_add(q31_t a, q31_t b) {
    int64_t sum = (int64_t)a + (int64_t)b; //64-bit conversion

    if (sum > Q31_MAX) return Q31_MAX;
    if (sum < Q31_MIN) return Q31_MIN;

    return (q31_t)sum;
}

q31_t q31_mul(q31_t a, q31_t b) {
    int64_t prod = (int64_t)a * (int64_t)b;   // 64-bit intermediate
    prod >>= Q31_FRACTIONAL_BITS;             // scale back down

    if (prod > Q31_MAX) prod = Q31_MAX;
    if (prod < Q31_MIN) prod = Q31_MIN;

    return (q31_t)prod;
}

q31_t q31_sub(q31_t a, q31_t b) {
    int64_t diff = (int64_t)a - (int64_t)b; //64-bit conversion

    if (diff > Q31_MAX) return Q31_MAX;
    if (diff < Q31_MIN) return Q31_MIN;

    return (q31_t)diff;
}

q31_t q31_div(q31_t a, q31_t b) {
    // Handle divide-by-zero
    if (b == 0) {
        return (a >= 0) ? Q31_MAX : Q31_MIN;
    }

    // Promote to 32-bit and scale
    int64_t dividend = ((int64_t)a << 32);
    int64_t result = dividend / b;

    // Saturate to Q15 range
    if (result > Q31_MAX) result = Q31_MAX;
    if (result < Q31_MIN) result = Q31_MIN;

    return (q31_t)result;
}