#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include <stddef.h>

//for C++ consumption
#ifdef __cplusplus
extern "C" {
#endif

//JSON Writer — zero‑malloc, streaming JSON serialization for embedded systems.

typedef struct json_writer json_writer_t;

typedef void (*json_writer_output_fn)(
    void *ctx,
    const char *data,
    size_t len
);

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

#ifdef __cplusplus
}
#endif

#endif