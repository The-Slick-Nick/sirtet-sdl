/* state_runner.h
 *
 * Header file definition for much of the core run-logic of the engine.
 *
 * The Staterunner struct represents how to handle and run different
 * application state throughout the runtime of the program.
 *
 * The StateRunner manages three stacks of data that pertain to different
 * "states" of execution of the program:
 * 1. State data
 *      Structs of data, stored as void pointers, representing all
 *      state-specific data for a given state. Passed to its corresponding
 *      "runner" for processing and to its corresponding "deconstructor"
 *      when no longer needed.
 * 2. State "runners"
 *      Function pointers to functions that process and do things with
 *      state data. This function should operate a single "frame" of some
 *      input loop. Should be implemented such that they re-cast the void
 *      pointers passed from state data (above) to the correct type of
 *      state struct. "typedef"'d as state_func_t and should expect three
 *      arguments:
 *          StateRunner* (in case the state needs to push additional states)
 *          ApplicationState* (for access to global config, renderers, etc.)
 *          void* (void pointer to the state's specific data)
 *
 *      This function should also return an integer value indicating the
 *      status of this state post execution, returning 0 if it should keep
 *      updating, and -1 if its execution is complete and should be popped
 *      from the stack.
 *
 * 3. State "deconstructors"
 *      Function points to functions that free up any memory and clear
 *      the state structs from (1). Expects a void pointer, corresponding
 *      to state, which is then recast and freed up (however needed).
 *      This function is called after a frame-run of the function indicated
 *      in (2) when said function returns -1.
 * 
 */
#ifndef STATE_RUNNER_H
#define STATE_RUNNER_H


#include "application_state.h"


// typedefs for convenience
typedef struct StateRunner StateRunner;

typedef int (*deconstruct_func_t)(void*);
typedef int (*state_func_t)(StateRunner*, ApplicationState*, void*);


// Struct to store information about states
struct StateRunner {

    int head;  // Index to read from for `states`, `runners`, and `deconstructors`
    int size;

    int buffer_head;  // Index to be added to
    int buffer_tail;  // Index to be read from - lags to reconcile with head

    int buffer_size;
    // NOTE: The max that can be buffered is one less than
    // number of slots allocated, due to collision
    // detection for head & tail upon addition

    void **states;
    // Array of state structs, stored as void pointers.
    // These represent state data for this index. Behaves
    // as a stack.

    state_func_t *runners;
    // Array of function pointers to run state logic.
    // corresponds (by index) to data in `states`.

    deconstruct_func_t *deconstructors;
    // Array of function pointers
    // deconstruct state structs.
    // corresponds (by index) to data in `states`
    

    /* Below are temporary buffers to handle states being
    *  added during exeuction of another state - addition of these to 
    *  be paused until after the current state run finishes execution.
    *  Each is cleared & copied into its corresponding "live" array upon
    *  StateRunner_commitBuffer()
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
