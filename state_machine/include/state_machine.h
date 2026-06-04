#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

//state identifiers
typedef enum {
    SM_STATE_IDLE = 0,
    SM_STATE_RUNNING,
    SM_STATE_ERROR,
    SM_STATE_COUNT
} sm_state_t;

//event identifiers
typedef enum {
    SM_EVENT_START = 0,
    SM_EVENT_STOP,
    SM_EVENT_FAULT,
    SM_EVENT_RESET,
    SM_EVENT_COUNT
} sm_event_t;

typedef struct sm_context sm_context_t;

//Public APIs
//------------------------------------------------------
//initialize state machine
void sm_init(sm_context_t *ctx);

//dispatch even to state machine. Returns True if causes valid transition
bool sm_dispatch(sm_context_t *ctx, sm_event_t event);

//helper to query current state
sm_state_t sm_get_state(const sm_context_t *ctx);

#endif