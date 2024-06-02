/*========================================================================================
core.c

Implemention of things defined in core.h
========================================================================================*/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "core.h"
#include "assertion_constants.h"



/*----------------------------------------------------------------------------------------
INITIALIZATION
----------------------------------------------------------------------------------------*/


TestSuite* TestSuite_init()
{
    TestSuite* self = (TestSuite*)malloc(sizeof(TestSuite));
    self->total_pass = 0;
    self->total_fail = 0;
    self->total_tests = 0;

    self->cases_pass = 0;
    self->cases_fail = 0;
    self->num_cases = 0;

    self->strlib_length = 0;
    self->strlib_size = 16;
    self->strlib = (char*)malloc(16 * sizeof(char));

    self->length = 0;
    self->_size = 4;
    self->cases = (_caseitem*)malloc(4 * sizeof(_caseitem));

    return self;
}

// Initializes an already allocated _caseitem
void _caseitem_init(_caseitem* self, unsigned int name_offset)
{
    self->is_committed = false;
    self->num_pass = 0;
    self->num_fail = 0;
    self->num_tests = 0;
    self->name_offset = name_offset;

    self->length = 0;
    self->_size = 4;
    self->assertions = (_assertionitem*)malloc(4 * sizeof(_assertionitem));
}


// Initializes an already allocated _assertionitem
void _assertionitem_init(
    _assertionitem* self, int status_code, unsigned int msg_offset,
    unsigned int file_name_offset, int line_num
)
{
    self->status_code = status_code;
    self->msg_offset = msg_offset;
    self->file_name_offset = file_name_offset;
    self->line_num = line_num;

}


/*----------------------------------------------------------------------------------------
DECONSTRUCTION
----------------------------------------------------------------------------------------*/

void _caseitem_deconstruct(_caseitem* self)
{
    if (self == NULL)
        return;

    // Only thing malloc'd for this
    free(self->assertions);
}

// Public method
void TestSuite_deconstruct(TestSuite* self)
{
    if (self == NULL)
        return;

    for (int i = 0; i < self->length; i++)
    {
        _caseitem_deconstruct((_caseitem*)(self->cases + i));
    }

    free(self->strlib);
    free(self->cases);
    free(self);
}


/*----------------------------------------------------------------------------------------
HANDLING STRLIB
----------------------------------------------------------------------------------------*/

void TestSuite_resizeStrlib(TestSuite* self, size_t target_size)
{
    while (target_size > self->strlib_size)
    {
        self->strlib_size *= 2;
    }
    self->strlib = (char*)realloc(self->strlib, self->strlib_size * sizeof(char));
}


// Add a string to TestSuite's string library
// Returns offset to strlib pointer where string is now located
unsigned int TestSuite_addString(TestSuite* self, char* new_str)
{
    size_t num_new_chars;
    size_t target_size;
    unsigned int start_length = self->strlib_length;

    num_new_chars = strlen(new_str) + 1;

    // Resize our strlib buffer ifneeded
    target_size = num_new_chars + self->strlib_length;
    if (target_size > self->strlib_size) {
        TestSuite_resizeStrlib(self, target_size);
    }

    strcpy((char*)(self->strlib + self->strlib_length), new_str);
    self->strlib_length += num_new_chars;

    return start_length;
}


/*----------------------------------------------------------------------------------------
PRINT METHODS
----------------------------------------------------------------------------------------*/


// Print summary of current case. Full output, but excludes successes
void _caseitem_print(_caseitem* self, char* strlib)
{
    if (self->num_tests == 0 || !self->is_committed)
        return;

    char* case_name;
    char* file_name;
    char* last_file_name = "";
    _assertionitem* ass;

    // for convenience
    case_name = (char*)(strlib + self->name_offset);

    PRINT_DOUBLE_LINE;
    printf("%s\n", case_name);

    for (int i = 0; i < self->length; i++)
    {
/*         ass = (_assertionitem*)(self->assertions + i - 1); */
        ass = (_assertionitem*)(self->assertions + i);
        // Regular print - don't report on successes
        if (ass->status_code != STATUS_CODE_FAIL) {
            continue;
        }

        file_name = (char*)(strlib + ass->file_name_offset);
        if (strcmp(file_name, last_file_name) != 0)
            printf("    %s\n", file_name);
        
        last_file_name = file_name;

        printf("    [%d] ", ass->line_num);
        printf("Fail");
        printf(": %s\n", (strlib + ass->msg_offset));
    }   

    PRINT_SINGLE_LINE;
    printf("[%dP] ", self->num_pass);
    printf("[%dF] ", self->num_fail);
    printf("Total: %d\n", self->num_tests);
}



// Compact version of printing
void _caseitem_printCompact(_caseitem* self, char* strlib)
{
    _assertionitem* ass;

    if (self->num_tests == 0 || !self->is_committed)
        return;


    printf("%s: ", strlib + self->name_offset);

    // Print overall status for _caseitem
    if (self->num_fail > 0)
        printf("Failed ");
    else
        printf("Passed ");

    printf("[");
    for (int i = 0; i < self->num_tests; i++)
    {
        ass =  (self->assertions + i);
        switch (ass->status_code)
        {
            case STATUS_CODE_PASS:
                printf("P");
                break;
            case STATUS_CODE_FAIL:
                printf("F");
                break;
            default:
                break;
        }
    }
    printf("]");
}

void _caseitem_printVerbose(_caseitem* self, char* strlib)
{
    if (self->num_tests == 0 || !self->is_committed)
        return;


    char* file_name;
    char* last_file_name = "";
    _assertionitem* ass;

    // for convenience
    char* case_name = (char*)(strlib + self->name_offset);

    PRINT_DOUBLE_LINE;
    printf("%s\n", case_name);

    for (int i = 0; i < self->length; i++)
    {
        ass = (_assertionitem*)(self->assertions + i);

        file_name = (char*)(strlib + ass->file_name_offset);
        if (strcmp(file_name, last_file_name) != 0)
            printf("    %s\n", file_name);

        last_file_name = file_name;
        printf("    [%d] ", ass->line_num);
        // Regular print - don't report on successes
        switch (ass->status_code)
        {
            case STATUS_CODE_PASS:
                printf("Success");
                break;
            case STATUS_CODE_FAIL:
                printf("Fail");
                break;
        }
        printf(": %s\n", (strlib + ass->msg_offset));
    }

    PRINT_SINGLE_LINE;
    printf("[%dP] ", self->num_pass);
    printf("[%dF] ", self->num_fail);
    printf("Total: %d\n", self->num_tests);

}

// Default print method. Prints full test case report, but excludes successes
void TestSuite_print(TestSuite* self)
{

    _caseitem* current_case;
    
    for (int i = 0; i < self->length; i++)
    {
        current_case = (_caseitem*)(self->cases + i);
        if (current_case->num_fail == 0)
            continue;

        _caseitem_print(self->cases + i, self->strlib);
    }

    PRINT_DOUBLE_LINE;
    printf("Assertions\n");
    printf("[%dP] [%dF] Total: %d", self->total_pass, self->total_fail, self->total_tests);
    printf("\n\nTest Cases\n");
    printf("[%dP] [%dF] Total: %d", self->cases_pass, self->cases_fail, self->num_cases);
    printf("\n");
    PRINT_UNDER_LINE;

}

// Public method
void TestSuite_printCompact(TestSuite* self)
{
    PRINT_DOUBLE_LINE;
    for (int i = 0; i < self->length; i++)
    {
        _caseitem_printCompact(self->cases + i, self->strlib);
        printf("\n");
    }

    PRINT_DOUBLE_LINE;
    printf("Assertions ");
    printf("[%dP] [%dF] Total: %d\n", self->total_pass, self->total_fail, self->total_tests);
    printf("Test Cases ");
    printf("[%dP] [%dF] Total: %d\n", self->cases_pass, self->cases_fail, self->num_cases);
}

void TestSuite_printVerbose(TestSuite* self)
{
    _caseitem* current_case;

    for (int i = 0; i < self->length; i++)
    {
        current_case = (_caseitem*)(self->cases + i);
        _caseitem_printVerbose(current_case, self->strlib);
    }

    PRINT_DOUBLE_LINE;
    printf("Assertions\n");
    printf("[%dP] [%dF] Total: %d", self->total_pass, self->total_fail, self->total_tests);
    printf("\n\nTest Cases\n");
    printf("[%dP] [%dF] Total: %d", self->cases_pass, self->cases_fail, self->num_cases);
    printf("\n");
}

/*----------------------------------------------------------------------------------------
ASSERTION/TEST CASE MANAGEMENT
----------------------------------------------------------------------------------------*/


void _caseitem_resizeAssertions(_caseitem* self)
{
    while (self->length >= self->_size)
    {
        self->_size *= 2;
    }
    self->assertions = (_assertionitem*)realloc(
        self->assertions, self->_size * sizeof(_assertionitem)
    );
}


// Addes a new assertion to a provided _caseitem pointer
void _caseitem_addAssertion(
    _caseitem* self, int status_code, unsigned int msg_offset,
    unsigned int file_name_offset, int line_num
)
{
    if (self->length >= self->_size)
        _caseitem_resizeAssertions(self);

    _assertionitem_init(
        (_assertionitem*)(self->assertions + self->length), status_code, msg_offset,
        file_name_offset, line_num
    );
    self->length++;
}

// Resize a TestSuite's internal case storage array to fit more elements
void TestSuite_resizeCases(TestSuite* self)
{
    while (self->length >= self->_size)
    {
        self->_size *= 2;
    }
    self->cases = (_caseitem*)realloc(self->cases, self->_size * sizeof(_caseitem));
}


// A _caseitem is considered incomplete until it has been committed. This method commits
// it, adding success/fail/total counts to TestSuite and flagging it as considered
int TestSuite_commitCase(TestSuite* self)
{
    if (self->length == 0)
        return -1;

    _caseitem* current = (_caseitem*)(self->cases + self->length - 1);

    if (current->is_committed)
        return -1;

    self->total_pass += current->num_pass;
    self->total_fail += current->num_fail;
    self->total_tests += current->num_tests;

    self->num_cases++;

    // Determine if the case as a whole succeeded or failed
    if (current->num_fail > 0)
        self->cases_fail++;
    else    
        self->cases_pass++;

    current->is_committed = true;
    return 0;
}

// Adds a new test case. First commits the old running one at tail _caseitem, then
// appends a brand new one to the right side
int TestSuite_newCase(TestSuite* self, char* case_name)
{
    // New case - commit the old one
    TestSuite_commitCase(self);

    // Add case_name to string library and remember offset to point to
    unsigned int case_name_offset = TestSuite_addString(self, case_name);

    if (self->length >= self->_size)
        TestSuite_resizeCases(self);

    _caseitem_init((_caseitem*)(self->cases + self->length), case_name_offset);
    self->length++;

    return 0;
}

// Call to indicate an assertion passed on the current running _caseitem
// Public method
int TestSuite_pass(TestSuite* self, char* file_name, long line_num, char* msg, ...)
{
    if (self->length == 0)
        return -1;

    unsigned int msg_offset;
    unsigned int file_name_offset;
    _caseitem* current_case;

    /* TestSuite can only add a pre-formatted string, so format it here */
    va_list arg_list;
    va_start(arg_list, msg);

    char fmtMsg[MSG_BUFF_SIZE];
    vsnprintf(fmtMsg, MSG_BUFF_SIZE, msg, arg_list);

    va_end(arg_list);
    /* -- */

    msg_offset = TestSuite_addString(self, fmtMsg);
    file_name_offset = TestSuite_addString(self, file_name);

    current_case = (_caseitem*)(self->cases + self->length - 1);
    _caseitem_addAssertion(
        current_case, STATUS_CODE_PASS, msg_offset, file_name_offset, line_num 
    );

    current_case->num_pass++;
    current_case->num_tests++;

    return 0;
}


// Call to indicate an assertion failed on the current running _caseitem
// Public method
int TestSuite_fail(TestSuite* self, char* file_name, long line_num, char* msg, ...)
{
    if (self->length == 0)
        return -1;

    va_list arg_list;
    unsigned int msg_offset;
    unsigned int file_name_offset;
    _caseitem* current_case;

    /* TestSuite can only add a pre-formatted string, so format it here */
    va_start(arg_list, msg);

    char fmtMsg[MSG_BUFF_SIZE];
    vsnprintf(fmtMsg, MSG_BUFF_SIZE, msg, arg_list);

    va_end(arg_list);
    /* -- */

    msg_offset = TestSuite_addString(self, fmtMsg);
    file_name_offset = TestSuite_addString(self, file_name);

    current_case = (_caseitem*)(self->cases + self->length - 1);

    _caseitem_addAssertion(
        current_case, STATUS_CODE_FAIL, msg_offset, file_name_offset, line_num 
    );

    current_case->num_fail++;
    current_case->num_tests++;

    return 0;
}
