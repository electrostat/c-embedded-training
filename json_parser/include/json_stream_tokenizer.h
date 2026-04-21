#ifndef JSON_STREAM_TOKENIZER_H
#define JSON_STREAM_TOKENIZER_H
#include "json_token.h"
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    JSON_STREAM_TOKEN_OK,
    JSON_STREAM_TOKEN_NEED_MORE,
    JSON_STREAM_TOKEN_ERROR,
    JSON_STREAM_TOKEN_EOF
} json_stream_token_status_t;

typedef struct {
    json_token_t token;
    json_stream_token_status_t status;
} json_stream_token_result_t;

typedef struct json_stream_tokenizer {
    const char *buf;
    size_t len;
    size_t pos;

    bool end_of_stream;

    char partial[128];
    size_t partial_len;
    bool using_partial;

    bool in_string;
    bool in_escape;

    bool in_number;
    bool in_literal;
    char literal_buf[5];
    size_t literal_len;
} json_stream_tokenizer_t;

void json_stream_tokenizer_init(json_stream_tokenizer_t *t);

/* Feed new data into the tokenizer */
void json_stream_tokenizer_feed(json_stream_tokenizer_t *t,
                                const char *data,
                                size_t len);

/* Attempt to read the next token */
json_stream_token_result_t json_stream_tokenizer_next(json_stream_tokenizer_t *t);

/* Signal that no more data is coming */
void json_stream_tokenizer_end(json_stream_tokenizer_t *t);

#endif