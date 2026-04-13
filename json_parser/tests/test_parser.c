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

void test_empty_object(void) {
    const char *json = "{}";
    event_log_t log = {0};

    json_parser_t p;
    json_parser_init(&p, json, strlen(json), make_callbacks(), &log);
    json_parser_parse(&p);

    if (log.count != 2 ||
        log.events[0].type != EVENT_OBJ_BEGIN ||
        log.events[1].type != EVENT_OBJ_END) {
        printf("FAIL: empty object\n");
        return;
    }

    printf("PASS: empty object\n");
}

void test_simple_array(void) {
    const char *json = "[1,2,3]";
    event_log_t log = {0};

    json_parser_t p;
    json_parser_init(&p, json, strlen(json), make_callbacks(), &log);
    json_parser_parse(&p);

    if (log.count != 5 ||
        log.events[0].type != EVENT_ARRAY_BEGIN ||
        log.events[1].type != EVENT_NUMBER ||
        strcmp(log.events[1].text, "1") != 0 ||
        log.events[2].type != EVENT_NUMBER ||
        strcmp(log.events[2].text, "2") != 0 ||
        log.events[3].type != EVENT_NUMBER ||
        strcmp(log.events[3].text, "3") != 0 ||
        log.events[4].type != EVENT_ARRAY_END) {
        printf("FAIL: simple array\n");
        return;
    }

    printf("PASS: simple array\n");
}

void test_nested_object(void) {
    const char *json = "{\"a\":{\"b\":2}}";
    event_log_t log = {0};

    json_parser_t p;
    json_parser_init(&p, json, strlen(json), make_callbacks(), &log);
    json_parser_parse(&p);

    if (log.count != 7 ||
        log.events[0].type != EVENT_OBJ_BEGIN ||
        log.events[1].type != EVENT_KEY ||
        strcmp(log.events[1].text, "a") != 0 ||
        log.events[2].type != EVENT_OBJ_BEGIN ||
        log.events[3].type != EVENT_KEY ||
        strcmp(log.events[3].text, "b") != 0 ||
        log.events[4].type != EVENT_NUMBER ||
        strcmp(log.events[4].text, "2") != 0 ||
        log.events[5].type != EVENT_OBJ_END ||
        log.events[6].type != EVENT_OBJ_END) {
        printf("FAIL: nested object\n");
        return;
    }

    printf("PASS: nested object\n");
}

void test_mixed_types(void) {
    const char *json = "{\"a\":[1,true,null,\"x\"]}";
    event_log_t log = {0};

    json_parser_t p;
    json_parser_init(&p, json, strlen(json), make_callbacks(), &log);
    json_parser_parse(&p);

    if (log.count != 9 ||
        log.events[0].type != EVENT_OBJ_BEGIN ||
        log.events[1].type != EVENT_KEY ||
        strcmp(log.events[1].text, "a") != 0 ||
        log.events[2].type != EVENT_ARRAY_BEGIN ||
        log.events[3].type != EVENT_NUMBER ||
        strcmp(log.events[3].text, "1") != 0 ||
        log.events[4].type != EVENT_BOOL ||
        strcmp(log.events[4].text, "true") != 0 ||
        log.events[5].type != EVENT_NULL ||
        log.events[6].type != EVENT_STRING ||
        strcmp(log.events[6].text, "x") != 0 ||
        log.events[7].type != EVENT_ARRAY_END ||
        log.events[8].type != EVENT_OBJ_END) {
        printf("FAIL: mixed types\n");
        return;
    }

    printf("PASS: mixed types\n");
}

void test_error_missing_colon(void) {
    const char *json = "{\"a\" 1}";
    event_log_t log = {0};

    json_parser_t p;
    json_parser_init(&p, json, strlen(json), make_callbacks(), &log);
    json_parser_parse(&p);

    if (!p.error) {
        printf("FAIL: missing colon should error\n");
        return;
    }

    printf("PASS: error missing colon\n");
}

void test_error_missing_comma(void) {
    const char *json = "{\"a\":1 \"b\":2}";
    event_log_t log = {0};

    json_parser_t p;
    json_parser_init(&p, json, strlen(json), make_callbacks(), &log);
    json_parser_parse(&p);

    if (!p.error) {
        printf("FAIL: missing comma should error\n");
        return;
    }

    printf("PASS: error missing comma\n");
}

void test_convenience_api(void) {
    const char *json = "{\"a\":1}";
    event_log_t log = {0};

    bool ok = json_parse(json, strlen(json), make_callbacks(), &log);

    if (!ok) {
        printf("FAIL: convenience API returned error\n");
        return;
    }

    if (log.count != 4 ||
        log.events[0].type != EVENT_OBJ_BEGIN ||
        log.events[1].type != EVENT_KEY ||
        strcmp(log.events[1].text, "a") != 0 ||
        log.events[2].type != EVENT_NUMBER ||
        strcmp(log.events[2].text, "1") != 0 ||
        log.events[3].type != EVENT_OBJ_END) {
        printf("FAIL: convenience API event sequence\n");
        return;
    }

    printf("PASS: convenience API\n");
}

int main(void) {
    test_parser_initialization();
    test_simple_object();
    test_empty_object();
    test_simple_array();
    test_nested_object();
    test_mixed_types();
    test_error_missing_colon();
    test_error_missing_comma();
    test_convenience_api();
    return 0;
}