/* state_runner.c
*
* Logic implementation for operating on a StateRunner struct
*/
#include <stdio.h>

#include "state_runner.h"
#include "sirtet.h"


/* ============================================================================
 * State management
 ============================================================================*/


// Add a state to the StateRunner's buffer
int StateRunner_addState(
    StateRunner* self, void* state_data,
    state_func_t state_runner,
    deconstruct_func_t state_deconstructor
) {

    self->states_buffer[self->buffer_head] = state_data;
    self->runners_buffer[self->buffer_head] = state_runner;
    self->deconstructors_buffer[self->buffer_head] = state_deconstructor;

    /* rotating buffer - queue-like (rather than stack-like) */
    self->buffer_head = (self->buffer_head + 1) % self->buffer_size;

    // Caution around overflow & wraparound
    if (self->buffer_head == self->buffer_tail) {
        return -1;
    }

    return 0;
}

// Migrate buffer states to the "live" state array
int StateRunner_commitBuffer(StateRunner *self) {

    while (self->buffer_tail != self->buffer_head) {

        // Move write head
        self->head++;

        if (self->head >= self->size) {
            return -1;
        }

        // Copy from read head to write head
        self->states[self->head] = self->states_buffer[self->buffer_tail];
        self->runners[self->head] = self->runners_buffer[self->buffer_tail];
        self->deconstructors[self->head] = self->deconstructors_buffer[self->buffer_tail];

        // Move read head
        self->buffer_tail = (self->buffer_tail + 1) % self->buffer_size;

        if (self->buffer_tail >= self->buffer_size) {
            return -1;
        }

    }
    return 0;

}


/**
 * @brief Flush the StateRunner's pop buffer
 * @param self - StateRunner pointer to clear
 */
void StateRunner_flushPop(StateRunner *self) {

    for (;self->pop_count > 0 && self->head >= 0; self->pop_count--, self->head--) {

        deconstruct_func_t decon = self->deconstructors[self->head];
        void *state = self->states[self->head];

        if (decon != NULL) {
            int refcount = 0;
            for (int stack_i = 0; stack_i <= self->head; stack_i++) {
                if (self->states[stack_i] == state) {
                    refcount++;
                }
            }

            for (int queue_i = self->buffer_tail;
                 queue_i != self->buffer_head;
                 queue_i = (queue_i + 1) % self->buffer_size) {

                if (self->states_buffer[queue_i] == state) {
                    refcount++;
                }
            }

            if (refcount == 1) {
                decon(state);
            }

        }
    }
}

/**
 * @brief - Run an interation of the "top" state
 * @param self - StateRunner pointer to run from
 * @param app_state - Pointer to some struct holding global,
 *                    application-level state. Passed as a void
 *                    pointer to make this API generic, to be
 *                    recast to whatever ApplicationState
 *                    struct implementation is used
 */
int StateRunner_runState(StateRunner *self, void* app_state) {

    if (self->head < 0 || self->head >= self->size) {
        return -1;
    }

    state_func_t top_runner = self->runners[self->head];
    void* top_state = self->states[self->head];

    StateRunner_flushPop(self);
    int retval = top_runner(self, app_state, top_state);
    if (retval < 0) {
        char *errmsg = Sirtet_getError();
        printf("Error running state function:\n    %s\n", errmsg);
        return -1;
    }
    StateRunner_flushPop(self);
    return 0;

}

int StateRunner_getStateCount(StateRunner *self) {
    return self->head + 1;
}

int StateRunner_setPopCount(StateRunner *self, int count) {
    if (count < 0) {
        return -1;
    }
    self->pop_count = count;
    return 0;
}

/* ============================================================================
 * Initialization and deconstruction
 ============================================================================*/


/**
 * @brief - Calculate the number of bytes needed to fully
 *          build a StateRunner struct successfully
 * @param buffer_size - Size of current-state storage
 * @param q_size - Size of added-state storage
 */
size_t StateRunner_requiredBytes(int buffer_size, int q_size) {

    size_t bytes_needed = 0;

    // covers by-value memory stuff
    bytes_needed += sizeof(StateRunner);

    // states & states buffer memory
    bytes_needed += sizeof(void*) * (buffer_size + q_size);

    // runners & runners buffer memory
    bytes_needed += sizeof(state_func_t) * (buffer_size + q_size);

    // deconstructors & deconstructors buffer
    bytes_needed += sizeof(deconstruct_func_t) * (buffer_size + q_size);

    return bytes_needed;
}




/** 
 * @brief - Initialize StateRunner with heap allocated memory. Must be
 *          later freed with StateRunner_deconstruct(...)
 * @param - buffer_size - Integer number of states that can be maintained
 *                        at one time
 * @param - q_size - Integer number of in-transit states that can
 *                   be queued for addition within a single frame
*/
StateRunner* StateRunner_init(int buffer_size, int q_size) {

    size_t sf_sz = sizeof(state_func_t);
    size_t decon_sz = sizeof(deconstruct_func_t);

    StateRunner *runner = (StateRunner*)malloc(sizeof(StateRunner));

    *runner = (StateRunner){

        .head = -1,
        .size = buffer_size,

        .buffer_head = 0,
        .buffer_tail = 0,
        .buffer_size = q_size,
        .pop_count = 0,

        .states = malloc(buffer_size * sizeof(void*)),
        .states_buffer = malloc(q_size * sizeof(void*)),

        .runners = (state_func_t*)malloc(buffer_size * sf_sz),
        .runners_buffer = (state_func_t*)malloc(q_size * sf_sz),

        .deconstructors = (deconstruct_func_t*)malloc(buffer_size * decon_sz),
        .deconstructors_buffer = (deconstruct_func_t*)malloc(q_size * decon_sz)
    };
    return runner;
}


/**
 * @brief - Initialize a StateRunner pointer from a single allocated
 *          memory block. Experimental, use with caution. Must either call
 *          StateRunner_deconstructSingleBlock(...) or directly call free.
 * @param buffer_size - Integer number of states to maintain at once
 * @param q_size - Integer number of in-transit states that can be maintained
 */
StateRunner* StateRunner_initSingleBlock(int buffer_size, int q_size) {

    size_t req_bytes = StateRunner_requiredBytes(buffer_size, q_size);

    char *memory = (char*)malloc(req_bytes);

    StateRunner_build(memory, buffer_size, q_size);
    return (StateRunner*)memory;
}


/**
 * @brief - Construct a StateRunner struct from a pre-allocated, provided
 *          chunk of memory
 * @param memory - char pointer to the starting address of a chunk of memory
 *                 to use. Must be at least of size StateRunner_requiredBytes(...)
 * @param buffer_size - Integer number of states to maintain at once
 * @param q_size - Integer number of in-transit states that can be maintained
 */
int StateRunner_build(char *memory, int buffer_size, int q_size) {

    *(StateRunner*)(memory) = (StateRunner){

        .head = -1,
        .size = buffer_size,

        .buffer_head = 0,
        .buffer_tail = 0,
        .buffer_size = 16
    };

    StateRunner *runner = (StateRunner*)(memory);
    int offset = sizeof(StateRunner);

    runner->states = (void*)(memory + offset);
    offset += buffer_size * sizeof(void*);

    runner->states_buffer = (void*)(memory + offset);
    offset += q_size * sizeof(void*);

    runner->runners = (state_func_t*)(memory + offset);
    offset += buffer_size * sizeof(state_func_t);

    runner->runners_buffer = (state_func_t*)(memory + offset);
    offset += q_size * sizeof(state_func_t);


    runner->deconstructors = (deconstruct_func_t*)(memory + offset);
    offset += buffer_size * sizeof(deconstruct_func_t);

    runner->deconstructors_buffer = (deconstruct_func_t*)(memory + offset);

    return 0;
}


/**
 * @brief - Decommission a StateRunner initialized with StateRunner_init,
 *          freeing all of its allocated memory
 * @param self - Pointer to StateRunner struct to free
 */
int StateRunner_deconstruct(StateRunner *self) {

    StateRunner_commitBuffer(self);
    StateRunner_setPopCount(self, StateRunner_getStateCount(self));
    StateRunner_flushPop(self);


    free(self->states);
    free(self->states_buffer);

    free(self->runners);
    free(self->runners_buffer);

    free(self->deconstructors);
    free(self->deconstructors_buffer);


    free(self);
    return 0;
}

int StateRunner_deconstructSingleBlock(StateRunner *self) {
    free(self);
    return 0;
}

