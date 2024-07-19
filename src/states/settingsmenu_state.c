
#include "state_runner.h"

/******************************************************************************
 * menufunc predeclarations
 *
 * NOTE: These may otherwise go in a header file, but these need not be shared
 * with any other file, I'm choosing to define them here
 *
******************************************************************************/

void menufunc_exitSettings(
    StateRunner *state_runner, void *app_data,
    void *menu_data
);


/******************************************************************************
 * State running
******************************************************************************/


int SettingsMenuState_run(
    StateRunner *state_runner, void *application_data, void *state_data
) {

    return 0;
}


