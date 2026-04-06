#ifndef JSON_TOKEN_H
#define JSON_TOKEN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    JSON_TOKEN_OBJECT_BEGIN,   // {
    JSON_TOKEN_OBJECT_END,     // }
    JSON_TOKEN_ARRAY_BEGIN,    // [
    JSON_TOKEN_ARRAY_END,      // ]
    JSON_TOKEN_COLON,          // :
    JSON_TOKEN_COMMA,          // ,
    JSON_TOKEN_STRING,         // "abc"
    JSON_TOKEN_NUMBER,         // 123, -4.5, 1e10
    JSON_TOKEN_TRUE,           // true
    JSON_TOKEN_FALSE,          // false
    JSON_TOKEN_NULL,           // null
    JSON_TOKEN_EOF,
    JSON_TOKEN_ERROR
} json_token_type_t;

typedef struct {
    json_token_type_t type;
    const char *start;   // pointer to input buffer - no saving data/duplication
    size_t length;       // length of token
} json_token_t;

// Tokenizer states
typedef enum {
    JSON_STATE_START,
    JSON_STATE_STRING,
    JSON_STATE_STRING_ESCAPE,
    JSON_STATE_NUMBER,
    JSON_STATE_TRUE,
    JSON_STATE_FALSE,
    JSON_STATE_NULL,
    JSON_STATE_ERROR
} json_tokenizer_state_t;

// Tokenizer object
typedef struct {
    json_tokenizer_state_t state;
    const char *input;
    size_t pos;
    size_t length;
} json_tokenizer_t;

// API
void json_tokenizer_init(json_tokenizer_t *t, const char *input, size_t length);
json_token_t json_tokenizer_next(json_tokenizer_t *t);

#endif
