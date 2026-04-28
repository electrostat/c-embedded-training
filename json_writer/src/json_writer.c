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