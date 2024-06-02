# EWENIT v2.0.1
A basic unit testing framework for C.
## Usage
The basic flow of creating a test file using this framework is as below:
* [Include header](#include-header)
* [Define test suite](#define-test-suite)
* [Begin suite](#begin-suite)
* [Add cases](#add-cases)
* [End suite](#end-suite)

### Include header

```.c
/* test.c */
#include "EWENIT.h"
```

### Define test suite
A test suite exists in some *.c file, under main function. When built and run,
we want the tests to fire off by going through main
```c
/* test.c */
#include "EWENIT.h"


int main() {
    /* My test suite */
    return 0;
}
```

### Begin suite
Each test suite must be initialized by a call to macro `EWENIT_START` within `EWENIT.h`

```c
/* test.c */
#include "EWENIT.h"


int main() {
    /* My test suite */
    EWENIT_START;
    return 0;
}
```

### Add cases
Each test suite consists of test cases - functions with a void return type taking no arguments
that invoke one (or more) of [assertion macros](#assertions) defined in this library.
An "assertion" is a specific expectation about the behavior of the code being tested.
Each assertion passes or fails based on the state of the values passed,
deciding if that expectation was met or not. These will be reported on when the test suite is run.
```c
/* test.c */
#include "EWENIT.h"
void test1() {
    ASSERT_EQUAL_INT(1, 1); // passes
}

void test2() {
    ASSERT_FALSE(true); // fails
}

int main() {
    /* My test suite */
    EWENIT_START;
    ADD_CASE(test1);
    ADD_CASE(test2);
    return 0;
}
```

### End suite
Each suite ends with a specific call to an ending macro `EWENIT_END` or an alternative
(more listed [below](#ending)). This call runs each test case and reports out on the
overall status of the suite.

```c
/* test.c */
#include "EWENIT.h"
void test1() {
    ASSERT_EQUAL_INT(1, 1); // passes
}

void test2() {
    ASSERT_FALSE(true); // fails
}

int main() {
    /* My test suite */
    EWENIT_START;
    ADD_CASE(test1);
    ADD_CASE(test2);
    EWENIT_END;
    return 0;
}
```


## An Example
### The code
```c
/* example_test.c */

#include "EWENIT.h" 

// =======================================
 /* NOTE: In an actual application, these
  * will likely be imported from elsewhere
  */
int intFunc() {
    return 4;
}

char* strFunc() {
    return "Good result";
}

// =======================================
// Test cases
void test1() {
    int intResult = intFunc();
    ASSERT_EQUAL_INT(intResult, 4); // succeeds
}

void test2() {
    char* strResult = strFunc();
    ASSERT_NOT_EQUAL_STR(strResult, "Bad result"); // succeeds
}

void test3() {
    int intResult = intFunc();
    ASSERT_GREATER_THAN_INT(intResult, 10); // fails
}

void test4() {
    char* strResult = strFunc();
    ASSERT_EQUAL_STR(strResult, "Another bad one"); // fails
}

// =======================================
// Run suite

int main() {

    // Initialize test suite
    EWENIT_START;

    // Add tests cases - each of which includes one (or more)
    // assertions
    ADD_CASE(test1);
    ADD_CASE_CUSTOM(test2, "String Test");
    ADD_CASE(test3);
    ADD_CASE(test4);

    // End
    EWENIT_END_VERBOSE;

    // alternatives
    // EWENIT_END_VERBOSE;
    // EWENIT_END_COMPACT;
}

```
### The output

With `EWENIT_END`

```bash
==============================
test3
    readme_example.c
    [29] Fail: 4 <= 10
------------------------------
[0P] [1F] Total: 1
==============================
test4
    readme_example.c
    [34] Fail: String lengths differ: 11 vs 15
      Good result
      Another
------------------------------
[1P] [1F] Total: 2
==============================
Assertions
[3P] [2F] Total: 5

Test Cases
[2P] [2F] Total: 4
______________________________
```

With `EWENIT_END_VERBOSE`
```bash

==============================
test1
    readme_example.c
    [19] Success: 4 == 4
------------------------------
[1P] [0F] Total: 1
==============================
String Test
    readme_example.c
    [24] Success: Strings differ
      Good result
      Bad result
------------------------------
[1P] [0F] Total: 1
==============================
test3
    readme_example.c
    [29] Fail: 4 <= 10
------------------------------
[0P] [1F] Total: 1
==============================
test4
    readme_example.c
    [34] Fail: String lengths differ: 11 vs 15
      Good result
      Another
    [35] Success: Strings differ
      Good result
      Bad one again
------------------------------
[1P] [1F] Total: 2
==============================
Assertions
[3P] [2F] Total: 5

Test Cases
[2P] [2F] Total: 4

```

With `EWENIT_END_COMPACT`
```bash
==============================
test1: Passed [P]
String Test: Passed [P]
test3: Failed [F]
test4: Failed [FP]
==============================
Assertions [3P] [2F] Total: 5
Test Cases [2P] [2F] Total: 4
```

## EWENIT Macros

### Initialization
##### `EWENIT_START`
Begins and prepares the test suite. Must be called before any 
assertions can be made.
##### `TEST_START`
An alias for `EWENIT_START`
***
### Ending
Each ending macro ends the test suite - it must be called after `EWENIT_START` (or equivalent) and after some test cases have been added. Note that there are several options, but only one may be used per suite.
##### `EWENIT_END`
Finalize, commit, run, and report on the current test suite. Prints a status report out of each test, showing only those tests with failed assertions (and information about those assertions.
##### `TEST_END`
An alias for EWENIT_END.
##### `EWENIT_END_COMPACT`
Commit & run the current test suite, reporting a condensed output of test successes & failures. 
##### `TEST_END_COMPACT`
An alias for `EWENIT_END_COMPACT`
##### `EWENIT_END_VERBOSE`
Commit & run the test suite, reporting a more detailed summary of each test case. Formatted just like `EWENIT_END`, but includes a report-out on each success as well.
##### `TEST_END_VERBOSE`
An alias for `EWENIT_END_VERBOSE`
***
### Test case management
##### `ADD_CASE(func)`
Adds `func` to the test suite. The name used when reporting out
on test status will be the name of `func` itself. `func` should have a void return value and take no arguments, e.g.
```c
void func() {
    /* test code here 
}
```
```c
ADD_CASE(func)
```
##### `ADD_CASE_CUSTOM(func, name)`
Adds `func` to the test suite, giving it a custom `name` to use when reporting on status on test suite execution.
```c
ADD_CASE(func, "Nicer name for func");
```
***
### Assertions
* For assertions with arguments `val1` and `val2`, checks that the relationship described holds true for that respective assertion's type (int, str, float, double, etc.)
* For boolean assertions `ASSERT_TRUE(val)` and `ASSERT_FALSE(val)`, simply checks that `val` is appropriately `true` or `false`.
* For assertions that imply a significance to argument order (greater than, less than, etc.),  `val1` is taken as the "first" value for the comparison, and `val2` the "second". e.g. ASSERT_GREATER_THAN_INT(`val1`, `val2`) checks that `val1` > `val2`.


#### Integers
##### `ASSERT_EQUAL_INT(val1, val2)`
##### `ASSERT_NOT_EQUAL_INT(val1, val2)`
##### `ASSERT_GREATER_THAN_INT(val1, val2)`
#### Booleans
##### `ASSERT_TRUE(val)`
##### `ASSERT_FALSE(val)`
#### Strings
##### `ASSERT_EQUAL_STR(val1, val2)`
##### `ASSERT_NOT_EQUAL_STR(val1, val2)`
#### Floats
##### `ASSERT_EQUAL_FLOAT(val1, val2)`
##### `ASSERT_NOT_EQUAL_FLOAT(val1, val2)`
##### `ASSERT_GREATER_THAN_FLOAT(val1, val2)`
##### `ASSSERT_ALMOST_EQUAL_FLOAT(val1, val2)`
##### `ASSERT_NOT_ALMOST_EQUAL_FLOAT(val1, val2)`
#### Doubles
##### `ASSERT_EQUAL_DOUBLE(val1, val2)`
##### `ASSERT_NOT_EQUAL_DOUBLE(val1, val2)`
##### `ASSERT_GREATER_THAN_DOUBLE(val1, val2)`
##### `ASSERT_ALMOST_EQUAL_DOUBLE(val1, val2)`
##### `ASSERT_NOT_ALMOST_EQUAL_DOUBLE(val1, val2)`
#### Manual assertions
##### `TEST_PASS(msg)`
Manually log a success with the provided message.
##### `TEST_PASS_FMT(msg, ...)`
Manually log a success with the provided message with support for format arguments (%d, %f, %s, etc.)


