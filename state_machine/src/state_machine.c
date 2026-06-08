#include <stddef.h>
#include "state_machine.h"

struct sm_context {
    sm_state_t current_state;
    sm_state_t previous_state;
    sm_event_t last_event;
    int32_t error_code;
    void *user_data;            //external context pointer
    uint32_t tick_count;        //internal counter/timer
};

typedef bool (*sm_transition_fn)(sm_context_t *ctx);

typedef struct{
    sm_state_t next_state;
    sm_transition_fn handler;
} sm_transition_t;

static const sm_transition_t sm_table[SM_STATE_COUNT][SM_EVENT_COUNT] = {
    //IDLE
    [SM_STATE_IDLE] = {
        [SM_EVENT_START] = { SM_STATE_RUNNING, NULL },
        [SM_EVENT_STOP]  = { SM_STATE_IDLE,    NULL },
        [SM_EVENT_FAULT] = { SM_STATE_ERROR,   NULL },
        [SM_EVENT_RESET] = { SM_STATE_IDLE,    NULL }
    },
    //RUNNING
    [SM_STATE_RUNNING] = {
        [SM_EVENT_START] = { SM_STATE_RUNNING, NULL },
        [SM_EVENT_STOP]  = { SM_STATE_IDLE,    NULL },
        [SM_EVENT_FAULT] = { SM_STATE_ERROR,   NULL },
        [SM_EVENT_RESET] = { SM_STATE_IDLE,    NULL }
    },
    //ERROR
    [SM_STATE_ERROR] = {
        [SM_EVENT_START] = { SM_STATE_ERROR, NULL },
        [SM_EVENT_STOP]  = { SM_STATE_ERROR, NULL },
        [SM_EVENT_FAULT] = { SM_STATE_ERROR, NULL },
        [SM_EVENT_RESET] = { SM_STATE_IDLE,  NULL }
    }
};

void sm_init(sm_context_t *ctx){
    if(ctx == NULL) {
        return;
    }

    ctx->current_state  = SM_STATE_IDLE;
    ctx->previous_state = SM_STATE_IDLE;
    ctx->last_event     = SM_EVENT_RESET;
    ctx->error_code     = 0;
    ctx->user_data      = NULL;
    ctx->tick_count     = 0;
}

bool sm_dispatch(sm_context_t *ctx, sm_event_t event){
    //invalid context
    if (ctx == NULL){
        return false;
    }

    //invalid event
    if (event == SM_EVENT_COUNT){
        return false;
    }

    sm_state_t current = ctx->current_state;
    const sm_transition_t *t = &sm_table[current][event];

    //no transition defined
    if (t->next_state >= SM_STATE_COUNT) {
        return false;
    }

    //guard side effects
    if (t->handler != NULL) {
        bool allowed = t->handler(ctx);
        if (!allowed) {
            return false;
        }
    }

    // Perform transition
    ctx->previous_state = ctx->current_state;
    ctx->current_state = t->next_state;
    ctx->last_event = event;

    return true;
}