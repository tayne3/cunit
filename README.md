# cunit

![CMake](https://img.shields.io/badge/CMake-3.14%2B-brightgreen?logo=cmake&logoColor=white)
[![Release](https://img.shields.io/github/v/release/tayne3/cunit?include_prereleases&label=release&logo=github&logoColor=white)](https://github.com/tayne3/cunit/releases)
[![Tag](https://img.shields.io/github/v/tag/tayne3/cunit?color=%23ff8936&style=flat-square&logo=git&logoColor=white)](https://github.com/tayne3/cunit/tags)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/tayne3/cunit)

**English** | [中文](README_zh.md)

**A modern, lightweight, and professional C unit testing framework designed for simplicity and elegance.**

cunit provides a clean, structured API for writing and organizing unit tests in C, with both modern macro-based syntax and traditional function calls for maximum flexibility.

## ✨ Key Features

- **🏗️ Structured API**: Modern macro-based syntax with `CUNIT_SUITE_BEGIN`/`CUNIT_SUITE_END` blocks
- **🚀 Easy Integration**: CMake support with FetchContent and CPM.cmake
- **🎯 ANSI C99**: Standard-compliant code works everywhere

## 🚀 Quick Start

### Basic Test Structure

```c
#include "cunit.h"

void test_math_operations(void) {
    assert_int_eq(2 + 2, 4);
    assert_true(5 > 3);
    assert_false(1 > 2);
}

void test_string_operations(void) {
    assert_str_eq("hello", "hello");
    assert_str_ne("world", "hello");
}

int main(void) {
    cunit_init();

    CUNIT_SUITE_BEGIN("Math Tests", NULL, NULL)
    CUNIT_TEST("Basic Operations", test_math_operations)
    CUNIT_SUITE_END()

    CUNIT_SUITE_BEGIN("String Tests", NULL, NULL)
    CUNIT_TEST("String Comparisons", test_string_operations)
    CUNIT_SUITE_END()

    return cunit_run();
}
```

### Advanced Example with Setup/Teardown

```c
#include "cunit.h"
#include <stdlib.h>

static int *test_array;
static size_t array_size;

void setup_array_tests(void) {
    array_size = 5;
    test_array = malloc(array_size * sizeof(int));
    for (size_t i = 0; i < array_size; i++) {
        test_array[i] = (int)i * 2;
    }
}

void teardown_array_tests(void) {
    free(test_array);
    test_array = NULL;
}

void test_array_access(void) {
    assert_not_null(test_array);
    assert_int_eq(test_array[0], 0);
    assert_int_eq(test_array[2], 4);
}

void test_array_bounds(void) {
    assert_int_eq(array_size, 5);
    assert_int_eq(test_array[array_size - 1], 8);
}

int main(void) {
    cunit_init();

    CUNIT_SUITE_BEGIN("Array Tests", setup_array_tests, teardown_array_tests)
    CUNIT_TEST("Array Access", test_array_access)
    CUNIT_TEST("Array Bounds", test_array_bounds)
    CUNIT_SUITE_END()

    return cunit_run();
}
```

## 🔧 Installation & Integration

### Method 1: CMake FetchContent

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyProject)

include(FetchContent)
FetchContent_Declare(
    cunit
    GIT_REPOSITORY https://github.com/tayne3/cunit.git
    GIT_TAG v0.2.3
)
FetchContent_MakeAvailable(cunit)

# Create your test executable
add_executable(my_tests test_main.c)
target_link_libraries(my_tests PRIVATE cunit::cunit)

# Enable testing
enable_testing()
add_test(NAME my_tests COMMAND my_tests)
```

### Method 2: CPM.cmake

```cmake
include(cmake/CPM.cmake)
CPMAddPackage("gh:tayne3/cunit@v0.2.3")

add_executable(my_tests test_main.c)
target_link_libraries(my_tests PRIVATE cunit::cunit)
```

### Method 3: Manual Build

```bash
git clone https://github.com/tayne3/cunit.git
cd cunit
mkdir build && cd build
cmake ..
make
```

## 📚 API Reference

### Core Functions

| Function | Description |
|----------|-------------|
| `cunit_init()` | Initialize the framework |
| `cunit_cleanup()` | Clean up resources |
| `cunit_suite(name, setup, teardown)` | Create a test suite |
| `cunit_test(name, func)` | Add a test to current suite |
| `cunit_run()` | Run all tests |
| `cunit_run_suite(name)` | Run specific suite |

### Structured API (Recommended)

| Macro | Description |
|-------|-------------|
| `CUNIT_SUITE_BEGIN(name, setup, teardown)` | Begin suite definition |
| `CUNIT_TEST(name, func)` | Add test to current suite |
| `CUNIT_SUITE_END()` | End suite definition |

### Query Functions

| Function | Description |
|----------|-------------|
| `cunit_test_count()` | Get total number of tests |
| `cunit_failure_count()` | Get number of failed tests |
| `cunit_suite_count()` | Get number of test suites |

### Assertion Macros

#### Boolean Assertions

```c
assert_true(condition);
assert_false(condition);
assert_bool(expected, actual);
```

#### Integer Assertions  

```c
assert_int_eq(expected, actual);
assert_int_ne(expected, actual);
assert_int_lt(expected, actual);
assert_int_gt(expected, actual);
assert_int_le(expected, actual);
assert_int_ge(expected, actual);
```

#### String Assertions

```c
assert_str_eq(expected, actual);
assert_str_ne(expected, actual);
assert_str_case(expected, actual);  // Case-insensitive
assert_str_n(expected, actual, n);  // First n characters
```

#### Pointer Assertions

```c
assert_ptr_eq(expected, actual);
assert_ptr_ne(expected, actual);
assert_null(ptr);
assert_not_null(ptr);
```

#### Float Assertions

```c
assert_float32_eq(expected, actual);
assert_float64_eq(expected, actual);
// Also: _ne, _lt, _gt, _le, _ge variants
```
