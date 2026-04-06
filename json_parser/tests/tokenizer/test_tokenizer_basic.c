#include "json_token.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("[FAIL] %s\n", msg); \
            assert(cond); \
        } else { \
            printf("[PASS] %s\n", msg); \
        } \
    } while (0)

void string_test(const char *string) {
    json_tokenizer_t t;
    size_t raw_len = strlen(string);
    json_tokenizer_init(&t, string, raw_len);

    json_token_t tok = json_tokenizer_next(&t);
    TEST_ASSERT(tok.type == JSON_TOKEN_STRING, "string token recognized");
    
    size_t expected_content_len = raw_len - 2; // remove quotes
    TEST_ASSERT(tok.length == expected_content_len, "string length is correct");
    TEST_ASSERT(strncmp(tok.start, string + 1, expected_content_len) == 0, "string content matches");
}

void empty_string_test(void) {
    const char *json = "\"\"";
    json_tokenizer_t t;
    json_tokenizer_init(&t, json, strlen(json));

    json_token_t tok = json_tokenizer_next(&t);

    TEST_ASSERT(tok.type == JSON_TOKEN_STRING, "empty string recognized");
    TEST_ASSERT(tok.length == 0, "empty string length is zero");
}

void escaped_quote_test(void) {
    const char *json = "\"he\\\"llo\"";
    size_t raw_len = strlen(json);

    json_tokenizer_t t;
    json_tokenizer_init(&t, json, raw_len);

    json_token_t tok = json_tokenizer_next(&t);

    TEST_ASSERT(tok.type == JSON_TOKEN_STRING, "escaped quote recognized");

    size_t expected_len = raw_len - 2; // still raw minus quotes
    TEST_ASSERT(tok.length == expected_len, "escaped quote length matches raw content length");

    // Compare content: tok.start points to h
    TEST_ASSERT(strncmp(tok.start, json + 1, expected_len) == 0, "escaped quote content matches raw buffer");
}

void escaped_backslash_test(void) {
    const char *json = "\"a\\\\b\"";  // JSON: "a\\b", C: "a\\\\b"
    size_t raw_len = strlen(json);

    json_tokenizer_t t;
    json_tokenizer_init(&t, json, raw_len);

    json_token_t tok = json_tokenizer_next(&t);

    TEST_ASSERT(tok.type == JSON_TOKEN_STRING, "escaped backslash recognized");

    size_t expected_len = raw_len - 2;
    TEST_ASSERT(tok.length == expected_len, "escaped backslash length matches");

    TEST_ASSERT(strncmp(tok.start, json + 1, expected_len) == 0,"escaped backslash content matches raw buffer");
}

void single_char_token_tests(){
    const char *json = "{}";
    json_tokenizer_t t;
    json_tokenizer_init(&t, json, 2);

    json_token_t tok1 = json_tokenizer_next(&t);
    TEST_ASSERT(tok1.type == JSON_TOKEN_OBJECT_BEGIN, "object begin token");

    json_token_t tok2 = json_tokenizer_next(&t);
    TEST_ASSERT(tok2.type == JSON_TOKEN_OBJECT_END, "object end token");

    json_token_t tok3 = json_tokenizer_next(&t);
    TEST_ASSERT(tok3.type == JSON_TOKEN_EOF, "EOF after {}");
}

void literal_true_test(void) {
    const char *json = "true";
    json_tokenizer_t t;
    json_tokenizer_init(&t, json, strlen(json));

    json_token_t tok = json_tokenizer_next(&t);
    TEST_ASSERT(tok.type == JSON_TOKEN_TRUE, "true literal recognized");
}

void literal_false_test(void) {
    const char *json = "false";
    json_tokenizer_t t;
    json_tokenizer_init(&t, json, strlen(json));

    json_token_t tok = json_tokenizer_next(&t);
    TEST_ASSERT(tok.type == JSON_TOKEN_FALSE, "false literal recognized");
}

void literal_null_test(void) {
    const char *json = "null";
    json_tokenizer_t t;
    json_tokenizer_init(&t, json, strlen(json));

    json_token_t tok = json_tokenizer_next(&t);
    TEST_ASSERT(tok.type == JSON_TOKEN_NULL, "null literal recognized");
}

void literal_error_test(void) {
    const char *json1 = "tru";
    const char *json2 = "falze";
    const char *json3 = "nulx";

    json_tokenizer_t t;

    json_tokenizer_init(&t, json1, strlen(json1));
    TEST_ASSERT(json_tokenizer_next(&t).type == JSON_TOKEN_ERROR, "invalid true literal errors");

    json_tokenizer_init(&t, json2, strlen(json2));
    TEST_ASSERT(json_tokenizer_next(&t).type == JSON_TOKEN_ERROR, "invalid false literal errors");

    json_tokenizer_init(&t, json3, strlen(json3));
    TEST_ASSERT(json_tokenizer_next(&t).type == JSON_TOKEN_ERROR, "invalid null literal errors");
}

int main(void) {
    single_char_token_tests();

    // String tests
    string_test("\"hello\"");
    empty_string_test();
    escaped_quote_test();
    escaped_backslash_test();

    //literal tests
    literal_true_test();
    literal_false_test();
    literal_null_test();
    literal_error_test();

    return 0;
}