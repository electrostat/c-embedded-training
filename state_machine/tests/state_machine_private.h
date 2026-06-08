#include "state_machine.h"

struct sm_context{
    sm_state_t current_state;
    sm_state_t previous_state;
    sm_event_t last_event;
    int32_t error_code;
    void *user_data;
    uint32_t tick_count;
};