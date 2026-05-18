#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json_writer.h"

// Simple test buffer + output callback
typedef struct {
    char buf[4096];
    size_t len;
} test_buf_t;

static void test_output(void *ctx, const char *s, size_t len) {
    test_buf_t *tb = ctx;
    if (tb->len + len < sizeof(tb->buf)) {
        memcpy(tb->buf + tb->len, s, len);
        tb->len += len;
    }
}

// Minimal test harness
static int tests_run = 0;
static int tests_failed = 0;

#define TEST(name) \
    do { \
        tests_run++; \
        printf("TEST: %s\n", name); \
    } while (0)

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            tests_failed++; \
            printf("  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            return; \
        } \
    } while (0)

#define ASSERT_STR_EQ(actual, expected) \
    do { \
        if (strcmp(actual, expected) != 0) { \
            tests_failed++; \
            printf("  FAIL: %s:%d:\n    expected: \"%s\"\n    actual:   \"%s\"\n", \
                   __FILE__, __LINE__, expected, actual); \
            return; \
        } \
    } while (0)

#define ASSERT_ERR(w, errcode) \
    do { \
        if (json_writer_error(&(w)) != (errcode)) { \
            tests_failed++; \
            printf("  FAIL: %s:%d: expected error %d, got %d\n", \
                   __FILE__, __LINE__, errcode, json_writer_error(&(w))); \
            return; \
        } \
    } while (0)


// Test cases
static void test_simple_object(void) {
    TEST("simple object");

    test_buf_t tb = {0};
    json_writer_t w;
    json_writer_init(&w, test_output, &tb, 0, 2);

    json_writer_begin_object(&w);
    json_writer_key(&w, "name");
    json_writer_string(&w, "Anthony");
    json_writer_end_object(&w);

    tb.buf[tb.len] = '\0';
    ASSERT_STR_EQ(tb.buf, "{\"name\":\"Anthony\"}");
    ASSERT_ERR(w, JW_ERROR_NONE);
}

static void test_nested_object(void) {
    TEST("nested object");

    test_buf_t tb = {0};
    json_writer_t w;
    json_writer_init(&w, test_output, &tb, 0, 2);

    json_writer_begin_object(&w);
    json_writer_key(&w, "a");
    json_writer_begin_object(&w);
    json_writer_key(&w, "b");
    json_writer_number(&w, 42);
    json_writer_end_object(&w);
    json_writer_end_object(&w);

    tb.buf[tb.len] = '\0';
    ASSERT_STR_EQ(tb.buf, "{\"a\":{\"b\":42}}");
    ASSERT_ERR(w, JW_ERROR_NONE);
}

static void test_array_mixed(void) {
    TEST("array mixed types");

    test_buf_t tb = {0};
    json_writer_t w;
    json_writer_init(&w, test_output, &tb, 0, 2);

    json_writer_begin_array(&w);
    json_writer_string(&w, "x");
    json_writer_number(&w, 3.14);
    json_writer_bool(&w, 1);
    json_writer_null(&w);
    json_writer_end_array(&w);

    tb.buf[tb.len] = '\0';
    ASSERT_STR_EQ(tb.buf, "[\"x\",3.14,true,null]");
    ASSERT_ERR(w, JW_ERROR_NONE);
}

static void test_pretty_nested(void) {
    TEST("pretty nested");

    test_buf_t tb = {0};
    json_writer_t w;
    json_writer_init(&w, test_output, &tb, 1, 2);

    json_writer_begin_object(&w);
    json_writer_key(&w, "outer");
    json_writer_begin_object(&w);
    json_writer_key(&w, "inner");
    json_writer_begin_array(&w);
    json_writer_number(&w, 1);
    json_writer_number(&w, 2);
    json_writer_number(&w, 3);
    json_writer_end_array(&w);
    json_writer_end_object(&w);
    json_writer_end_object(&w);

    tb.buf[tb.len] = '\0';

    const char *expected =
        "{\n"
        "  \"outer\": {\n"
        "    \"inner\": [\n"
        "      1,\n"
        "      2,\n"
        "      3\n"
        "    ]\n"
        "  }\n"
        "}";

    ASSERT_STR_EQ(tb.buf, expected);
    ASSERT_ERR(w, JW_ERROR_NONE);
}

static void test_invalid_key_in_array(void) {
    TEST("invalid key in array");

    test_buf_t tb = {0};
    json_writer_t w;
    json_writer_init(&w, test_output, &tb, 0, 2);

    json_writer_begin_array(&w);
    json_writer_key(&w, "x"); // invalid

    ASSERT_ERR(w, JW_ERROR_INVALID_SCOPE);
}

static void test_invalid_value_when_key_expected(void) {
    TEST("invalid value when key expected");

    test_buf_t tb = {0};
    json_writer_t w;
    json_writer_init(&w, test_output, &tb, 0, 2);

    json_writer_begin_object(&w);
    json_writer_string(&w, "oops"); // invalid

    ASSERT_ERR(w, JW_ERROR_INVALID_STATE);
}

static void test_depth_overflow(void) {
    TEST("depth overflow");

    test_buf_t tb = {0};
    json_writer_t w;
    json_writer_init(&w, test_output, &tb, 0, 2);

    for (int i = 0; i < JSON_WRITER_MAX_DEPTH + 5; i++) {
        json_writer_begin_object(&w);
        if (json_writer_error(&w) != JW_ERROR_NONE)
            break;
    }

    ASSERT_ERR(w, JW_ERROR_DEPTH_OVERFLOW);
}

static void test_raw_injection(void) {
    TEST("raw injection");

    test_buf_t tb = {0};
    json_writer_t w;
    json_writer_init(&w, test_output, &tb, 0, 2);

    json_writer_begin_object(&w);
    json_writer_key(&w, "raw");
    json_writer_raw(&w, "{\"x\":1}");
    json_writer_end_object(&w);

    tb.buf[tb.len] = '\0';
    ASSERT_STR_EQ(tb.buf, "{\"raw\":{\"x\":1}}");
    ASSERT_ERR(w, JW_ERROR_NONE);
}

static void test_reset_clears_error(void) {
    TEST("reset clears error");

    test_buf_t tb = {0};
    json_writer_t w;
    json_writer_init(&w, test_output, &tb, 0, 2);

    json_writer_begin_array(&w);
    json_writer_key(&w, "x"); // invalid
    ASSERT_ERR(w, JW_ERROR_INVALID_SCOPE);

    json_writer_reset(&w);
    ASSERT_ERR(w, JW_ERROR_NONE);

    json_writer_begin_array(&w);
    json_writer_number(&w, 1);
    json_writer_end_array(&w);

    tb.buf[tb.len] = '\0';
    ASSERT_STR_EQ(tb.buf, "[1]");
}

// Main test runner
int main(void) {
    test_simple_object();
    test_nested_object();
    test_array_mixed();
    test_pretty_nested();
    test_invalid_key_in_array();
    test_invalid_value_when_key_expected();
    test_depth_overflow();
    test_raw_injection();
    test_reset_clears_error();

    printf("\nTests run: %d\n", tests_run);
    printf("Tests failed: %d\n", tests_failed);

    return tests_failed ? 1 : 0;
}