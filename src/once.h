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
#ifndef CUNIT_ONCE_H
#define CUNIT_ONCE_H

#include "cunit/def.h"

#ifdef _WIN32
#ifdef __cplusplus
extern "C" {
#endif

typedef INIT_ONCE cunit_once_flag_t;
typedef void (*cunit_once_routine_t)(void);
#define CUNIT_ONCE_FLAG_INIT INIT_ONCE_STATIC_INIT

static inline BOOL CALLBACK __cunit_once_callback(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context) {
	(void)InitOnce;
	(void)Context;
	cunit_once_routine_t routine = (cunit_once_routine_t)Parameter;
	routine();
	return TRUE;
}

static inline void cunit_call_once(cunit_once_flag_t *flag, cunit_once_routine_t routine) {
	InitOnceExecuteOnce(flag, __cunit_once_callback, (PVOID)routine, NULL);
}

#ifdef __cplusplus
}
#endif
#else
#include <pthread.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef pthread_once_t cunit_once_flag_t;
typedef void (*cunit_once_routine_t)(void);
#define CUNIT_ONCE_FLAG_INIT PTHREAD_ONCE_INIT

static inline void cunit_call_once(cunit_once_flag_t *flag, cunit_once_routine_t routine) { pthread_once(flag, routine); }

#ifdef __cplusplus
}
#endif
#endif

#endif  // CUNIT_ONCE_H
