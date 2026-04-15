#include "json_stream_tokenizer.h"

void json_stream_tokenizer_init(json_stream_tokenizer_t *t) {
    t->buf = NULL;
    t->len = 0;
    t->pos = 0;

    t->end_of_stream = false;

    t->partial_len = 0;
    t->in_string = false;
    t->in_escape = false;
}

void json_stream_tokenizer_feed(json_stream_tokenizer_t *t, const char *data, size_t len) {
    t->buf = data;
    t->len = len;
    t->pos = 0;
}

static json_stream_token_result_t start_string_token(json_stream_tokenizer_t *t) {
    json_stream_token_result_t r = {0};

    /* consume opening quote */
    t->pos++;
    t->in_string = true;
    t->in_escape = false;
    t->using_partial = false;
    t->partial_len = 0;

    /* Try to parse entirely from this chunk */
    size_t start = t->pos;

    while (t->pos < t->len) {
        char c = t->buf[t->pos];

        if (t->in_escape) {
            /* accept this char as escape payload */
            t->in_escape = false;
            t->pos++;
            continue;
        }

        if (c == '\\') {
            t->in_escape = true;
            t->pos++;
            continue;
        }

        if (c == '"') {
            /* closing quote found */
            size_t end = t->pos;
            t->pos++;          /* consume closing quote */
            t->in_string = false;

            r.token.type = JSON_TOKEN_STRING;
            r.token.start = &t->buf[start];
            r.token.length = end - start;
            r.status = JSON_STREAM_TOKEN_OK;
            return r;
        }

        t->pos++;
    }

    /* Ran out of chunk before closing quote */
    size_t chunk_len = t->pos - start;
    if (chunk_len > sizeof(t->partial)) {
        r.status = JSON_STREAM_TOKEN_ERROR;
        return r;
    }

    memcpy(t->partial, &t->buf[start], chunk_len);
    t->partial_len = chunk_len;
    t->using_partial = true;

    r.status = JSON_STREAM_TOKEN_NEED_MORE;
    return r;
}

static json_stream_token_result_t resume_string_token(json_stream_tokenizer_t *t) {
    json_stream_token_result_t r = {0};

    while (t->pos < t->len) {
        char c = t->buf[t->pos];

        if (t->in_escape) {
            t->in_escape = false;
            if (t->using_partial) {
                if (t->partial_len >= sizeof(t->partial)) {
                    r.status = JSON_STREAM_TOKEN_ERROR;
                    return r;
                }
                t->partial[t->partial_len++] = c;
            }
            t->pos++;
            continue;
        }

        if (c == '\\') {
            t->in_escape = true;
            if (t->using_partial) {
                if (t->partial_len >= sizeof(t->partial)) {
                    r.status = JSON_STREAM_TOKEN_ERROR;
                    return r;
                }
                t->partial[t->partial_len++] = c;
            }
            t->pos++;
            continue;
        }

        if (c == '"') {
            /* closing quote */
            t->pos++;
            t->in_string = false;

            r.token.type = JSON_TOKEN_STRING;

            if (t->using_partial) {
                r.token.start = t->partial;
                r.token.length = t->partial_len;
            } else {
                /* rare case: started in previous chunk but never used partial */
                r.status = JSON_STREAM_TOKEN_ERROR;
                return r;
            }

            r.status = JSON_STREAM_TOKEN_OK;
            return r;
        }

        /* regular char */
        if (t->using_partial) {
            if (t->partial_len >= sizeof(t->partial)) {
                r.status = JSON_STREAM_TOKEN_ERROR;
                return r;
            }
            t->partial[t->partial_len++] = c;
        }
        t->pos++;
    }

    /* Still no closing quote */
    r.status = JSON_STREAM_TOKEN_NEED_MORE;
    return r;
}

static bool is_number_char(char c) {
    return (c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-';
}

static json_stream_token_result_t start_number_token(json_stream_tokenizer_t *t) {
    json_stream_token_result_t r = {0};

    t->in_number = true;
    t->using_partial = false;
    t->partial_len = 0;

    size_t start = t->pos;

    while (t->pos < t->len && is_number_char(t->buf[t->pos])) {
        t->pos++;
    }

    if (t->pos < t->len) {
        /* token ends in this chunk */
        t->in_number = false;

        r.token.type = JSON_TOKEN_NUMBER;
        r.token.start = &t->buf[start];
        r.token.length = t->pos - start;
        r.status = JSON_STREAM_TOKEN_OK;
        return r;
    }

    /* ran out of chunk mid-number */
    size_t chunk_len = t->pos - start;
    if (chunk_len > sizeof(t->partial)) {
        r.status = JSON_STREAM_TOKEN_ERROR;
        return r;
    }

    memcpy(t->partial, &t->buf[start], chunk_len);
    t->partial_len = chunk_len;
    t->using_partial = true;

    r.status = JSON_STREAM_TOKEN_NEED_MORE;
    return r;
}

static json_stream_token_result_t resume_number_token(json_stream_tokenizer_t *t) {
    json_stream_token_result_t r = {0};

    while (t->pos < t->len && is_number_char(t->buf[t->pos])) {
        if (t->partial_len >= sizeof(t->partial)) {
            r.status = JSON_STREAM_TOKEN_ERROR;
            return r;
        }
        t->partial[t->partial_len++] = t->buf[t->pos];
        t->pos++;
    }

    if (t->pos == t->len && !t->end_of_stream) {
        r.status = JSON_STREAM_TOKEN_NEED_MORE;
        return r;
    }

    t->in_number = false;

    r.token.type = JSON_TOKEN_NUMBER;
    r.token.start = t->partial;
    r.token.length = t->partial_len;
    r.status = JSON_STREAM_TOKEN_OK;
    return r;
}

static bool is_punctuation(char c) {
    return c == '{' || c == '}' ||
           c == '[' || c == ']' ||
           c == ':' || c == ',';
}

static json_stream_token_result_t emit_punctuation_token(json_stream_tokenizer_t *t, char c) {
    json_stream_token_result_t r = {0};

    if(c == '{') {
        r.token.type = JSON_TOKEN_OBJECT_BEGIN;
    } else if (c == '}'){
        r.token.type = JSON_TOKEN_OBJECT_END;
    } else if (c == '['){
        r.token.type = JSON_TOKEN_ARRAY_BEGIN;
    } else if (c == ']'){
        r.token.type = JSON_TOKEN_ARRAY_END;
    } else if (c == ':'){
        r.token.type = JSON_TOKEN_COLON;
    } else if (c == ','){
        r.token.type = JSON_TOKEN_COMMA;
    }
    

    r.token.start  = &t->buf[t->pos];
    r.token.length = 1;
    r.status       = JSON_STREAM_TOKEN_OK;

    t->pos++;  // consume

    return r;
}

//literal code
static json_stream_token_result_t start_literal_token(json_stream_tokenizer_t *t) {
    json_stream_token_result_t r = {0};

    char c = t->buf[t->pos++];
    size_t start = t->pos;

    // Literal: true
    if (c == 't') {
        if (t->pos + 2 < t->len &&
            t->buf[t->pos] == 'r' &&
            t->buf[t->pos + 1] == 'u' &&
            t->buf[t->pos + 2] == 'e') {

            t->pos += 3; // consumed r,u,e
            r.token.type = JSON_TOKEN_TRUE;
            r.token.start = &t->buf[t->pos - 4]; // points to 't'
            r.token.length = 4;
            return r;
        }
        r.token.type = JSON_TOKEN_ERROR;
        return r;
    }

    // Literal: false
    if (c == 'f') {
        if (t->pos + 3 < t->len &&
            t->buf[t->pos] == 'a' &&
            t->buf[t->pos + 1] == 'l' &&
            t->buf[t->pos + 2] == 's' &&
            t->buf[t->pos + 3] == 'e') {

            t->pos += 4; // consumed a,l,s,e
            r.token.type = JSON_TOKEN_FALSE;
            r.token.start = &t->buf[t->pos - 5];
            r.token.length = 5;
            return r;
        }
        r.token.type = JSON_TOKEN_ERROR;
        return r;
    }

    // Literal: null
    if (c == 'n') {
        if (t->pos + 2 < t->len &&
            t->buf[t->pos] == 'u' &&
            t->buf[t->pos + 1] == 'l' &&
            t->buf[t->pos + 2] == 'l') {

            t->pos += 3; // consumed u,l,l
            r.token.type = JSON_TOKEN_NULL;
            r.token.start = &t->buf[t->pos - 4];
            r.token.length = 4;
            return r;
        }
        r.token.type = JSON_TOKEN_ERROR;
        return r;
    }

    /* Ran out of chunk before closing quote */
    size_t chunk_len = t->pos - start;
    if (chunk_len > sizeof(t->partial)) {
        r.status = JSON_STREAM_TOKEN_ERROR;
        return r;
    }

    memcpy(t->partial, &t->buf[start], chunk_len);
    t->partial_len = chunk_len;
    t->using_partial = true;

    r.status = JSON_STREAM_TOKEN_NEED_MORE;
    return r;
}

static json_stream_token_result_t resume_literal_token(json_stream_tokenizer_t *t) {
    json_stream_token_result_t r = {0};

    char c = t->buf[t->pos++];
    size_t start = t->pos;

    // Literal: true
    if (c == 't') {
        if (t->pos + 2 < t->len &&
            t->buf[t->pos] == 'r' &&
            t->buf[t->pos + 1] == 'u' &&
            t->buf[t->pos + 2] == 'e') {

            t->pos += 3; // consumed r,u,e
            r.token.type = JSON_TOKEN_TRUE;
            r.token.start = &t->buf[t->pos - 4]; // points to 't'
            r.token.length = 4;
            return r;
        }
        r.token.type = JSON_TOKEN_ERROR;
        return r;
    }

    // Literal: false
    if (c == 'f') {
        if (t->pos + 3 < t->len &&
            t->buf[t->pos] == 'a' &&
            t->buf[t->pos + 1] == 'l' &&
            t->buf[t->pos + 2] == 's' &&
            t->buf[t->pos + 3] == 'e') {

            t->pos += 4; // consumed a,l,s,e
            r.token.type = JSON_TOKEN_FALSE;
            r.token.start = &t->buf[t->pos - 5];
            r.token.length = 5;
            return r;
        }
        r.token.type = JSON_TOKEN_ERROR;
        return r;
    }

    // Literal: null
    if (c == 'n') {
        if (t->pos + 2 < t->len &&
            t->buf[t->pos] == 'u' &&
            t->buf[t->pos + 1] == 'l' &&
            t->buf[t->pos + 2] == 'l') {

            t->pos += 3; // consumed u,l,l
            r.token.type = JSON_TOKEN_NULL;
            r.token.start = &t->buf[t->pos - 4];
            r.token.length = 4;
            return r;
        }
        r.token.type = JSON_TOKEN_ERROR;
        return r;
    }

    /* Ran out of chunk before closing quote */
    size_t chunk_len = t->pos - start;
    if (chunk_len > sizeof(t->partial)) {
        r.status = JSON_STREAM_TOKEN_ERROR;
        return r;
    }

    /* Still no closing quote */
    r.status = JSON_STREAM_TOKEN_NEED_MORE;
    return r;
}

json_stream_token_result_t json_stream_tokenizer_next(json_stream_tokenizer_t *t) {
    json_stream_token_result_t r = {0};

    //in the middle of a token, resume it
    if (t->in_string || t->in_number || t->in_literal) {
        return resume_in_progress_token(t);
    }

    //skip whitespace
    while (t->pos < t->len && is_json_whitespace((unsigned char)t->buf[t->pos])) {
        t->pos++;
    }

    //out of bytes
    if (t->pos >= t->len) {
        if (t->end_of_stream) {
            r.status = JSON_STREAM_TOKEN_EOF;
        } else {
            r.status = JSON_STREAM_TOKEN_NEED_MORE;
        }
        return r;
    }

    //determine token
    char c = t->buf[t->pos];

    if (c == '"') {
        return start_string_token(t);
    } else if (c == '-' || is_digit(c)) {
        return start_number_token(t);
    } else if (c == 't' || c == 'f' || c == 'n') {
        return start_literal_token(t);
    } else if (is_punctuation(c)) {
        return emit_punctuation_token(t, c);
    } else {
        r.status = JSON_STREAM_TOKEN_ERROR;
        return r;
    }
}

static json_stream_token_result_t resume_in_progress_token(json_stream_tokenizer_t *t) {
    if (t->in_string) {
        return resume_string_token(t);
    } else if (t->in_number) {
        return resume_number_token(t);
    } else if (t->in_literal) {
        return resume_literal_token(t);
    }

    json_stream_token_result_t r = {0};
    r.status = JSON_STREAM_TOKEN_ERROR;
    return r;
}

void json_stream_tokenizer_end(json_stream_tokenizer_t *t) {
    t->end_of_stream = true;
}
