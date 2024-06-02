/*========================================================================================
core.h

Core components for my janky testing framework EWENIT

Definitions/methods for structs:
    TestSuite
    _caseitem
    _assertionitem

Any methods/structs preceded with an underscore
(_caseitem, _assertionitem, and associated methods) should be considered private
and should probably never be directly called outside of this file.

========================================================================================*/

#ifndef INCLUDE_GUARD_CORE
#define INCLUDE_GUARD_CORE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>


#define STATUS_CODE_PASS    0
#define STATUS_CODE_FAIL    -1

#define PRINT_SINGLE_LINE   printf("------------------------------\n")
#define PRINT_DOUBLE_LINE   printf("==============================\n")
#define PRINT_UNDER_LINE    printf("______________________________\n")


/*----------------------------------------------------------------------------------------
STRUCT DEFINITIONS
----------------------------------------------------------------------------------------*/

// _assertionitem - individual assertion element - one per assertion per test case
typedef struct TestAssertion {
    int status_code;            // Integer representing status of an assertion
    unsigned int file_name_offset;            // File name assertion was called from
    int line_num;               // Row number assertion was called from
    unsigned int msg_offset;                  // Message to print for this assertion
} _assertionitem;

typedef struct TestCaseItem {
    bool is_committed;
    int num_pass;
    int num_fail;
    int num_tests;
    unsigned int name_offset;   // Offset to strlib pointer containing name

    size_t length;
    size_t _size;
    _assertionitem* assertions; // Now this is a dynamically allocated array
} _caseitem;


// TestSuite - head manager struct for tests
typedef struct {
    int total_pass;             // Grand total number of successes recorded
    int total_fail;             // Grand total number of failures recorded
    int total_tests;            // Grand total number of assertions ran

    int cases_pass;             // Number of test cases passed
    int cases_fail;             // Number of test cases failed
    int num_cases;              // Total number of _caseitem elements

    size_t strlib_length;   
    size_t strlib_size;
    char* strlib;      

    size_t length;              // length of cases
    size_t _size;                // Internal size of array allocated for cases
    _caseitem* cases;           // Array of _caseitem elements

} TestSuite;


/*----------------------------------------------------------------------------------------
INITIALIZATION
----------------------------------------------------------------------------------------*/

TestSuite* TestSuite_init();
void _caseitem_init(_caseitem* self, unsigned int name_offset);
void _assertionitem_init(
    _assertionitem* self, int status_code, unsigned int msg_offset,
    unsigned int file_name_offset, int line_num
);

/*----------------------------------------------------------------------------------------
DECONSTRUCTION
----------------------------------------------------------------------------------------*/

void _caseitem_deconstruct(_caseitem* self);
void TestSuite_deconstruct(TestSuite* self);

/*----------------------------------------------------------------------------------------
HANDLING STRLIB
----------------------------------------------------------------------------------------*/

void TestSuite_resizeStrlib(TestSuite* self, size_t target_size);
unsigned int TestSuite_addString(TestSuite* self, char* new_str);

/*----------------------------------------------------------------------------------------
PRINT METHODS
----------------------------------------------------------------------------------------*/

// Print summary of current case. Full output, but excludes successes
void _caseitem_print(_caseitem* self, char* strlib);
// Compact version of printing
void _caseitem_printCompact(_caseitem* self, char* strlib);

// Prints full test case report, including successes
void _caseitem_printVerbose(_caseitem* self, char* strlib);

// Default print method. Prints full test case report, but excludes successes
void TestSuite_print(TestSuite* self);

// Public method
void TestSuite_printCompact(TestSuite* self);

void TestSuite_printVerbose(TestSuite* self);

/*----------------------------------------------------------------------------------------
ASSERTION/TEST CASE MANAGEMENT
----------------------------------------------------------------------------------------*/

void _caseitem_resizeAssertions(_caseitem* self);
void _caseitem_addAssertion(
    _caseitem* self, int status_code, unsigned int msg_offset,
    unsigned int file_name_offset, int line_num
);
void TestSuite_resizeCases(TestSuite* self);
int TestSuite_commitCase(TestSuite* self);
int TestSuite_newCase(TestSuite* self, char* case_name);
int TestSuite_pass(TestSuite* self, char* file_name, long line_num, char* msg, ...);
int TestSuite_fail(TestSuite* self, char* file_name, long line_num, char* msg, ...);


#endif
