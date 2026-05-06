#include "json_writer.h"
#include <stddef.h>

#define JSON_WRITER_MAX_DEPTH 32

//internal scope
typedef enum {
    JW_SCOPE_NONE = 0,
    JW_SCOPE_OBJECT,
    JW_SCOPE_ARRAY
} jw_scope_t;

//state machine
typedef enum {
    JW_STATE_START = 0,     // nothing written
    JW_STATE_KEY,           // expecting a key (inside object)
    JW_STATE_VALUE,         // expecting a value (after key or inside array)
    JW_STATE_AFTER_VALUE    // value written (expect comma or close object/array)
} jw_state_t;

struct json_writer {
    // Output plumbing
    json_writer_output_fn out;
    void *ctx;

    // Formatting configuration
    int pretty;
    int indent_width;
    int indent_level;

    // State machine
    jw_state_t state;

    // Scope stack for nested objects/arrays
    jw_scope_t scope_stack[JSON_WRITER_MAX_DEPTH];
    int depth;

    // Whether the next value should be preceded by a comma
    int need_comma;
};

void json_writier_init(json_writer_t *w,  json_writer_output_fn out, void *ctx, int pretty, int indent_width) {
    //output
    w->out = out;
    w->ctx = ctx;
   
    //formatting
    w->pretty = pretty;
    w->indent_width = indent_width;
    w-> indent_level = 0;
    
    //state
    w->state = JW_STATE_START;

    //scope stack
    w->depth = 0;
    w->scope_stack[0] = JW_SCOPE_NONE;

    //comma insertion
    w->need_comma = 0;
}

void json_writer_reset(json_writer_t *w) {
    w->indent_level = 0;
    w->state = JW_STATE_START;
    w->depth = 0;
    w->scope_stack[0] = JW_SCOPE_NONE;
    w->need_comma = 0;
}

//internal helpers
static void write_raw(json_writer_t *w, const char *data, size_t len) {
    if (w->out) {
        w->out(w->ctx, data, len);
    }
}

//string convenience wrapper
static void write_str(json_writer_t *w, const char *data) {
    write_raw(w, data, strlen(data));
}

static void write_indent(json_writer_t *w) {
    if (!w->pretty) {
        return;
    }

    write_raw(w, "\n", 1);

    int total = w->indent_level * w->indent_width;
    for (int i = 0; i < total; i++) {
        write_raw(w, " ", 1);
    }
}

static void maybe_write_comma(json_writer_t *w) {
    if (w->need_comma) {
        write_str(w, ",");
        if (w->pretty) {
            write_raw(w, " ", 1);
        }
        w->need_comma = 0;
    }
}

static void push_scope(json_writer_t *w, jw_scope_t scope) {
    if (w->depth < JSON_WRITER_MAX_DEPTH - 1) {
        w->depth++;
        w->scope_stack[w->depth] = scope;
    }
}

static jw_scope_t pop_scope(json_writer_t *w) {
    if (w->depth > 0) {
        jw_scope_t s = w->scope_stack[w->depth];
        w->depth--;
        return s;
    }
    return JW_SCOPE_NONE;
}

static jw_scope_t current_scope(const json_writer_t *w) {
    return w->scope_stack[w->depth];
}

//since these two happen so frequently together
static void write_value_prefix(json_writer_t *w) {
    maybe_write_comma(w);
    write_indent(w);
}

void json_writer_begin_object(json_writer_t *w) {
    write_value_prefix(w);

    write_str(w, "{");

    push_scope(w, JW_SCOPE_OBJECT);
    w->indent_level++;

    w->state = JW_STATE_KEY;
    w->need_comma = 0;
}

void json_writer_end_object(json_writer_t *w) {
    if (current_scope(w) != JW_SCOPE_OBJECT) {
        return;
    }

    w->indent_level--;
    if (w->pretty && w->state == JW_STATE_AFTER_VALUE) {
        write_indent(w);
    }

    write_str(w, "}");

    pop_scope(w);

    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;
}

void json_writer_begin_array(json_writer_t *w) {
    write_value_prefix(w);

    write_str(w, "[");

    push_scope(w, JW_SCOPE_ARRAY);
    w->indent_level++;

    w->state = JW_STATE_VALUE;
    w->need_comma = 0;
}

void json_writer_end_array(json_writer_t *w) {
    if (current_scope(w) != JW_SCOPE_ARRAY) {
        return; // or assert
    }

    w->indent_level--;
    if (w->pretty && w->state == JW_STATE_AFTER_VALUE) {
        write_indent(w);
    }

    write_str(w, "]");

    pop_scope(w);

    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;
}

void json_writer_key(json_writer_t *w, const char *key) {
    // Keys are only valid inside objects
    if (current_scope(w) != JW_SCOPE_OBJECT) {
        return;
    }

    // Keys are only valid when the writer expects a key
    if (w->state != JW_STATE_KEY) {
        return;
    }

    write_value_prefix(w);

    // Write the key as a JSON string
    write_raw(w, "\"", 1);
    write_str(w, key);
    write_raw(w, "\"", 1);

    // add colon
    write_raw(w, ":", 1);

    //pretty mode -> add a space after the colon
    if (w->pretty) {
        write_raw(w, " ", 1);
    }

    // After writing a key, the next thing must be a value
    w->state = JW_STATE_VALUE;
}

void json_writer_string(json_writer_t *w, const char value) {
    if (w->state != JW_STATE_VALUE) {
        return;
    }

    write_value_prefix(w);

    // Opening quote
    write_raw(w, "\"", 1);

    // Emit characters with escaping
    for (const char *p = value; *p; p++) {
        char c = *p;

        switch (c) {
            case '\"': write_str(w, "\\\""); break;
            case '\\': write_str(w, "\\\\"); break;
            case '\b': write_str(w, "\\b");  break;
            case '\f': write_str(w, "\\f");  break;
            case '\n': write_str(w, "\\n");  break;
            case '\r': write_str(w, "\\r");  break;
            case '\t': write_str(w, "\\t");  break;

            default:
                // Control characters must be escaped as \u00XX
                if ((unsigned char)c < 0x20) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    write_str(w, buf);
                } else {
                    write_raw(w, &c, 1);
                }
                break;
        }
    }

    // Closing quote
    write_raw(w, "\"", 1);

    // set AFTER_VALUE state
    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;
}

void json_writer_number(json_writer_t *w, double value) {
    if (w->state != JW_STATE_VALUE) {
        return;
    }

    write_value_prefix(w);

    // Format the number into a small local buffer
    char buf[32];

    // Deterministic formatting:
    int len = snprintf(buf, sizeof(buf), "%.17g", value);

    // Emit the number
    write_raw(w, buf, (size_t)len);

    // Update state machine
    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;
}

void json_writer_bool(json_writer_t *w, int value) {
    if (w->state != JW_STATE_VALUE) {
        return;
    }

    comma_indent_check(w);

    if (value) {
        write_str(w, "true");
    } else {
        write_str(w, "false");
    }

    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;
}

void json_writer_null(json_writer_t *w){
    if (w->state != JW_STATE_VALUE) {
        return;
    }

    comma_indent_check(w);
    write_str(w, "null");

    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;
}

void json_writer_raw(json_writer_t *w, const char *data) {
    if (w->state != JW_STATE_VALUE) {
        return;
    }

    comma_indent_check(w);
    write_str(w, data);

    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;
}



