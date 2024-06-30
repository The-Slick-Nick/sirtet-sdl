#ifndef STATE_RUNNER_H
#define STATE_RUNNER_H


#include "application_state.h"


// typedefs for convenience
typedef struct StateRunner StateRunner;

typedef int (*deconstruct_func_t)(void*);
typedef int (*state_func_t)(StateRunner*, ApplicationState*, void*);


// Struct to store information about states
struct StateRunner {

    void **states;  // Array of state structs, stored as void pointers

    state_func_t *runners;  // Array of function pointers to run state
    deconstruct_func_t *deconstructors;  // Array of function pointers to deconstruct state structs

};


#endif
