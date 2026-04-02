#include "json_token.h"

void json_tokenizer_init(json_tokenizer_t *t, const char *input, size_t length) {
    t->state = JSON_STATE_START;
    t->input = input;
    t->pos = 0;
    t->length = length;
}

json_token_t json_tokenizer_next(json_tokenizer_t *t) {
    json_token_t tok = { .type = JSON_TOKEN_EOF, .start = NULL, .length = 0 };

    // End of input
    if (t->pos >= t->length) {
        tok.type = JSON_TOKEN_EOF;
        return tok;
    }

    char c = t->input[t->pos++];

    switch (c) {
        case '{':
            tok.type = JSON_TOKEN_OBJECT_BEGIN;
            tok.start = &t->input[t->pos - 1];
            tok.length = 1;
            return tok;

        case '}':
            tok.type = JSON_TOKEN_OBJECT_END;
            tok.start = &t->input[t->pos - 1];
            tok.length = 1;
            return tok;

        case '[':
            tok.type = JSON_TOKEN_ARRAY_BEGIN;
            tok.start = &t->input[t->pos - 1];
            tok.length = 1;
            return tok;

        case ']':
            tok.type = JSON_TOKEN_ARRAY_END;
            tok.start = &t->input[t->pos - 1];
            tok.length = 1;
            return tok;

        case ':':
            tok.type = JSON_TOKEN_COLON;
            tok.start = &t->input[t->pos - 1];
            tok.length = 1;
            return tok;

        case ',':
            tok.type = JSON_TOKEN_COMMA;
            tok.start = &t->input[t->pos - 1];
            tok.length = 1;
            return tok;

        // Skip whitespace
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return json_tokenizer_next(t);

        default:
            tok.type = JSON_TOKEN_ERROR;
            tok.start = &t->input[t->pos - 1];
            tok.length = 1;
            return tok;
    }
}