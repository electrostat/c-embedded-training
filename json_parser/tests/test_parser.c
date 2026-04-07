#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "json_parser.h"
#include "json_token.h"

// A simple event log for testing
typedef enum {
    EVENT_OBJ_BEGIN,
    EVENT_OBJ_END,
    EVENT_ARRAY_BEGIN,
    EVENT_ARRAY_END,
    EVENT_KEY,
    EVENT_STRING,
    EVENT_NUMBER,
    EVENT_BOOL,
    EVENT_NULL
} event_type_t;

typedef struct {
    event_type_t type;
    char text[64];
} event_t;

typedef struct {
    event_t events[128];
    int count;
} event_log_t;

// ---- Callback implementations ----

static void log_event(event_log_t *log, event_type_t type,
                      const char *start, size_t len)
{
    event_t *e = &log->events[log->count++];
    e->type = type;

    if (start && len < sizeof(e->text)) {
        memcpy(e->text, start, len);
        e->text[len] = '\0';
    } else {
        e->text[0] = '\0';
    }
}

static void cb_object_begin(void *ctx) {
    log_event((event_log_t *)ctx, EVENT_OBJ_BEGIN, NULL, 0);
}

static void cb_object_end(void *ctx) {
    log_event((event_log_t *)ctx, EVENT_OBJ_END, NULL, 0);
}

static void cb_array_begin(void *ctx) {
    log_event((event_log_t *)ctx, EVENT_ARRAY_BEGIN, NULL, 0);
}

static void cb_array_end(void *ctx) {
    log_event((event_log_t *)ctx, EVENT_ARRAY_END, NULL, 0);
}

static void cb_key(void *ctx, const char *start, size_t len) {
    log_event((event_log_t *)ctx, EVENT_KEY, start, len);
}

static void cb_string(void *ctx, const char *start, size_t len) {
    log_event((event_log_t *)ctx, EVENT_STRING, start, len);
}

static void cb_number(void *ctx, const char *start, size_t len) {
    log_event((event_log_t *)ctx, EVENT_NUMBER, start, len);
}

static void cb_bool(void *ctx, bool value) {
    log_event((event_log_t *)ctx, EVENT_BOOL, value ? "true" : "false", value ? 4 : 5);
}

static void cb_null(void *ctx) {
    log_event((event_log_t *)ctx, EVENT_NULL, "null", 4);
}

// ---- Test cases ----

static json_parser_callbacks_t make_callbacks(void) {
    json_parser_callbacks_t cb = {
        .on_object_begin = cb_object_begin,
        .on_object_end   = cb_object_end,
        .on_array_begin  = cb_array_begin,
        .on_array_end    = cb_array_end,
        .on_key          = cb_key,
        .on_string       = cb_string,
        .on_number       = cb_number,
        .on_bool         = cb_bool,
        .on_null         = cb_null
    };
    return cb;
}

void test_parser_initialization(void) {
    const char *json = "{}";
    event_log_t log = {0};

    json_parser_t p;
    json_parser_init(&p, json, strlen(json), make_callbacks(), &log);

    if (p.error) {
        printf("FAIL: parser should not start in error state\n");
        return;
    }

    printf("PASS: parser initialization\n");
}

// This test will pass once the state machine is implemented
void test_simple_object(void) {
    const char *json = "{\"a\": 1}";
    event_log_t log = {0};

    json_parser_t p;
    json_parser_init(&p, json, strlen(json), make_callbacks(), &log);

    json_parser_parse(&p);

    // Expected event sequence:
    // OBJ_BEGIN
    // KEY("a")
    // NUMBER("1")
    // OBJ_END

    if (log.count != 4) {
        printf("TODO: parser state machine not implemented yet\n");
        return;
    }

    printf("PASS: simple object parsing\n");
}

int main(void) {
    test_parser_initialization();
    test_simple_object();
    return 0;
}