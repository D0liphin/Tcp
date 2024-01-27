#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>

/**
 * Contains compile-time accessible type information. Construct with `TYPEINFO`
 * macro e.g.
 * 
 * ```c
 * TYPEINFO(int)
 * ```
 */
struct type {
        size_t size;
};

/**
 * Contruct a new `struct type` with a given `size`.
 */
struct type type_new(size_t size);

#define TYPEINFO(T) type_new(sizeof(T))

/**
 * A dynamic, exponentially growing array. Each array has an associate type, for
 * which the array is 'valid'. The user must keep track of this type for a 
 * given array, e.g. through hungarian notation. 
 * 
 * This implmentation aims to be just as good as other templated implementations
 * in regards to codegen, even on `-Oz`. 
 * 
 * Methods require const type information to be passed as a parameter. Since the 
 * API is designed with the expectation that this type information is passed as 
 * a constant, codegen is only evaluated for that case. 
 * 
 * # Example
 * 
 * The following example demonstrates some basic usage of a dynamic array
 * 
 * ```c
 * {
 *      struct dynarray arr = dynarray_new();
 *      int values[10] = { 1, 2, 4, 4, 5, 6, 7, 8, 9, 10 };
 *      dynarray_extend(&arr, &values[0], &values[10]);
 *      for (int *it = dynarray_begin(&arr); it != dynarray_end(&arr); ++it) {
 *              printf("%d ", *it);
 *      }
 *      puts("");
 *      for (size_t _ = 0; _ < 10; ++_) {
 *              int n = *(int *)dynarray_pop(&arr, TYPEINFO(int));
 *              printf("%d ", n);
 *      }
 *      puts("");
 *      dynarray_free(&arr);
 * }
 * ```
 * 
 * This outputs
 * 
 * ```
 * 1 2 4 4 5 6 7 8 9 10 
 * 10 9 8 7 6 5 4 4 2 1 
 * ```
 */
struct dynarray {
        void *data;
        /** capacity in bytes */
        size_t cap;
        /** length in bytes */
        size_t len;
};

#define DYN_ARRAY_MIN_CAP 4

/**
 * Initialize an empty `dynarray`. This does not allocate.
 */
struct dynarray dynarray_new();

/**
 * Return a pointer to the start of this array. This pointer is valid for casts
 * to a pointer to the inner type. And the resulting pointer is valid for reads
 * and writes of the inner type.
 */
void *dynarray_begin(struct dynarray *self);

/**
 * Acquire a pointer to the end of this array. This pointer is valid for casts
 * to a pointer to the inner type. However, this pointer may not be valid for
 * reads or writes. You should just assume that it is not writeable.
 *
 * If you wish to modify the next unoccupied cell in this array, you can use
 * `dynarray_next()` for a safer version.
 */
void *dynarray_end(struct dynarray *self);

/**
 * Exponentially resize this dynamic array. This only produces unrecoverable
 * errors.
 * 
 * This is ever so slightly faster than `dynarray_resize_to_fit()`. So I can 
 * justify having it here. 
 */
void dynarray_resize(struct dynarray *self, struct type val_type);

/**
 * Resize this dynamic array to fit at least `additional` more bytes.
 */
void dynarray_resize_to_fit(struct dynarray *self, size_t additional);

/**
 * Get an aligned pointer to the next element in the dynamic array. Subsequent
 * calls to this function will increment the returned pointer as well as the
 * length of this `struct dynarray`.
 *
 * # Safety
 * You must initialize the value at this pointer to a valid value for the type
 * which the dynamic array houses.
 */
void *dynarray_next_unchecked(struct dynarray *self, struct type val_type);

/**
 * Extend this array with another, identically laid out buffer, without
 * checking if it will fit.
 *
 * # Safety
 * This has all the safety requirements of a `memcpy()` with the additionoal
 * requirement that `self` must have enough remaining capacity to handle
 * extension. Use `dynarray_extend()` for an automatically growing version.
 */
void dynarray_extend_unchecked(struct dynarray *self, uint8_t *buf,
                               size_t buf_size);

/**
 * Extends this dynamic array with a buffer starting at `begin` and ending at 
 * `end` exclusive. This is supposed to mimic an iterator `begin` and `end`.
 * 
 * # Speed
 * On optimized-for-speed targets, this is likely to be just as good as 
 * assigning to `*dynarray_next()`. However, for size-optimized targets, this
 * will use a `memcpy()` internally, so is only better for reasonably large
 * buffers.
 */
void dynarray_extend(struct dynarray *self, void const *begin, void const *end);

/**
 * Get a pointer to the next element in this array. The pointer is guaranteed
 * to be valid for writes of the inner type.
 *
 * # Safety
 * You must initialize the value at this pointer to a valid value for the type
 * which the dynamic array houses.
 *
 * # Example
 * This example initializes the first 100 elements of a dynamic array to the
 * range of integers `1..=100`.
 *
 * ```c
 * struct dynarray arr = dynarray_new();
 * for (int n = 1; n <= 100; ++n) {
 *     *(int *)dynarray_next(&arr, TYPEINFO(int)) = n;
 * }
 * ```
 */
void *dynarray_next(struct dynarray *self, struct type val_type);

/**
 * Pop an element from the end of this array. The returned pointer is valid for
 * reads of the internal type if it is not `NULL`. `NULL` is returned to 
 * indicate the array is empty.
 */
void *dynarray_pop(struct dynarray *self, struct type val_type);

/**
 * Return the length of a dynamic array in terms of the number of elements of 
 * the inner type.
 */
inline size_t dynarray_length(struct dynarray const *self,
                              struct type val_type);

/**
 * Return the capacity of a dynamic array in terms of the number of elements of 
 * the inner type.
 */
inline size_t dynarray_capacity(struct dynarray const *self,
                                struct type val_type);

/**
 * Free this dynamic array, invalidating it for any future use.
 */
void dynarray_free(struct dynarray *self);

/**
 * Checked `get()` accessor this array. This is recommended over use of 
 * unchecked alternatives. The returned pointer is valid for reads and writes
 * of the inner type.
 * 
 * # Safety
 * `val_type` must be the inner type.
 */
void *dynarray_get(struct dynarray *self, struct type val_type, size_t index);