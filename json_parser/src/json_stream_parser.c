#include "json_stream_parser.h"
#include "json_stream_tokenizer.h"

struct json_stream_parser {
    json_stream_tokenizer_t tokenizer;
    json_parser_state_t state;
    json_parser_stack_t stack;
    bool error;
    bool done;
    json_parser_callbacks_t cb;
    void *user_ctx;
};