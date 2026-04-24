#ifndef JSON_STREAM_PARSER_H
#define JSON_STREAM_PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include "json_parser.h"
#include "json_stream_tokenizer.h"

typedef enum {
    JSON_STREAM_PARSER_OK,
    JSON_STREAM_PARSER_NEED_MORE,
    JSON_STREAM_PARSER_ERROR,
    JSON_STREAM_PARSER_DONE
} json_stream_parser_status_t;

typedef struct json_stream_parser json_stream_parser_t;

struct json_stream_parser {
    json_stream_tokenizer_t tokenizer;
    json_parser_state_t state;
    json_parser_stack_t stack;
    bool error;
    bool done;
    json_parser_callbacks_t cb;
    void *user_ctx;
};

void json_stream_parser_init(json_stream_parser_t *p,
                             json_parser_callbacks_t cb,
                             void *user_ctx);

/* Feed a chunk of data into the parser */
json_stream_parser_status_t json_stream_parser_feed(json_stream_parser_t *p,
                        const char *data,
                        size_t len);

/* Signal that no more data coming */
json_stream_parser_status_t json_stream_parser_finish(json_stream_parser_t *p);

#endif