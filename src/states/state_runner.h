#ifndef STATE_RUNNER_H
#define STATE_RUNNER_H


#include "application_state.h"


// typedefs for convenience
typedef struct StateRunner StateRunner;

typedef int (*deconstruct_func_t)(void*);
typedef int (*state_func_t)(StateRunner*, ApplicationState*, void*);


// Struct to store information about states
struct StateRunner {

    int head;
    int size;

    int buffer_head;  // Index to be added to
    int buffer_tail;  // Index to be read from - lags to reconcile with head
    int buffer_size;    // NOTE: The max that can be buffered is one less than
                        // number of slots allocated, due to collision
                        // detection for head & tail upon addition

    void **states;  // Array of state structs, stored as void pointers
    state_func_t *runners;  // Array of function pointers to run state
    deconstruct_func_t *deconstructors;  // Array of function pointers to deconstruct state structs
    

    /* Below are temporary buffers to handle states being
    *  added during exeuction of another state - addition of these to 
    *  be paused until after the current state run finishes execution
    */
    void **states_buffer;
    state_func_t *runners_buffer;
    deconstruct_func_t *deconstructors_buffer;

};


// Add a state to the StateRunner's buffer
int StateRunner_addState(StateRunner *self, void* state_data, state_func_t state_runner, deconstruct_func_t state_deconstructor);

// Migrate buffer states to the "live" state array
int StateRunner_commitBuffer(StateRunner *self);

// Run an iteration of the "top" state
int StateRunner_runState(StateRunner *self, ApplicationState* app_state);

#endif
