#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include <stddef.h>

//for C++ consumption
#ifdef __cplusplus
extern "C" {
#endif

//JSON Writer — zero‑malloc, streaming JSON serialization for embedded systems.
#define JSON_WRITER_MAX_DEPTH 32

typedef struct json_writer json_writer_t;

typedef void (*json_writer_output_fn)(
    void *ctx,
    const char *data,
    size_t len
);

//internal scope
typedef enum {
    JW_SCOPE_NONE = 0,
    JW_SCOPE_OBJECT,
    JW_SCOPE_ARRAY
} jw_scope_t;

//state machine
typedef enum {
    JW_STATE_START = 0,     // nothing written
    JW_STATE_KEY,           // expecting a key
    JW_STATE_VALUE,         // expecting a value
    JW_STATE_AFTER_VALUE    // value written
} jw_state_t;

//error codes
typedef enum {
    JW_ERROR_NONE = 0,
    JW_ERROR_INVALID_STATE,
    JW_ERROR_INVALID_SCOPE,
    JW_ERROR_DEPTH_OVERFLOW
} jw_error_t;

struct json_writer {
    //Output plumbing
    json_writer_output_fn out;
    void *ctx;

    //Formatting configuration
    int pretty;
    int indent_width;
    int indent_level;

    //State machine
    jw_state_t state;

    //Scope stack for nested objects/arrays
    jw_scope_t scope_stack[JSON_WRITER_MAX_DEPTH];
    int depth;

    //Whether the next value should be preceded by a comma
    int need_comma;

    //error code
    int error;
};

//Initialize the writer
void json_writer_init(
    json_writer_t *w,
    json_writer_output_fn out,
    void *ctx,
    int pretty,
    int indent_width
);

void json_writer_reset(json_writer_t *w);

// Structural tokens
void json_writer_begin_object(json_writer_t *w);
void json_writer_end_object(json_writer_t *w);

void json_writer_begin_array(json_writer_t *w);
void json_writer_end_array(json_writer_t *w);

//Object key (must be followed by a value)
void json_writer_key(json_writer_t *w, const char *key);

// Primitive values
void json_writer_string(json_writer_t *w, const char *value);
void json_writer_number(json_writer_t *w, double value);
void json_writer_bool(json_writer_t *w, int value);
void json_writer_null(json_writer_t *w);

//if user wants raw data without any manipulation
void json_writer_raw(json_writer_t *w, const char *data);

//error output
int json_writer_error(const json_writer_t *w);

#ifdef __cplusplus
}
#endif

#endif