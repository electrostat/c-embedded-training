#include "json_parser.h"

void json_parser_init(json_parser_t *p, const char *input, size_t length, json_parser_callbacks_t cb,
                      void *user_ctx) {
    json_tokenizer_init(&p->tokenizer, input, length);
    p->cb = cb;
    p->user_ctx = user_ctx;

    p->stack.depth = 0;
    p->state = JSON_PARSER_EXPECT_VALUE;
    p->error = false;
}

void json_parser_parse(json_parser_t *p) {
    while (!p->error) {
        json_token_t tok = json_tokenizer_next(&p->tokenizer);

        if (tok.type == JSON_TOKEN_EOF) {
            return;
        }

        // Full logic comes next step
        // For now, just detect tokenizer errors
        if (tok.type == JSON_TOKEN_ERROR) {
            p->error = true;
            return;
        }
    }
}