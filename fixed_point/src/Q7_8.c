#include "Q7_8.h"

#define Q7_8_SCALE 256.0f
#define Q7_8_MAX   127.996f
#define Q7_8_MIN  -128.0f

q7_8_t q7_8_from_float(float x){
    if (x > Q7_8_MAX) x = Q7_8_MAX;
    if (x < Q7_8_MIN) x = Q7_8_MIN;
    return (q7_8_t)(x * Q7_8_SCALE);
}

float q7_8_to_float(q7_8_t x){
    return (float)x / 256.0f;
}

q7_8_t q7_8_add(q7_8_t a, q7_8_t b){
    int32_t r = (int32_t)a + (int32_t)b;
    if (r > INT16_MAX) r = INT16_MAX;
    if (r < INT16_MIN) r = INT16_MIN;
    return (q7_8_t)r;
}

q7_8_t q7_8_sub(q7_8_t a, q7_8_t b){
    int32_t r = (int32_t)a - (int32_t)b;
    if (r > INT16_MAX) r = INT16_MAX;
    if (r < INT16_MIN) r = INT16_MIN;
    return (q7_8_t)r;
}

q7_8_t q7_8_mul(q7_8_t a, q7_8_t b){
    int32_t r = (int32_t)a * (int32_t)b; // Q14.16
    r >>= 8; // back to Q7.8
    if (r > INT16_MAX) r = INT16_MAX;
    if (r < INT16_MIN) r = INT16_MIN;
    return (q7_8_t)r;
}

q7_8_t q7_8_div(q7_8_t a, q7_8_t b){
    if (b == 0) return (a >= 0) ? INT16_MAX : INT16_MIN;

    int32_t r = ((int32_t)a << 8) / b;
    if (r > INT16_MAX) r = INT16_MAX;
    if (r < INT16_MIN) r = INT16_MIN;
    return (q7_8_t)r;
}
