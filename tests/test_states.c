
#include <EWENIT.h>
#include <assert.h>

#include "state_runner.h"


// a struct for unit tests
typedef struct {

    int run_count;
    int deconstruct_count;

} TestStruct;


// A state runner that increments a counter but does not terminate
int runFunc(StateRunner *runner, void *app_data, void *state_data) {

    TestStruct *local_state = (TestStruct*)state_data;
    local_state->run_count++;

    return 0;
}

// A state runner that increments a counter then increments a pop count
int runFuncTerminates(StateRunner *runner, void *app_data, void *state_data) {
    TestStruct *local_state = (TestStruct*)state_data;
    local_state->run_count++;

    StateRunner_setPopCount(runner, 1);
    return 0;
}

// A state runner that increments a counter then pops top 5 states
int runFuncPop5(StateRunner *runner, void *app_data, void *state_data) {
         TestStruct *local_state = (TestStruct*)state_data;
         local_state->run_count++;

         StateRunner_setPopCount(runner, 5);
         return 0;
}


int deconFunc(void *state_data) {
    TestStruct *local_state = (TestStruct*)state_data;
    local_state->deconstruct_count++;

    return 0;
}



void testStateRunnerInit() {
    // initialize (and deconstruct) a
    // StateRunner

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

    // The state has been "add"ed but not "commit"ed, therefore should be 0
    ASSERT_EQUAL_INT(StateRunner_getStateCount(runner), 0);
    ASSERT_EQUAL_INT(test_struct.run_count, 0);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 0);

    // now it should actually run
    StateRunner_commitBuffer(runner);
    ASSERT_EQUAL_INT(StateRunner_getStateCount(runner), 1);

    StateRunner_runState(runner, NULL);
    ASSERT_EQUAL_INT(test_struct.run_count, 1);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 0);
}


void testRunWithDecon() {

    StateRunner *runner = StateRunner_init(32, 16);
    TestStruct test_struct = {0, 0};

    StateRunner_addState(
        runner, (void*)&test_struct, runFuncTerminates, deconFunc
    );
    StateRunner_commitBuffer(runner);
    StateRunner_runState(runner, NULL);

    ASSERT_EQUAL_INT(test_struct.run_count, 1);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 1);


    StateRunner_deconstruct(runner);
}

void testRunMultiple() {

    StateRunner *runner = StateRunner_init(32, 16);
    TestStruct test_struct = {0, 0};

    // two deconstructors
    StateRunner_addState(
        runner, (void*)&test_struct, runFuncTerminates, deconFunc);
    StateRunner_addState(
        runner, (void*)&test_struct, runFuncTerminates, deconFunc);
    StateRunner_commitBuffer(runner);

    StateRunner_runState(runner, NULL);
    ASSERT_EQUAL_INT(test_struct.run_count, 1);
    // there should still be a live ref to test_struct, so no termination
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 0);  

    ASSERT_EQUAL_INT(StateRunner_getStateCount(runner), 1);

    StateRunner_runState(runner, NULL);
    ASSERT_EQUAL_INT(test_struct.run_count, 2);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 1);

    ASSERT_EQUAL_INT(StateRunner_getStateCount(runner), 0);

    StateRunner_deconstruct(runner);
}

void testNullDeconstructor() {

    StateRunner *runner = StateRunner_init(32, 16);
    TestStruct test_struct = {0, 0};

    StateRunner_addState(runner, (void*)&test_struct, runFuncTerminates, NULL);
    StateRunner_commitBuffer(runner);

    StateRunner_runState(runner, NULL);
    ASSERT_EQUAL_INT(test_struct.run_count, 1);
    ASSERT_EQUAL_INT(test_struct.deconstruct_count, 0);

    StateRunner_deconstruct(runner);

}

void testPopcountReset() {

    StateRunner *runner = StateRunner_init(32, 16);
    TestStruct test_struct = {0, 0};

    StateRunner_addState(runner, (void*)&test_struct, runFunc, NULL);
    StateRunner_commitBuffer(runner);

    StateRunner_setPopCount(runner, 1);
    StateRunner_runState(runner, NULL);

    // this should not have popped - popCount should reset between calls
    ASSERT_EQUAL_INT(StateRunner_getStateCount(runner), 1);


    StateRunner_deconstruct(runner);
}


void testPopMultiple() {
    // skip through more than 1 state with popping

    StateRunner *runner = StateRunner_init(32, 16);
    TestStruct test_struct = {0, 0};

    for (int i = 0; i < 7; i++) {
        StateRunner_addState(
            runner, (void*)&test_struct, runFuncPop5, deconFunc
        );
    }
    StateRunner_commitBuffer(runner);
    ASSERT_EQUAL_INT(StateRunner_getStateCount(runner), 7);

    StateRunner_runState(runner, NULL);
    ASSERT_EQUAL_INT(StateRunner_getStateCount(runner), 2);

    // Popping more than present simply goes to 0
    StateRunner_runState(runner, NULL);
    ASSERT_EQUAL_INT(StateRunner_getStateCount(runner), 0);

    StateRunner_deconstruct(runner);
}



int main() {
    EWENIT_START;

    ADD_CASE(testStateRunnerInit);
    ADD_CASE(testRunState);
    ADD_CASE(testRunWithDecon);
    ADD_CASE(testRunMultiple);
    ADD_CASE(testNullDeconstructor);

    ADD_CASE(testPopcountReset);
    ADD_CASE(testPopMultiple);

    EWENIT_END;

}
