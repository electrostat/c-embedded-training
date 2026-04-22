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

static void handle_object_begin(json_stream_parser_t *p) {
    if (p->state != JSON_PARSER_EXPECT_VALUE) {
        p->error = true;
        return;
    }

    if (p->stack.depth >= JSON_MAX_DEPTH) {
        p->error = true;
        return;
    }

    p->cb.on_object_begin(p->user_ctx);

    p->stack.stack[p->stack.depth++] = JSON_CONTEXT_OBJECT;
    p->state = JSON_PARSER_EXPECT_KEY;   // objects expect keys first
}

static void handle_object_end(json_stream_parser_t *p) {
    if (p->stack.depth == 0 ||
        p->stack.stack[p->stack.depth - 1] != JSON_CONTEXT_OBJECT) {
        p->error = true;
        return;
    }

    p->cb.on_object_end(p->user_ctx);

    p->stack.depth--;

    // After closing an object, expect comma or end of parent
    p->state = JSON_PARSER_EXPECT_COMMA_OR_END;
}

static void handle_array_begin(json_stream_parser_t *p) {
    if (p->state != JSON_PARSER_EXPECT_VALUE) {
        p->error = true;
        return;
    }

    if (p->stack.depth >= JSON_MAX_DEPTH) {
        p->error = true;
        return;
    }

    p->cb.on_array_begin(p->user_ctx);

    p->stack.stack[p->stack.depth++] = JSON_CONTEXT_ARRAY;
    p->state = JSON_PARSER_EXPECT_VALUE;   // arrays expect values first
}

static void handle_array_end(json_stream_parser_t *p) {
    if (p->stack.depth == 0 ||
        p->stack.stack[p->stack.depth - 1] != JSON_CONTEXT_ARRAY) {
        p->error = true;
        return;
    }

    p->cb.on_array_end(p->user_ctx);

    p->stack.depth--;

    p->state = JSON_PARSER_EXPECT_COMMA_OR_END;
}

static void handle_string(json_stream_parser_t *p, const json_token_t *tok) {
    if (p->state == JSON_PARSER_EXPECT_KEY) {
        p->cb.on_key(p->user_ctx, tok->start, tok->length);
        p->state = JSON_PARSER_EXPECT_COLON;
        return;
    }

    if (p->state == JSON_PARSER_EXPECT_VALUE) {
        p->cb.on_string(p->user_ctx, tok->start, tok->length);
        p->state = JSON_PARSER_EXPECT_COMMA_OR_END;
        return;
    }

    p->error = true;
}

static void handle_number(json_stream_parser_t *p, const json_token_t *tok) {
    if (p->state != JSON_PARSER_EXPECT_VALUE) {
        p->error = true;
        return;
    }

    p->cb.on_number(p->user_ctx, tok->start, tok->length);
    p->state = JSON_PARSER_EXPECT_COMMA_OR_END;
}

static void handle_bool(json_stream_parser_t *p, bool value) {
    if (p->state != JSON_PARSER_EXPECT_VALUE) {
        p->error = true;
        return;
    }

    p->cb.on_bool(p->user_ctx, value);
    p->state = JSON_PARSER_EXPECT_COMMA_OR_END;
}

static void handle_null(json_stream_parser_t *p) {
    if (p->state != JSON_PARSER_EXPECT_VALUE) {
        p->error = true;
        return;
    }

    p->cb.on_null(p->user_ctx);
    p->state = JSON_PARSER_EXPECT_COMMA_OR_END;
}

static void handle_colon(json_stream_parser_t *p) {
    if (p->state != JSON_PARSER_EXPECT_COLON) {
        p->error = true;
        return;
    }

    p->state = JSON_PARSER_EXPECT_VALUE;
}

static void handle_comma(json_stream_parser_t *p) {
    if (p->state != JSON_PARSER_EXPECT_COMMA_OR_END) {
        p->error = true;
        return;
    }

    json_context_t ctx = p->stack.stack[p->stack.depth - 1];

    if (ctx == JSON_CONTEXT_OBJECT) {
        p->state = JSON_PARSER_EXPECT_KEY;
    } else {
        p->state = JSON_PARSER_EXPECT_VALUE;
    }
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