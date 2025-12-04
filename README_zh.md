# cunit

![CMake](https://img.shields.io/badge/CMake-3.14%2B-brightgreen?logo=cmake&logoColor=white)
[![Release](https://img.shields.io/github/v/release/tayne3/cunit?include_prereleases&label=release&logo=github&logoColor=white)](https://github.com/tayne3/cunit/releases)
[![Tag](https://img.shields.io/github/v/tag/tayne3/cunit?color=%23ff8936&style=flat-square&logo=git&logoColor=white)](https://github.com/tayne3/cunit/tags)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/tayne3/cunit)

[English](README.md) | **中文**

**现代化、轻量级的 C 语言单元测试框架。**

cunit 提供了清晰、结构化的 API 来编写和组织 C 语言单元测试，既支持现代的宏语法，也支持传统的函数调用，最大化灵活性。

## ✨ 核心特性

- **🏗️ 结构化 API**: 现代宏语法，使用 `CUNIT_SUITE_BEGIN`/`CUNIT_SUITE_END` 代码块
- **🚀 易于集成**: CMake 支持，兼容 FetchContent 和 CPM.cmake
- **🎯 ANSI C99**: 标准兼容代码，适用于所有平台

## 🚀 快速开始

### 基础测试结构

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

### 带有 Setup/Teardown 的高级示例

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

## 🔧 安装与集成

### 方法 1: CMake FetchContent

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

# 创建测试可执行文件
add_executable(my_tests test_main.c)
target_link_libraries(my_tests PRIVATE cunit::cunit)

# 启用测试
enable_testing()
add_test(NAME my_tests COMMAND my_tests)
```

### 方法 2: CPM.cmake

```cmake
include(cmake/CPM.cmake)
CPMAddPackage("gh:tayne3/cunit@v0.2.3")

add_executable(my_tests test_main.c)
target_link_libraries(my_tests PRIVATE cunit::cunit)
```

### 方法 3: 手动构建

```bash
git clone https://github.com/tayne3/cunit.git
cd cunit
mkdir build && cd build
cmake ..
make
```

## 📚 API 参考

### 核心函数

| 函数 | 描述 |
|------|------|
| `cunit_init()` | 初始化框架 |
| `cunit_cleanup()` | 清理资源 |
| `cunit_suite(name, setup, teardown)` | 创建测试套件 |
| `cunit_test(name, func)` | 向当前套件添加测试 |
| `cunit_run()` | 运行所有测试 |
| `cunit_run_suite(name)` | 运行指定套件 |

### 结构化 API（推荐）

| 宏 | 描述 |
|----|------|
| `CUNIT_SUITE_BEGIN(name, setup, teardown)` | 开始套件定义 |
| `CUNIT_TEST(name, func)` | 向当前套件添加测试 |
| `CUNIT_SUITE_END()` | 结束套件定义 |

### 查询函数

| 函数 | 描述 |
|------|------|
| `cunit_test_count()` | 获取测试总数 |
| `cunit_failure_count()` | 获取失败测试数 |
| `cunit_suite_count()` | 获取测试套件数 |

### 断言宏

#### 布尔断言

```c
assert_true(condition);
assert_false(condition);
assert_bool(expected, actual);
```

#### 整数断言

```c
assert_int_eq(expected, actual);   // 等于
assert_int_ne(expected, actual);   // 不等于
assert_int_lt(expected, actual);   // 小于
assert_int_gt(expected, actual);   // 大于
assert_int_le(expected, actual);   // 小于等于
assert_int_ge(expected, actual);   // 大于等于
```

#### 字符串断言

```c
assert_str_eq(expected, actual);     // 字符串相等
assert_str_ne(expected, actual);     // 字符串不等
assert_str_case(expected, actual);   // 忽略大小写比较
assert_str_n(expected, actual, n);   // 前 n 个字符比较
```

#### 指针断言

```c
assert_ptr_eq(expected, actual);
assert_ptr_ne(expected, actual);
assert_null(ptr);
assert_not_null(ptr);
```

#### 浮点数断言

```c
assert_float32_eq(expected, actual);
assert_float64_eq(expected, actual);
// 同样有: _ne, _lt, _gt, _le, _ge 变种
```
