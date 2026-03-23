#include "Q15.h"

q15_t q15_from_float(float x) {
    // clamp input to valid range - Q15 max byte size
    //since 1 byte is being used for sign, can't get to 1 (32767)
    if (x > 0.999969f) x = 0.999969f;
    //however -1 is representable (-32768)
    if (x < -1.0f)     x = -1.0f;

    //stored_value=real_value×2^15
    return (q15_t)(x * Q15_SCALE);
}

float q15_to_float(q15_t x) {
    //real_value = stored_value/2^15
    return (float)x / Q15_SCALE;
}

q15_t q15_add(q15_t a, q15_t b) {
    int32_t tmp = (int32_t)a + (int32_t)b; //32-bit conversion

    if (tmp > 32767)  tmp = 32767;
    if (tmp < -32768) tmp = -32768;

    return (q15_t)tmp;
}

q15_t q15_mul(q15_t a, q15_t b) {
    int32_t tmp = (int32_t)a * (int32_t)b;   // 32-bit intermediate
    tmp >>= Q15_FRACTIONAL_BITS;             // scale back down

    if (tmp > 32767)  tmp = 32767;
    if (tmp < -32768) tmp = -32768;

    return (q15_t)tmp;
}

q15_t q15_sub(q15_t a, q15_t b) {
    int32_t tmp = (int32_t)a - (int32_t)b; //32-bit conversion

    if (tmp > 32767)  tmp = 32767;
    if (tmp < -32768) tmp = -32768;

    return (q15_t)tmp;
}

q15_t q15_div(q15_t a, q15_t b) {
    // Handle divide-by-zero
    if (b == 0) {
        return (a >= 0) ? 0x7FFF : (q15_t)0x8000;
    }

    // Promote to 32-bit and scale
    int32_t dividend = ((int32_t)a << 15);
    int32_t result = dividend / b;

    // Saturate to Q15 range
    if (result > 32767) result = 32767;
    if (result < -32768) result = -32768;

    return (q15_t)result;
}