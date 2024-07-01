/* state_runner.c
*
* Logic implementation for operating on a StateRunner struct
*/

#include "state_runner.h"
#include "application_state.h"

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

        // Copy from read head to write head
        self->states[self->head] = self->states_buffer[self->buffer_tail];
        self->runners[self->head] = self->runners_buffer[self->buffer_tail];
        self->deconstructors[self->head] = self->deconstructors_buffer[self->buffer_tail];

        // Move read head
        self->buffer_tail = (self->buffer_tail + 1) % self->buffer_size;
    }
    return 0;

}

// Run an iteration of the "top" state
int StateRunner_runState(StateRunner *self, ApplicationState* app_state) {

    if (self->head < 0) {
        return -1;
    }

    state_func_t top_runner = self->runners[self->head];
    deconstruct_func_t top_decon = self->deconstructors[self->head];
    void* top_state = self->states[self->head];

    int retval = top_runner(self, app_state, top_state);

    // TODO: Macro to makes codes more understandable (something something semantic)
    if (retval == -1) {
        top_decon(top_state);
        self->head--;
    }

    return 0;

}
