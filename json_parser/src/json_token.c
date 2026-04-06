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

    // Strings
    if (c == '"') {
        size_t start_pos = t->pos; // first character after the quote
        t->state = JSON_STATE_STRING;

        while (t->pos < t->length) {
            char ch = t->input[t->pos++];

            if (t->state == JSON_STATE_STRING) {

                //End of String
                if (ch == '"') {
                    tok.type = JSON_TOKEN_STRING;
                    tok.start = &t->input[start_pos];
                    tok.length = (t->pos - 1) - start_pos; // exclude closing quote
                    return tok;
                }

                if (ch == '\\') {
                    t->state = JSON_STATE_STRING_ESCAPE;
                    continue;
                }

                // Control characters (0x00–0x1F) are illegal in JSON strings
                if ((unsigned char)ch < 0x20) {
                    tok.type = JSON_TOKEN_ERROR;
                    return tok;
                }

                // Otherwise: normal character, continue scanning
            } else if (t->state == JSON_STATE_STRING_ESCAPE) {

                switch (ch) {
                    case '"':
                    case '\\':
                    case '/':
                    case 'b':
                    case 'f':
                    case 'n':
                    case 'r':
                    case 't':
                        // Valid simple escape
                        t->state = JSON_STATE_STRING;
                        continue;

                    case 'u':
                        // Expect 4 hex digits
                        if (t->pos + 4 > t->length) {
                            tok.type = JSON_TOKEN_ERROR;
                            return tok;
                        }
                        for (int i = 0; i < 4; i++) {
                            char h = t->input[t->pos++];
                            if (!((h >= '0' && h <= '9') ||
                                (h >= 'A' && h <= 'F') ||
                                (h >= 'a' && h <= 'f'))) {
                                tok.type = JSON_TOKEN_ERROR;
                                return tok;
                            }
                        }
                        t->state = JSON_STATE_STRING;
                        continue;

                    default:
                        tok.type = JSON_TOKEN_ERROR;
                        return tok;
                }
            }
        }

        // Unterminated string
        tok.type = JSON_TOKEN_ERROR;
        return tok;
    }

    // Literal: true
    if (c == 't') {
        if (t->pos + 2 < t->length &&
            t->input[t->pos] == 'r' &&
            t->input[t->pos + 1] == 'u' &&
            t->input[t->pos + 2] == 'e') {

            t->pos += 3; // consumed r,u,e
            tok.type = JSON_TOKEN_TRUE;
            tok.start = &t->input[t->pos - 4]; // points to 't'
            tok.length = 4;
            return tok;
        }
        tok.type = JSON_TOKEN_ERROR;
        return tok;
    }

    // Literal: false
    if (c == 'f') {
        if (t->pos + 3 < t->length &&
            t->input[t->pos] == 'a' &&
            t->input[t->pos + 1] == 'l' &&
            t->input[t->pos + 2] == 's' &&
            t->input[t->pos + 3] == 'e') {

            t->pos += 4; // consumed a,l,s,e
            tok.type = JSON_TOKEN_FALSE;
            tok.start = &t->input[t->pos - 5];
            tok.length = 5;
            return tok;
        }
        tok.type = JSON_TOKEN_ERROR;
        return tok;
    }

    // Literal: null
    if (c == 'n') {
        if (t->pos + 2 < t->length &&
            t->input[t->pos] == 'u' &&
            t->input[t->pos + 1] == 'l' &&
            t->input[t->pos + 2] == 'l') {

            t->pos += 3; // consumed u,l,l
            tok.type = JSON_TOKEN_NULL;
            tok.start = &t->input[t->pos - 4];
            tok.length = 4;
            return tok;
        }
        tok.type = JSON_TOKEN_ERROR;
        return tok;
    }

    // Numbers
    if (c == '-' || (c >= '0' && c <= '9')) {
        size_t start = t->pos - 1; // include the first char we consumed

        // Optional minus
        if (c == '-') {
            if (t->pos >= t->length) {
                tok.type = JSON_TOKEN_ERROR;
                return tok;
            }
            c = t->input[t->pos++];
            if (!(c >= '0' && c <= '9')) {
                tok.type = JSON_TOKEN_ERROR;
                return tok;
            }
        }

        // Integer part
        if (c == '0') {
            // Leading zero must not be followed by digits
            if (t->pos < t->length && (t->input[t->pos] >= '0' && t->input[t->pos] <= '9')) {
                tok.type = JSON_TOKEN_ERROR;
                return tok;
            }
        } else {
            while (t->pos < t->length) {
                char d = t->input[t->pos];
                if (d >= '0' && d <= '9') {
                    t->pos++;
                } else {
                    break;
                }
            }
        }

        // Fraction
        if (t->pos < t->length && t->input[t->pos] == '.') {
            t->pos++;
            if (t->pos >= t->length || !(t->input[t->pos] >= '0' && t->input[t->pos] <= '9')) {
                tok.type = JSON_TOKEN_ERROR;
                return tok;
            }
            while (t->pos < t->length && (t->input[t->pos] >= '0' && t->input[t->pos] <= '9')) {
                t->pos++;
            }
        }

        // Exponent
        if (t->pos < t->length && (t->input[t->pos] == 'e' || t->input[t->pos] == 'E')) {
            t->pos++;
            if (t->pos < t->length && (t->input[t->pos] == '+' || t->input[t->pos] == '-')) {
                t->pos++;
            }
            if (t->pos >= t->length || !(t->input[t->pos] >= '0' && t->input[t->pos] <= '9')) {
                tok.type = JSON_TOKEN_ERROR;
                return tok;
            }
            while (t->pos < t->length && (t->input[t->pos] >= '0' && t->input[t->pos] <= '9')) {
                t->pos++;
            }
        }

        tok.type = JSON_TOKEN_NUMBER;
        tok.start = &t->input[start];
        tok.length = t->pos - start;
        return tok;
    }

    //single character tokens
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