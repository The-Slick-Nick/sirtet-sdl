
#include <EWENIT.h>

#include "inputs.h"
#include "menu.h"



typedef struct {
    int value1;
    int value2;
} TestState;



// typedef void (*menufunc_t)(StateRunner*, void*, void*);
void incValue1(StateRunner *runner, void *app_data, void *state_data) {
    TestState *data = (TestState*)state_data;
    data->value1++;
}

void incValue2(StateRunner *runner, void *app_data, void *state_data) {
    TestState *data = (TestState*)state_data;
    data->value2++;
}



void testCreation() {

    Menu* menu = Menu_init(4, NULL);


    ASSERT_EQUAL_INT(menu->max_options, 4);
    ASSERT_EQUAL_INT(menu->num_options, 0);

    Menu_addOption(menu);
    Menu_addOption(menu);
    Menu_addOption(menu);
    Menu_addOption(menu);

    ASSERT_EQUAL_INT(menu->num_options, 4);

    // max options exceeded
    int retval = Menu_addOption(menu);
    ASSERT_EQUAL_INT(retval, -1);
    ASSERT_EQUAL_INT(menu->num_options, 4);


    Menu_deconstruct(menu);
}

void testCommands () {

    Menu* menu = Menu_init(2, NULL);

    TestState state = {0, 0};

    int opt1 = Menu_addOption(menu);
    int opt2 = Menu_addOption(menu);

    Menu_setCommand(menu, opt1, MENUCODE_SELECT, incValue1);
    Menu_setCommand(menu, opt2, MENUCODE_SELECT, incValue2);

    Menu_runCommand(menu, MENUCODE_SELECT, NULL, NULL, &state);
    ASSERT_EQUAL_INT(state.value1, 1);
    ASSERT_EQUAL_INT(state.value2, 0);

    Menu_nextOption(menu);
    Menu_runCommand(menu, MENUCODE_SELECT, NULL, NULL, &state);
    ASSERT_EQUAL_INT(state.value1, 1);
    ASSERT_EQUAL_INT(state.value2, 1);

    // already at highest option - same option
    Menu_nextOption(menu);
    Menu_runCommand(menu, MENUCODE_SELECT, NULL, NULL, &state);
    ASSERT_EQUAL_INT(state.value1, 1);
    ASSERT_EQUAL_INT(state.value2, 2);

    Menu_prevOption(menu);
    Menu_runCommand(menu, MENUCODE_SELECT, NULL, NULL, &state);
    ASSERT_EQUAL_INT(state.value1, 2);
    ASSERT_EQUAL_INT(state.value2, 2);

    // already at lowest option - same option
    Menu_prevOption(menu);
    Menu_runCommand(menu, MENUCODE_SELECT, NULL, NULL, &state);
    ASSERT_EQUAL_INT(state.value1, 3);
    ASSERT_EQUAL_INT(state.value2, 2);

    // No options mapped - nothing should happen
    Menu_runCommand(menu, MENUCODE_EXIT, NULL, NULL, &state);
    ASSERT_EQUAL_INT(state.value1, 3);
    ASSERT_EQUAL_INT(state.value2, 2);


    Menu_deconstruct(menu);
}



// void Menu_setCommand(Menu *self, int index, Menucode menucode) {}

int main(){ 
    EWENIT_START;

    ADD_CASE(testCreation);
    ADD_CASE(testCommands);
    EWENIT_END;
    return 0;
}
