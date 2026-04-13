#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "json_token.h"

typedef struct {
    void (*on_object_begin)(void *ctx);
    void (*on_object_end)(void *ctx);

    void (*on_array_begin)(void *ctx);
    void (*on_array_end)(void *ctx);

    void (*on_key)(void *ctx, const char *start, size_t len);

    void (*on_string)(void *ctx, const char *start, size_t len);
    void (*on_number)(void *ctx, const char *start, size_t len);

    void (*on_bool)(void *ctx, bool value);
    void (*on_null)(void *ctx);
} json_parser_callbacks_t;

typedef enum {
    JSON_CONTEXT_OBJECT,
    JSON_CONTEXT_ARRAY
} json_context_t;

#define JSON_MAX_DEPTH 32

typedef struct {
    json_context_t stack[JSON_MAX_DEPTH];
    int depth;
} json_parser_stack_t;

typedef enum {
    JSON_PARSER_EXPECT_VALUE,
    JSON_PARSER_EXPECT_KEY,
    JSON_PARSER_EXPECT_COLON,
    JSON_PARSER_EXPECT_COMMA_OR_END
} json_parser_state_t;

typedef struct {
    json_tokenizer_t tokenizer;
    json_parser_callbacks_t cb;
    void *user_ctx;

    json_parser_stack_t stack;
    json_parser_state_t state;

    bool error;
} json_parser_t;

// API
void json_parser_init(json_parser_t *p, const char *input, size_t length, json_parser_callbacks_t cb,
                      void *user_ctx);
void json_parser_parse(json_parser_t *p);
bool json_parse(const char *input, size_t length, json_parser_callbacks_t cb, void *user_ctx);

#endif