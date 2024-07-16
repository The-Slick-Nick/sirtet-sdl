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
 *      During the exeuction of this function, StateRunner_setPopCount(1)
 *      can be called to indicate to the StateRunner that this state function
 *      should be popped from the stack and not run any subsequent times.
 *      Additionally, StateRunner_setPopCount(x) can be called for x values
 *      greater than 1 to "skip down" more states in the stack.
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

#include <stdlib.h>

/******************************************************************************
 * Type definitions
******************************************************************************/

// typedefs for convenience
typedef struct StateRunner StateRunner;

// Parameter is a pointer to a struct of state data, cast as void
typedef int (*deconstruct_func_t)(void*);

// Params:
// StateRunner* - Pointer to StateRunner struct (to potentially propagate new state)
// void* - Pointer to application-level state data
// void* - Pointer to local-level state data
typedef int (*state_func_t)(StateRunner*, void*, void*);


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
    
    int pop_count;

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


/******************************************************************************
 * Initialization and deconstruction
******************************************************************************/

/** 
 * @brief - Initialize StateRunner with heap allocated memory
 * @param - buffer_size - Integer number of states that can be maintained
 *                        at one time
 * @param - q_size - Integer number of in-transit states that can
 *                   be queued for addition within a single frame
*/
StateRunner* StateRunner_init(int buffer_size, int q_size);


/**
 * @brief - Decommission a StateRunner initialized with StateRunner_init,
 *          freeing all of its allocated memory
 * @param self - Pointer to StateRunner struct to free
 */
int StateRunner_deconstruct(StateRunner *self);


/**
 * @brief - Calculate the number of bytes needed to fully
 *          build a StateRunner struct successfully
 * @param buffer_size - Size of current-state storage
 * @param q_size - Size of added-state storage
 */
size_t StateRunner_requiredBytes(int buffer_size, int q_size);


/**
 * @brief - Initialize a StateRunner pointer from a single allocated
 *          memory block. Experimental, use with caution.
 * @param buffer_size - Integer number of states to maintain at once
 * @param q_size - Integer number of in-transit states that can be maintained
 */
StateRunner* StateRunner_initSingleBlock(int buffer_size, int q_size);


int StateRunner_deconstructSingleBlock(StateRunner *self);


/**
 * @brief - Construct a StateRunner struct from a pre-allocated, provided
 *          chunk of memory
 * @param memory - char pointer to the starting address of a chunk of memory
 *                 to use. Must be at least of size StateRunner_requiredBytes(...)
 * @param buffer_size - Integer number of states to maintain at once
 * @param q_size - Integer number of in-transit states that can be maintained
 */
int StateRunner_build(char *memory, int buffer_size, int q_size);


/******************************************************************************
 * State running
******************************************************************************/


// Add a state to the StateRunner's buffer
int StateRunner_addState(StateRunner *self, void* state_data, state_func_t state_runner, deconstruct_func_t state_deconstructor);

// Migrate buffer states to the "live" state array
int StateRunner_commitBuffer(StateRunner *self);

/**
 * @brief - Run an interation of the "top" state
 * @param self - StateRunner pointer to run from
 * @param app_state - Pointer to some struct holding global,
 *                    application-level state. Passed as a void
 *                    pointer to make this API generic, to be
 *                    recast to whatever ApplicationState
 *                    struct implementation is used
 */
int StateRunner_runState(StateRunner *self, void* app_state);


void StateRunner_flushPop(StateRunner *self);
int StateRunner_getStateCount(StateRunner *self);
int StateRunner_setPopCount(StateRunner *self, int count);



#endif
