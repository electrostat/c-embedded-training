#include "json_writer.h"
#include <string.h>
#include <stdio.h>

void json_writer_init(json_writer_t *w,  json_writer_output_fn out, void *ctx, int pretty, int indent_width) {
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

    //error
    w->error = JW_ERROR_NONE;
}

void json_writer_reset(json_writer_t *w) {
    w->indent_level = 0;
    w->state = JW_STATE_START;
    w->depth = 0;
    w->scope_stack[0] = JW_SCOPE_NONE;
    w->need_comma = 0;
    w->error = JW_ERROR_NONE;
}

static void set_error(json_writer_t *w, jw_error_t err) {
    if (w->error == JW_ERROR_NONE) {
        w->error = err;
    }
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

static void push_scope(json_writer_t *w, jw_scope_t scope) {
    fprintf(stderr, "[SCOPE] push: new_depth=%d scope=%d\n", w->depth+1, scope);

    if (w->depth < JSON_WRITER_MAX_DEPTH - 1) {
        w->depth++;
        w->scope_stack[w->depth] = scope;
    } else {
        set_error(w, JW_ERROR_DEPTH_OVERFLOW);
        return;
    }
}

static jw_scope_t pop_scope(json_writer_t *w) {
    fprintf(stderr, "[SCOPE] pop: new_depth=%d\n", w->depth-1);

    if (w->depth > 0) {
        jw_scope_t s = w->scope_stack[w->depth];
        w->depth--;
        return s;
    }

    set_error(w, JW_ERROR_INVALID_SCOPE);
    return JW_SCOPE_NONE;
}

static jw_scope_t current_scope(const json_writer_t *w) {
    return w->scope_stack[w->depth];
}

//since these two happen so frequently together
static void write_value_prefix(json_writer_t *w) {
    if (w->state == JW_STATE_VALUE) {
        // first value in array or object
        if (w->pretty)
            write_indent(w);
    } else if (w->state == JW_STATE_AFTER_VALUE) {
        // comma between values
        write_raw(w, ",", 1);
        if (w->pretty)
            write_indent(w);
    }
}

static int error_check(json_writer_t *w) {
    return w->error != JW_ERROR_NONE;
}

static int can_write_value(json_writer_t *w) {
    // allow first value when in START and at depth 0
    if (w->state == JW_STATE_START && w->depth == 0)
        return 1;

    if (w->state == JW_STATE_VALUE || w->state == JW_STATE_AFTER_VALUE)
    return 1;

    set_error(w, JW_ERROR_INVALID_STATE);
    return 0;
}

void json_writer_begin_object(json_writer_t *w) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;

    if (w->depth >= JSON_WRITER_MAX_DEPTH - 1) {
        set_error(w, JW_ERROR_DEPTH_OVERFLOW);
        return;
    }

    if (!can_write_value(w)) return;

    write_value_prefix(w);

    write_str(w, "{");

    push_scope(w, JW_SCOPE_OBJECT);

    w->indent_level++;
    w->state = JW_STATE_KEY;
    w->need_comma = 0;
    w->scope_stack[w->depth] = JW_SCOPE_OBJECT;

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

void json_writer_end_object(json_writer_t *w) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;

    if (current_scope(w) != JW_SCOPE_OBJECT) {
        set_error(w, JW_ERROR_INVALID_SCOPE);
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

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

void json_writer_begin_array(json_writer_t *w) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;

    if (w->depth >= JSON_WRITER_MAX_DEPTH - 1) {
        set_error(w, JW_ERROR_DEPTH_OVERFLOW);
        return;
    }

    if (!can_write_value(w)) return;

    write_value_prefix(w);

    write_str(w, "[");

    push_scope(w, JW_SCOPE_ARRAY);

    w->indent_level++;
    w->state = JW_STATE_VALUE;
    w->need_comma = 0;
    w->scope_stack[w->depth] = JW_SCOPE_ARRAY;

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

void json_writer_end_array(json_writer_t *w) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;

    if (current_scope(w) != JW_SCOPE_ARRAY) {
        set_error(w, JW_ERROR_INVALID_SCOPE);
        return;
    }

    w->indent_level--;
    if (w->pretty && w->state == JW_STATE_AFTER_VALUE) {
        write_indent(w);
    }

    write_str(w, "]");

    pop_scope(w);

    w->need_comma = 1;
    w->state = JW_STATE_AFTER_VALUE;

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

void json_writer_key(json_writer_t *w, const char *key) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;

    if (current_scope(w) != JW_SCOPE_OBJECT) {
        set_error(w, JW_ERROR_INVALID_SCOPE);
        return;
    }

    if (w->state != JW_STATE_KEY) {
        set_error(w, JW_ERROR_INVALID_STATE);
        return;
    }

    // Comma between key-value pairs
    if (w->need_comma) {
        write_raw(w, ",", 1);
    }
    if (w->pretty)
        write_indent(w);

    // Write "key":
    write_raw(w, "\"", 1);
    write_str(w, key);
    write_raw(w, "\"", 1);
    write_raw(w, ":", 1);

    if (w->pretty) {
        write_raw(w, " ", 1);
    }

    w->state = JW_STATE_VALUE;
    w->need_comma = 0;

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

void json_writer_string(json_writer_t *w, const char *value) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;
    if (!can_write_value(w)) return;
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

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

void json_writer_number(json_writer_t *w, double value) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;
    if (!can_write_value(w)) return;
    write_value_prefix(w);

    // Format the number into a small local buffer
    char buf[32];

    // Deterministic formatting:
    int len = snprintf(buf, sizeof(buf), "%.15g", value);

    // Emit the number
    write_raw(w, buf, (size_t)len);

    // Update state machine
    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

void json_writer_bool(json_writer_t *w, int value) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;
    if (!can_write_value(w)) return;
    write_value_prefix(w);

    if (value) {
        write_str(w, "true");
    } else {
        write_str(w, "false");
    }

    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

void json_writer_null(json_writer_t *w) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;
    if (!can_write_value(w)) return;
    write_value_prefix(w);
    write_str(w, "null");

    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

void json_writer_raw(json_writer_t *w, const char *data) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    if (error_check(w)) return;
    if (!can_write_value(w)) return;
    write_value_prefix(w);
    write_raw(w, data, strlen(data));

    w->state = JW_STATE_AFTER_VALUE;
    w->need_comma = 1;

    fprintf(stderr, "[EXIT ] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

}

int json_writer_error(const json_writer_t *w) {
    fprintf(stderr, "[ENTER] %s: state=%d depth=%d scope=%d need_comma=%d error=%d\n",
        __func__, w->state, w->depth, current_scope(w), w->need_comma, w->error);

    return w->error;
}


