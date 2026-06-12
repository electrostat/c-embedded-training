#include <stdio.h>
#include <stdbool.h>
#include "state_machine_private.h"

//assertion macros
#define ASSERT_TRUE(cond, msg) \
    do { if (!(cond)) { printf("ASSERT_TRUE failed: %s\n", msg); return false; } } while (0)

#define ASSERT_EQ(expected, actual, msg) \
    do { if ((expected) != (actual)) { \
        printf("ASSERT_EQ failed: %s (expected=%d, actual=%d)\n", msg, (int)(expected), (int)(actual)); \
        return false; \
    } } while (0)

static bool test_init(void){
    sm_context_t ctx;
    sm_init(&ctx);

    ASSERT_EQ(SM_STATE_IDLE, sm_get_state(&ctx), "state after init");
    ASSERT_EQ(SM_STATE_IDLE, sm_get_previous_state(&ctx), "previous state");
    ASSERT_EQ(SM_EVENT_RESET, sm_get_last_event(&ctx), "last event");
    ASSERT_EQ(0, sm_get_error_code(&ctx), "error code");
    ASSERT_EQ(0, sm_get_tick_count(&ctx), "tick count");

    return true;
}

// Test: basic transitions
static bool test_basic_transitions(void){
    sm_context_t ctx;
    sm_init(&ctx);

    ASSERT_TRUE(sm_dispatch(&ctx, SM_EVENT_START), "dispatch START from IDLE");
    ASSERT_EQ(SM_STATE_RUNNING, sm_get_state(&ctx), "state after START");

    ASSERT_TRUE(sm_dispatch(&ctx, SM_EVENT_FAULT), "dispatch FAULT from RUNNING");
    ASSERT_EQ(SM_STATE_ERROR, sm_get_state(&ctx),   "state after FAULT");

    ASSERT_TRUE(sm_dispatch(&ctx, SM_EVENT_RESET), "dispatch RESET from ERROR");
    ASSERT_EQ(SM_STATE_IDLE, sm_get_state(&ctx),    "state after RESET");

    return true;
}

// Test: invalid usage
static bool test_invalid_usage(void){
    sm_context_t ctx;
    sm_init(&ctx);

    ASSERT_TRUE(!sm_dispatch(NULL, SM_EVENT_START), "dispatch with NULL ctx");
    ASSERT_TRUE(!sm_dispatch(&ctx, -1), "dispatch negative event");
    ASSERT_TRUE(!sm_dispatch(&ctx, SM_EVENT_COUNT), "dispatch event == SM_EVENT_COUNT");
    ASSERT_TRUE(!sm_dispatch(&ctx, (sm_event_t)999), "dispatch large invalid event");

    return true;
}

int main(void){
    int failures = 0;

    if (!test_init())            failures++;
    if (!test_basic_transitions()) failures++;
    if (!test_invalid_usage())   failures++;

    if (failures == 0) {
        printf("All state_machine tests PASSED\n");
        return 0;
    } else {
        printf("%d state_machine tests FAILED\n", failures);
        return 1;
    }
}