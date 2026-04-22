#include "json_stream_parser.h"
#include "json_stream_tokenizer.h"

void json_stream_parser_init(json_stream_parser_t *p, json_parser_callbacks_t cb, void *user_ctx) {
    json_stream_tokenizer_init(&p->tokenizer);

    p->cb = cb;
    p->user_ctx = user_ctx;

    p->stack.depth = 0;
    p->state = JSON_PARSER_EXPECT_VALUE;

    p->error = false;
    p->done = false;
}


static json_stream_parser_status_t json_stream_parser_process_token(json_stream_parser_t *p, const json_token_t *tok) {
    switch (tok->type) {

        case JSON_TOKEN_OBJECT_BEGIN:
            handle_object_begin(p);
            break;

        case JSON_TOKEN_OBJECT_END:
            handle_object_end(p);
            break;

        case JSON_TOKEN_ARRAY_BEGIN:
            handle_array_begin(p);
            break;

        case JSON_TOKEN_ARRAY_END:
            handle_array_end(p);
            break;

        case JSON_TOKEN_STRING:
            handle_string(p, tok);
            break;

        case JSON_TOKEN_NUMBER:
            handle_number(p, tok);
            break;

        case JSON_TOKEN_TRUE:
        case JSON_TOKEN_FALSE:
            handle_bool(p, tok->type == JSON_TOKEN_TRUE);
            break;

        case JSON_TOKEN_NULL:
            handle_null(p);
            break;

        case JSON_TOKEN_COLON:
            handle_colon(p);
            break;

        case JSON_TOKEN_COMMA:
            handle_comma(p);
            break;

        default:
            p->error = true;
            return JSON_STREAM_PARSER_ERROR;
    }

    if (p->error) {
        return JSON_STREAM_PARSER_ERROR;
    }

    return JSON_STREAM_PARSER_OK;
}


json_stream_parser_status_t json_stream_parser_feed(json_stream_parser_t *p, const char *data, size_t len) {
    if (p->error) return JSON_STREAM_PARSER_ERROR;
    if (p->done)  return JSON_STREAM_PARSER_DONE;

    json_stream_tokenizer_feed(&p->tokenizer, data, len);

    for (;;) {
        json_stream_token_result_t tr =
            json_stream_tokenizer_next(&p->tokenizer);

        if (tr.status == JSON_STREAM_TOKEN_OK) {
            json_stream_parser_status_t ps =
                json_stream_parser_process_token(p, &tr.token);

            if (ps == JSON_STREAM_PARSER_ERROR) return ps;

            // Document complete?
            if (p->state == JSON_PARSER_EXPECT_COMMA_OR_END &&
                p->stack.depth == 0) {
                p->done = true;
                return JSON_STREAM_PARSER_DONE;
            }

            continue;
        }

        if (tr.status == JSON_STREAM_TOKEN_NEED_MORE) {
            return JSON_STREAM_PARSER_NEED_MORE;
        }

        if (tr.status == JSON_STREAM_TOKEN_ERROR) {
            p->error = true;
            return JSON_STREAM_PARSER_ERROR;
        }

        // EOF should not appear here
        return JSON_STREAM_PARSER_NEED_MORE;
    }
}


json_stream_parser_status_t json_stream_parser_finish(json_stream_parser_t *p) {
    if (p->error) return JSON_STREAM_PARSER_ERROR;
    if (p->done)  return JSON_STREAM_PARSER_DONE;

    json_stream_tokenizer_end(&p->tokenizer);

    for (;;) {
        json_stream_token_result_t tr =
            json_stream_tokenizer_next(&p->tokenizer);

        if (tr.status == JSON_STREAM_TOKEN_OK) {
            json_stream_parser_status_t ps =
                json_stream_parser_process_token(p, &tr.token);

            if (ps == JSON_STREAM_PARSER_ERROR) return ps;

            // Document complete?
            if (p->state == JSON_PARSER_EXPECT_COMMA_OR_END &&
                p->stack.depth == 0) {
                p->done = true;
                return JSON_STREAM_PARSER_DONE;
            }

            continue;
        }

        if (tr.status == JSON_STREAM_TOKEN_EOF) {
            // If we reach EOF but not done, it's an error
            if (!p->done) {
                p->error = true;
                return JSON_STREAM_PARSER_ERROR;
            }
            return JSON_STREAM_PARSER_DONE;
        }

        if (tr.status == JSON_STREAM_TOKEN_NEED_MORE) {
            // No more data is coming
            p->error = true;
            return JSON_STREAM_PARSER_ERROR;
        }

        // ERROR
        p->error = true;
        return JSON_STREAM_PARSER_ERROR;
    }
}