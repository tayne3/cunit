/*
 * MIT License
 *
 * Copyright (c) 2025 tayne3
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 * 2. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *    SOFTWARE.
 */
#ifndef CUNIT_DEF_H
#define CUNIT_DEF_H

#ifdef __cplusplus
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#else
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

#if _WIN32
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// clang-format off

// newline
# ifndef STR_NEWLINE
#   ifdef _MSC_VER
#       define STR_NEWLINE      "\r\n"
#   else
#       define STR_NEWLINE      "\n"
#   endif
# endif
// empty string
# ifndef STR_NULL
#   define STR_NULL             ""
# endif
// string is empty
# ifndef STR_ISEMPTY
#   define STR_ISEMPTY(_s) 	    (!(_s) || !*(const char *)(_s))
# endif

# ifndef __GNUC_PREREQ
# 	define __GNUC_PREREQ(a, b)	0
# endif
	
# if defined __cplusplus ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#   define __cunit_func__		__extension__ __PRETTY_FUNCTION__
#	define __cunit_file__		__FILE__
#	define __cunit_line__		__LINE__
# elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define __cunit_func__		__func__
#	define __cunit_file__      __FILE__
#	define __cunit_line__      __LINE__
# elif defined(__GNUC__)
#	define __cunit_func__      __FUNCTION__
#	define __cunit_file__      __FILE__
#	define __cunit_line__      __LINE__
# elif defined(_MSC_VER)
#	define __cunit_func__      __FUNCTION__
#	define __cunit_file__      __FILE__
#	define __cunit_line__      __LINE__
# elif defined(__TINYC__)
#	define __cunit_func__      __func__
#	define __cunit_file__      __FILE__
#	define __cunit_line__      __LINE__
# else
#	define __cunit_func__      "(nil)"
#	define __cunit_file__      "(nil)"
#	define __cunit_line__      0
# endif

// clang-format on

#endif  // CUNIT_DEF_H
