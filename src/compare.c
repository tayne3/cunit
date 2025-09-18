#include "cunit/compare.h"

#include <stdarg.h>

#include "cunit/assert.h"

#ifdef _MSC_VER
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp
#endif

#define CUNIT_STRCMP(l, r)        (l == r ? 0 : !l ? -1 : !r ? 1 : strcmp(l, r))
#define CUNIT_STRCASECMP(l, r)    (l == r ? 0 : !l ? -1 : !r ? 1 : strcasecmp(l, r))
#define CUNIT_STRNCMP(l, r, size) (l == r ? 0 : !l ? -1 : !r ? 1 : strncmp(l, r, size))

#define CUNIT_NUMBER_COMPARE(l, r)  ((l > r) - (l < r))
#define CUNIT_FLOAT32_COMPARE(l, r) (isnan(l) ? isnan(r) ? 0 : -1 : isnan(r) ? 1 : (fabsf(l - r) <= FLT_EPSILON) ? 0 : (l > r) - (l < r))
#define CUNIT_FLOAT64_COMPARE(l, r) (isnan(l) ? isnan(r) ? 0 : -1 : isnan(r) ? 1 : (fabs(l - r) <= DBL_EPSILON) ? 0 : (l > r) - (l < r))

static inline void __cunit_print_bool(bool b) { fputs(b ? "true" : "false", stdout); }
static inline void __cunit_print_char(char c) { putchar(c); }
static inline void __cunit_print_f32(float f) { printf("%f", f); }
static inline void __cunit_print_f64(double f) { printf("%f", f); }
static inline void __cunit_print_str(const char *str) { fputs(str ? str : "(null)", stdout); }

static inline void __cunit_print_u64(uint64_t n) {
	// Buffer large enough for 20 digits of uint64_t + null terminator
	char buf[21];
	int  i = sizeof(buf) - 1;
	buf[i] = '\0';

	if (n == 0) {
		buf[--i] = '0';
	} else {
		while (n > 0) {
			buf[--i] = (n % 10) + '0';
			n /= 10;
		}
	}
	fputs(&buf[i], stdout);
}

static inline void __cunit_print_i64(int64_t n) {
	if (n >= 0) {
		__cunit_print_u64((uint64_t)n);
	} else {
		putchar('-');
		__cunit_print_u64((uint64_t)-n);
	}
}

static inline void __cunit_print_u32(uint32_t n) { __cunit_print_u64(n); }
static inline void __cunit_print_i32(int32_t n) { __cunit_print_i64(n); }
static inline void __cunit_print_u16(uint16_t n) { __cunit_print_u64(n); }
static inline void __cunit_print_i16(int16_t n) { __cunit_print_i64(n); }
static inline void __cunit_print_u8(uint8_t n) { __cunit_print_u64(n); }
static inline void __cunit_print_i8(int8_t n) { __cunit_print_i64(n); }

static inline void __cunit_print_ptr(const void *p) {
	if (p == NULL) {
		// Consistent with typical %p output for NULL
		fputs("(nil)", stdout);
		return;
	}
	// uintptr_t is an integer type wide enough to hold a pointer
	uintptr_t addr = (uintptr_t)p;
	// Buffer: "0x" + 2 hex chars per byte + null terminator
	char  buf[sizeof(uintptr_t) * 2 + 3];
	char *ptr = &buf[sizeof(buf) - 1];
	*ptr      = '\0';

	if (addr == 0) {
		*--ptr = '0';
	} else {
		const char hex_digits[] = "0123456789abcdef";
		while (addr > 0) {
			// Get the last 4 bits and convert to hex char
			*--ptr = hex_digits[addr & 0xF];
			// Shift right by 4 bits
			addr >>= 4;
		}
	}
	*--ptr = 'x';  // Add "0x" prefix
	*--ptr = '0';
	fputs(ptr, stdout);  // Print the resulting hex string
}

void __cunit_value_print(const cunit_value_t *self) {
	switch (self->type) {
		case CUnitType_Bool: __cunit_print_bool(self->d.b); break;
		case CUnitType_Char: __cunit_print_char(self->d.c); break;
		case CUnitType_Float32: __cunit_print_f32(self->d.f32); break;
		case CUnitType_Float64: __cunit_print_f64(self->d.f64); break;
		case CUnitType_String: __cunit_print_str(self->d.str); break;
		case CUnitType_Pointer: __cunit_print_ptr(self->d.ptr); break;
		case CUnitType_Int: __cunit_print_i32(self->d.i); break;
		case CUnitType_Int8: __cunit_print_i8(self->d.i8); break;
		case CUnitType_Int16: __cunit_print_i16(self->d.i16); break;
		case CUnitType_Int32: __cunit_print_i32(self->d.i32); break;
		case CUnitType_Int64: __cunit_print_i64(self->d.i64); break;
		case CUnitType_Uint: __cunit_print_u32(self->d.u); break;
		case CUnitType_Uint8: __cunit_print_u8(self->d.u8); break;
		case CUnitType_Uint16: __cunit_print_u16(self->d.u16); break;
		case CUnitType_Uint32: __cunit_print_u32(self->d.u32); break;
		case CUnitType_Uint64: __cunit_print_u64(self->d.u64); break;
		case CUnitType_Invalid:
		default: fputs("(invalid)", stdout); break;
	}
}

int __cunit_value_compare(const cunit_value_t *l, const cunit_value_t *r) {
	if (l->type != r->type) { return -2; }
	switch (l->type) {
		case CUnitType_Bool: return CUNIT_NUMBER_COMPARE(l->d.b, r->d.b);
		case CUnitType_Char: return CUNIT_NUMBER_COMPARE(l->d.c, r->d.c);
		case CUnitType_Float32: return CUNIT_FLOAT32_COMPARE(l->d.f32, r->d.f32);
		case CUnitType_Float64: return CUNIT_FLOAT64_COMPARE(l->d.f64, r->d.f64);
		case CUnitType_String: return CUNIT_STRCMP(l->d.str, r->d.str);
		case CUnitType_Pointer: return CUNIT_NUMBER_COMPARE(l->d.ptr, r->d.ptr);
		case CUnitType_Int: return CUNIT_NUMBER_COMPARE(l->d.i, r->d.i);
		case CUnitType_Int8: return CUNIT_NUMBER_COMPARE(l->d.i8, r->d.i8);
		case CUnitType_Int16: return CUNIT_NUMBER_COMPARE(l->d.i16, r->d.i16);
		case CUnitType_Int32: return CUNIT_NUMBER_COMPARE(l->d.i32, r->d.i32);
		case CUnitType_Int64: return CUNIT_NUMBER_COMPARE(l->d.i64, r->d.i64);
		case CUnitType_Uint: return CUNIT_NUMBER_COMPARE(l->d.u, r->d.u);
		case CUnitType_Uint8: return CUNIT_NUMBER_COMPARE(l->d.u8, r->d.u8);
		case CUnitType_Uint16: return CUNIT_NUMBER_COMPARE(l->d.u16, r->d.u16);
		case CUnitType_Uint32: return CUNIT_NUMBER_COMPARE(l->d.u32, r->d.u32);
		case CUnitType_Uint64: return CUNIT_NUMBER_COMPARE(l->d.u64, r->d.u64);
		case CUnitType_Invalid:
		default: return -2;
	}
}

// comparison results: greater than, less than, and equal to
enum cunit_compare_result {
	CUnitCompare_Unknown = -2,
	CUnitCompare_Less    = -1,
	CUnitCompare_Equal   = 0,
	CUnitCompare_Greater = 1,
};

#define CUNIT_COMPARE_RESULT_TO_STR(x) ((x) == CUnitCompare_Less ? "<" : (x) == CUnitCompare_Equal ? "=" : (x) == CUnitCompare_Greater ? ">" : "?")

#define __cunit_print_not_expected(ctx) \
	do { printf("\033[33;2m%s:%d\033[0m not expected: ", __cunit_relative(ctx.file), ctx.line); } while (0)

#define __cunit_print_info(ctx, format, ...)                                         \
	do {                                                                             \
		if (!STR_ISEMPTY(format)) {                                                  \
			printf("\033[37;2m%s:%d\033[0m ", __cunit_relative(ctx.file), ctx.line); \
			va_list args;                                                            \
			va_start(args, format);                                                  \
			vprintf(format, args);                                                   \
			va_end(args);                                                            \
			fputs(STR_NEWLINE, stdout);                                              \
		}                                                                            \
	} while (0)

static inline bool __cunit_check_any_is_in_array(const cunit_value_t value, const void *array, size_t size) {
	switch (value.type) {
		case CUnitType_Bool:
			for (size_t i = 0; i < size; i++) {
				if (((const bool *)array)[i] == value.d.b) { return true; }
			}
			return false;
		case CUnitType_Char:
			for (size_t i = 0; i < size; i++) {
				if (((const char *)array)[i] == value.d.c) { return true; }
			}
			return false;
		case CUnitType_Int:
			for (size_t i = 0; i < size; i++) {
				if (((const int *)array)[i] == value.d.i) { return true; }
			}
			return false;
		case CUnitType_Int8:
			for (size_t i = 0; i < size; i++) {
				if (((const int8_t *)array)[i] == value.d.i8) { return true; }
			}
			return false;
		case CUnitType_Int16:
			for (size_t i = 0; i < size; i++) {
				if (((const int16_t *)array)[i] == value.d.i16) { return true; }
			}
			return false;
		case CUnitType_Int32:
			for (size_t i = 0; i < size; i++) {
				if (((const int32_t *)array)[i] == value.d.i32) { return true; }
			}
			return false;
		case CUnitType_Int64:
			for (size_t i = 0; i < size; i++) {
				if (((const int64_t *)array)[i] == value.d.i64) { return true; }
			}
			return false;
		case CUnitType_Uint:
			for (size_t i = 0; i < size; i++) {
				if (((const unsigned int *)array)[i] == value.d.u) { return true; }
			}
			return false;
		case CUnitType_Uint8:
			for (size_t i = 0; i < size; i++) {
				if (((const uint8_t *)array)[i] == value.d.u8) { return true; }
			}
			return false;
		case CUnitType_Uint16:
			for (size_t i = 0; i < size; i++) {
				if (((const uint16_t *)array)[i] == value.d.u16) { return true; }
			}
			return false;
		case CUnitType_Uint32:
			for (size_t i = 0; i < size; i++) {
				if (((const uint32_t *)array)[i] == value.d.u32) { return true; }
			}
			return false;
		case CUnitType_Uint64:
			for (size_t i = 0; i < size; i++) {
				if (((const uint64_t *)array)[i] == value.d.u64) { return true; }
			}
			return false;
		case CUnitType_Float32:
			for (size_t i = 0; i < size; i++) {
				const float it = ((const float *)array)[i];
				if (isnan(it)) {
					if (isnan(value.d.f32)) { return true; }
					continue;
				}
				if (fabsf(it - value.d.f32) <= FLT_EPSILON) { return true; }
			}
			return false;
		case CUnitType_Float64:
			for (size_t i = 0; i < size; i++) {
				const double it = ((const double *)array)[i];
				if (isnan(it)) {
					if (isnan(value.d.f64)) { return true; }
					continue;
				}
				if (fabs(it - value.d.f64) <= DBL_EPSILON) { return true; }
			}
			return false;
		case CUnitType_String:
			for (size_t i = 0; i < size; i++) {
				if (((const char *const *)array)[i] == value.d.str || CUNIT_STRCMP(((const char *const *)array)[i], value.d.str) == 0) { return true; }
			}
			return false;
		case CUnitType_Pointer:
			for (size_t i = 0; i < size; i++) {
				if (((const void *const *)array)[i] == value.d.ptr) { return true; }
			}
			return false;
		default: return true;
	}
}

static inline void __cunit_print_hex(const uint8_t *array, size_t length) {
	if (!array) {
		fputs("(null)", stdout);
		return;
	}

	for (;;) {
		if (--length > 0) {
			printf("%02X ", *array++);
			continue;
		}
		if (length == 0) { printf("%02X", *array++); }
		break;
	}
}

#define __cunit_process_compare_result(result, cond, print_l, print_r, format) \
	do {                                                                       \
		switch (result) {                                                      \
			case CUnitCompare_Less:                                            \
				if (cond & CUnit_Less) { return true; }                        \
				break;                                                         \
			case CUnitCompare_Equal:                                           \
				if (cond & CUnit_Equal) { return true; }                       \
				break;                                                         \
			case CUnitCompare_Greater:                                         \
				if (cond & CUnit_Greater) { return true; }                     \
				break;                                                         \
			default: break;                                                    \
		}                                                                      \
		__cunit_print_not_expected(ctx);                                       \
		print_l;                                                               \
		fputs(" ", stdout);                                                    \
		fputs(CUNIT_COMPARE_RESULT_TO_STR(result), stdout);                    \
		fputs(" ", stdout);                                                    \
		print_r;                                                               \
		fputs(STR_NEWLINE, stdout);                                            \
		__cunit_print_info(ctx, format);                                       \
		return false;                                                          \
	} while (0)

bool __cunit_compare_bool(const cunit_context_t ctx, bool l, bool r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_bool(l), __cunit_print_bool(r), format);
}

bool __cunit_compare_char(const cunit_context_t ctx, char l, char r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_char(l), __cunit_print_char(r), format);
}

bool __cunit_compare_float(const cunit_context_t ctx, float l, float r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = CUNIT_FLOAT32_COMPARE(l, r);
	__cunit_process_compare_result(result, cond, __cunit_print_f32(l), __cunit_print_f32(r), format);
}

bool __cunit_compare_double(const cunit_context_t ctx, double l, double r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = CUNIT_FLOAT64_COMPARE(l, r);
	__cunit_process_compare_result(result, cond, __cunit_print_f64(l), __cunit_print_f64(r), format);
}

bool __cunit_compare_ptr(const cunit_context_t ctx, const void *l, const void *r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_ptr(l), __cunit_print_ptr(r), format);
}

bool __cunit_check_null(const cunit_context_t ctx, const void *p, const char *format, ...) {
	if (!p) { return true; }

	__cunit_print_not_expected(ctx);
	printf("%p is not null" STR_NEWLINE, p);
	__cunit_print_info(ctx, format);
	return false;
}

bool __cunit_check_not_null(const cunit_context_t ctx, const void *p, const char *format, ...) {
	if (p) { return true; }

	__cunit_print_not_expected(ctx);
	fputs("(null) is null" STR_NEWLINE, stdout);
	__cunit_print_info(ctx, format);
	return false;
}

bool __cunit_check_str(const cunit_context_t ctx, const char *l, const char *r, bool equal, const char *format, ...) {
	const bool is_str_equal = (l == r) || (l && r && !CUNIT_STRCMP(l, r));
	if (is_str_equal == equal) { return true; }

	__cunit_print_not_expected(ctx);
	printf("%s %s %s" STR_NEWLINE, l, equal ? "!=" : "==", r);
	__cunit_print_info(ctx, format);
	return false;
}

bool __cunit_check_str_n(const cunit_context_t ctx, const char *l, const char *r, size_t size, const char *format, ...) {
	if (l == r) { return true; }
	if (l && r && !CUNIT_STRNCMP(l, r, size)) { return true; }

	__cunit_print_not_expected(ctx);
	printf("%s != %s" STR_NEWLINE, l, r);
	__cunit_print_info(ctx, format);
	return false;
}

bool __cunit_check_str_case(const cunit_context_t ctx, const char *l, const char *r, const char *format, ...) {
	if (l == r) { return true; }
	if (l && r && !CUNIT_STRCASECMP(l, r)) { return true; }

	__cunit_print_not_expected(ctx);
	printf("%s != %s" STR_NEWLINE, l, r);
	__cunit_print_info(ctx, format);
	return false;
}

static inline int __cunit_bytearray_compare(const uint8_t *l, const uint8_t *r, size_t size) {
	for (size_t i = 0; i < size; i++) {
		if (l[i] != r[i]) { return (l[i] > r[i]) - (l[i] < r[i]); }
	}
	return 0;
}

bool __cunit_check_str_hex(const cunit_context_t ctx, const uint8_t *l, const uint8_t *r, size_t size, const char *format, ...) {
	if (l == r) { return true; }
	if (l && r && !__cunit_bytearray_compare(l, r, size)) { return true; }

	__cunit_print_not_expected(ctx);
	fputs("`", stdout);
	__cunit_print_hex(l, size);
	fputs("` != `", stdout);
	__cunit_print_hex(r, size);
	fputs("` " STR_NEWLINE, stdout);
	__cunit_print_info(ctx, format);
	return false;
}

bool __cunit_compare_int(const cunit_context_t ctx, int l, int r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_i64(l), __cunit_print_i64(r), format);
}

bool __cunit_compare_int8(const cunit_context_t ctx, int8_t l, int8_t r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_i64(l), __cunit_print_i64(r), format);
}

bool __cunit_compare_int16(const cunit_context_t ctx, int16_t l, int16_t r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_i64(l), __cunit_print_i64(r), format);
}

bool __cunit_compare_int32(const cunit_context_t ctx, int32_t l, int32_t r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_i64(l), __cunit_print_i64(r), format);
}

bool __cunit_compare_int64(const cunit_context_t ctx, int64_t l, int64_t r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_i64(l), __cunit_print_i64(r), format);
}

bool __cunit_compare_uint(const cunit_context_t ctx, unsigned l, unsigned r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_u64(l), __cunit_print_u64(r), format);
}

bool __cunit_compare_uint8(const cunit_context_t ctx, uint8_t l, uint8_t r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_u64(l), __cunit_print_u64(r), format);
}

bool __cunit_compare_uint16(const cunit_context_t ctx, uint16_t l, uint16_t r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_u64(l), __cunit_print_u64(r), format);
}

bool __cunit_compare_uint32(const cunit_context_t ctx, uint32_t l, uint32_t r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_u64(l), __cunit_print_u64(r), format);
}

bool __cunit_compare_uint64(const cunit_context_t ctx, uint64_t l, uint64_t r, int cond, const char *format, ...) {
	const enum cunit_compare_result result = (l > r) - (l < r);
	__cunit_process_compare_result(result, cond, __cunit_print_u64(l), __cunit_print_u64(r), format);
}

bool __cunit_check_any_in_array(const cunit_context_t ctx, const cunit_value_t value, const void *array, size_t size, const char *format, ...) {
	if (__cunit_check_any_is_in_array(value, array, size)) { return true; }

	__cunit_print_not_expected(ctx);
	__cunit_value_print(&value);
	fputs(" is not in array" STR_NEWLINE, stdout);
	__cunit_print_info(ctx, format);
	return false;
}

bool __cunit_check_any_not_in_array(const cunit_context_t ctx, const cunit_value_t value, const void *array, size_t size, const char *format, ...) {
	if (!__cunit_check_any_is_in_array(value, array, size)) { return true; }

	__cunit_print_not_expected(ctx);
	__cunit_value_print(&value);
	fputs(" is in array" STR_NEWLINE, stdout);
	__cunit_print_info(ctx, format);
	return false;
}
