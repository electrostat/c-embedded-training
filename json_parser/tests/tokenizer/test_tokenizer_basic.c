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

int main(void) {
    single_char_token_tests();

    //test string
    const char *string = "\"hello\"";
    string_test(string);

    return 0;
}