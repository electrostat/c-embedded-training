#include "json_stream_tokenizer.h"

void json_stream_tokenizer_init(json_stream_tokenizer_t *t)
{
    t->buf = NULL;
    t->len = 0;
    t->pos = 0;

    t->end_of_stream = false;

    t->partial_len = 0;
    t->in_string = false;
    t->in_escape = false;
}

void json_stream_tokenizer_feed(json_stream_tokenizer_t *t,
                                const char *data,
                                size_t len)
{
    t->buf = data;
    t->len = len;
    t->pos = 0;
}

json_stream_token_result_t
json_stream_tokenizer_next(json_stream_tokenizer_t *t)
{
    json_stream_token_result_t r = {0};
    r.status = JSON_STREAM_TOKEN_NEED_MORE;
    return r;
}

void json_stream_tokenizer_end(json_stream_tokenizer_t *t)
{
    t->end_of_stream = true;
}
