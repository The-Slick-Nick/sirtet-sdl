
#include <EWENIT.h>
#include <assert.h>

#include "state_runner.h"



// a struct for unit tests
typedef struct {

    int run_count;
    int deconstruct_count;

} TestStruct;


int runFunc(StateRunner *runner, void *app_data, void *state_data) {

    TestStruct *local_state = (TestStruct*)state_data;
    local_state->run_count++;

    return 0;

}

int runFuncTerminates(StateRunner *runner, void *app_data, void *state_data) {

    TestStruct *local_state = (TestStruct*)state_data;
    local_state->run_count++;

    return -1;
}


int deconFunc(void *state_data) {
    TestStruct *local_state = (TestStruct*)state_data;
    local_state->deconstruct_count++;

    return 0;
}



void testStateRunnerInit() {
    // initialize (and deconstruct) a
    // StateRunner
    //

    StateRunner *runner = StateRunner_init(32, 16);

    ASSERT_EQUAL_INT(runner->size, 32);
    ASSERT_EQUAL_INT(runner->buffer_size, 16);

    StateRunner_deconstruct(runner);


    StateRunner *runner2 = StateRunner_initSingleBlock(32, 16);
    ASSERT_EQUAL_INT(runner2->size, 32);
    ASSERT_EQUAL_INT(runner2->buffer_size, 16);

    StateRunner_deconstructSingleBlock(runner2);


    assert(10000 >= StateRunner_requiredBytes(32, 16));
    char mem_buffer[10000];
    StateRunner_build(mem_buffer, 32, 16);

    StateRunner *runner3 = (StateRunner*)(mem_buffer);
    ASSERT_EQUAL_INT(runner3->size, 32);
    ASSERT_EQUAL_INT(runner3->buffer_size, 16);

    // stack memory - no deconstruction needed

}


void testRunState() {

    char mem_buffer[1000];
    StateRunner_build(mem_buffer, 32, 16);
    StateRunner *runner = (StateRunner*)mem_buffer;

    TestStruct test_struct = {0, 0};

    StateRunner_addState(runner, (void*)&test_struct, runFunc, deconFunc);
    StateRunner_runState(runner, NULL);

    // not having commit added states, should still be 0
    ASSERT_EQUAL_INT(test_struct.run_count, 0);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 0);

    // now it should actually run
    StateRunner_commitBuffer(runner);
    StateRunner_runState(runner, NULL);

    ASSERT_EQUAL_INT(test_struct.run_count, 1);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 0);
}


void testRunWithDecon() {

    StateRunner *runner = StateRunner_init(32, 16);
    TestStruct test_struct = {0, 0};

    StateRunner_addState(runner, (void*)&test_struct, runFuncTerminates, deconFunc);
    StateRunner_commitBuffer(runner);

    StateRunner_runState(runner, NULL);
    ASSERT_EQUAL_INT(test_struct.run_count, 1);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 1);


    StateRunner_deconstruct(runner);
}

void testRunMultiple() {

    StateRunner *runner = StateRunner_init(32, 16);
    TestStruct test_struct = {0, 0};

    StateRunner_addState(runner, (void*)&test_struct, runFunc, deconFunc);
    StateRunner_addState(runner, (void*)&test_struct, runFuncTerminates, deconFunc);
    StateRunner_commitBuffer(runner);

    StateRunner_runState(runner, NULL);
    ASSERT_EQUAL_INT(test_struct.run_count, 1);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 1);

    StateRunner_runState(runner, NULL);
    ASSERT_EQUAL_INT(test_struct.run_count, 2);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 1);

    StateRunner_deconstruct(runner);

}




int main() {
    EWENIT_START;

    ADD_CASE(testStateRunnerInit);
    ADD_CASE(testRunState);
    ADD_CASE(testRunWithDecon);
    ADD_CASE(testRunMultiple);

    EWENIT_END;

}
