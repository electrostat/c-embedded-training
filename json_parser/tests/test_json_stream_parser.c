#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "json_stream_parser.h"

// Minimal assertion helpers
#define ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "ASSERT FAILED: %s (line %d)\n", #cond, __LINE__); \
        exit(1); \
    } \
} while (0)

#define ASSERT_STREQ(a,b) do { \
    if (strcmp((a),(b)) != 0) { \
        fprintf(stderr, "ASSERT FAILED: \"%s\" != \"%s\" (line %d)\n", (a),(b), __LINE__); \
        exit(1); \
    } \
} while (0)


// Logging test context + callbacks
typedef struct {
    char log[2048];
    size_t pos;
} test_ctx_t;

static void log_event(test_ctx_t *ctx, const char *msg) {
    size_t n = strlen(msg);
    memcpy(&ctx->log[ctx->pos], msg, n);
    ctx->pos += n;
    ctx->log[ctx->pos++] = '\n';
}

static void on_object_begin(void *u) { log_event(u, "obj_begin"); }
static void on_object_end(void *u)   { log_event(u, "obj_end"); }
static void on_array_begin(void *u)  { log_event(u, "arr_begin"); }
static void on_array_end(void *u)    { log_event(u, "arr_end"); }

static void on_key(void *u, const char *s, size_t n) {
    char buf[256];
    snprintf(buf, sizeof(buf), "key:%.*s", (int)n, s);
    log_event(u, buf);
}

static void on_string(void *u, const char *s, size_t n) {
    char buf[256];
    snprintf(buf, sizeof(buf), "str:%.*s", (int)n, s);
    log_event(u, buf);
}

static void on_number(void *u, const char *s, size_t n) {
    char buf[256];
    snprintf(buf, sizeof(buf), "num:%.*s", (int)n, s);
    log_event(u, buf);
}

static void on_bool(void *u, bool v) {
    log_event(u, v ? "bool:true" : "bool:false");
}

static void on_null(void *u) {
    log_event(u, "null");
}


// Helper: run a streaming parse with two chunks
static void run_two_chunk_test(const char *chunk1, const char *chunk2, const char *expected_log) {
    test_ctx_t ctx = {0};

    json_parser_callbacks_t cb = {
        .on_object_begin = on_object_begin,
        .on_object_end   = on_object_end,
        .on_array_begin  = on_array_begin,
        .on_array_end    = on_array_end,
        .on_key          = on_key,
        .on_string       = on_string,
        .on_number       = on_number,
        .on_bool         = on_bool,
        .on_null         = on_null
    };

    json_stream_parser_t p;
    json_stream_parser_init(&p, cb, &ctx);

    json_stream_parser_status_t st;

    st = json_stream_parser_feed(&p, chunk1, strlen(chunk1));
    ASSERT(st == JSON_STREAM_PARSER_NEED_MORE);

    st = json_stream_parser_feed(&p, chunk2, strlen(chunk2));
    ASSERT(st == JSON_STREAM_PARSER_DONE);

    ASSERT_STREQ(ctx.log, expected_log);
}


// TESTS
static void test_string_split_mid_content() {
    const char *chunk1 = "{\"key\":\"hel";
    const char *chunk2 = "lo\"}";

    const char *expected =
        "obj_begin\n"
        "key:key\n"
        "str:hello\n"
        "obj_end\n";

    run_two_chunk_test(chunk1, chunk2, expected);
}

static void test_string_split_escape() {
    const char *chunk1 = "{\"x\":\"he\\\"";
    const char *chunk2 = "llo\"}";

    const char *expected =
        "obj_begin\n"
        "key:x\n"
        "str:he\"llo\n"
        "obj_end\n";

    run_two_chunk_test(chunk1, chunk2, expected);
}

static void test_literal_split() {
    const char *chunk1 = "{\"a\":tru";
    const char *chunk2 = "e}";

    const char *expected =
        "obj_begin\n"
        "key:a\n"
        "bool:true\n"
        "obj_end\n";

    run_two_chunk_test(chunk1, chunk2, expected);
}

static void test_number_split() {
    const char *chunk1 = "{\"n\":12";
    const char *chunk2 = "34}";

    const char *expected =
        "obj_begin\n"
        "key:n\n"
        "num:1234\n"
        "obj_end\n";

    run_two_chunk_test(chunk1, chunk2, expected);
}

static void test_array_split() {
    const char *chunk1 = "[1,";
    const char *chunk2 = "2,3]";

    const char *expected =
        "arr_begin\n"
        "num:1\n"
        "num:2\n"
        "num:3\n"
        "arr_end\n";

    run_two_chunk_test(chunk1, chunk2, expected);
}


// MAIN
int main(void) {
    printf("Running streaming JSON tests...\n");

    test_string_split_mid_content();
    test_string_split_escape();
    test_literal_split();
    test_number_split();
    test_array_split();

    printf("All tests passed.\n");
    return 0;
}