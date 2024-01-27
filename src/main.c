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
struct type type_new(size_t size)
{
        struct type type;
        type.size = size;
        return type;
}

#define TYPEINFO(T) type_new(sizeof(T))

/**
 * Panic and `abort()` this program, with an error message first printed to 
 * `stderr`. You do not need to add a newline yourself.
 */
void panic(char const *restrict format, ...)
{
        va_list args;
        va_start(args, format);
        fprintf(stderr, format, args);
        puts("");
        va_end(args);
        abort();
}

/**
 * Mark an unreachable path as 'TODO'
 */
void todo()
{
        panic("Not yet implemented");
}

/**
 * A dynamic, exponentially growing array.
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
struct dynarray dynarray_new()
{
        struct dynarray arr;
        arr.data = NULL;
        arr.cap = 0;
        arr.len = 0;
        return arr;
}

/**
 * Return a pointer to the start of this array. This pointer is valid for casts
 * to a pointer to the inner type. And the resulting pointer is valid for reads
 * and writes of the inner type.
 */
void *dynarray_begin(struct dynarray *self)
{
        return self->data;
}

/**
 * Acquire a pointer to the end of this array. This pointer is valid for casts
 * to a pointer to the inner type. However, this pointer may not be valid for
 * reads or writes. You should just assume that it is not writeable.
 *
 * If you wish to modify the next unoccupied cell in this array, you can use
 * `dynarray_next()` for a safer version.
 */
void *dynarray_end(struct dynarray *self)
{
        return &((uint8_t *)self->data)[self->len];
}

/**
 * Exponentially resize this dynamic array. This only produces unrecoverable
 * errors.
 * 
 * This is ever so slightly faster than `dynarray_resize_to_fit()`. So I can 
 * justify having it here. 
 */
void dynarray_resize(struct dynarray *restrict self, struct type val_type)
{
        void *newdata;
        size_t newcap;
        if (self->cap > 0) {
                newcap = self->cap * 2;
                newdata = realloc(self->data, newcap);
        } else {
                newcap = DYN_ARRAY_MIN_CAP * val_type.size;
                newdata = malloc(newcap);
        }
        if (newdata == NULL) {
                panic("malloc() returned NULL");
        }
        self->data = newdata;
        self->cap = newcap;
}

/**
 * `__builtin_clz` or fallback implementation for `size_t` 
 */
size_t clzz(size_t n)
{
#if SIZE_MAX == UINT_MAX
        return __builtin_clz(n);
#elif SIZE_MAX == ULONG_MAX
        return __builtin_clzl(n);
#else
        size_t acc = 0;
        size_t mask = (size_t)1 << (sizeof(size_t) - 1);
        while ((mask | n) != n) {
                mask >>= 1;
                acc += 1;
        }
        return acc;
#endif
}

/**
 * The minimum power of 2 that is greater than or equal to `n`
 */
size_t minpow2(size_t n)
{
        size_t leading_zeros = clzz(n);
        size_t mask = ((size_t)1 << (sizeof(size_t) * 8 - 1)) >> leading_zeros;
        if (mask == n) {
                return mask;
        }
        return mask << 1;
}

/**
 * Resize this dynamic array to fit at least `additional` more bytes.
 */
void dynarray_resize_to_fit(struct dynarray *restrict self, size_t additional)
{
        void *newdata;
        size_t newcap = minpow2(self->cap + additional);
        if (self->cap == 0) {
                newdata = malloc(newcap);
        } else {
                newdata = realloc(self->data, newcap);
        }
        if (newdata == NULL) {
                panic("malloc() returned NULL");
        }
        self->data = newdata;
        self->cap = newcap;
}

/**
 * Get an aligned pointer to the next element in the dynamic array. Subsequent
 * calls to this function will increment the returned pointer as well as the
 * length of this `struct dynarray`.
 *
 * # Safety
 * You must initialize the value at this pointer to a valid value for the type
 * which the dynamic array houses.
 */
void *dynarray_next_unchecked(struct dynarray *restrict self,
                              struct type val_type)
{
        void *next = dynarray_end(self);
        self->len += val_type.size;
        return next;
}

/**
 * Extend this array with another, identically laid out buffer, without
 * checking if it will fit.
 *
 * # Safety
 * This has all the safety requirements of a `memcpy()` with the additionoal
 * requirement that `self` must have enough remaining capacity to handle
 * extension. Use `dynarray_extend()` for an automatically growing version.
 */
void dynarray_extend_unchecked(struct dynarray *restrict self,
                               uint8_t *restrict buf, size_t buf_size)
{
        void *end = dynarray_end(self);
        memcpy((void *)end, (void *)buf, buf_size);
        self->len += buf_size;
}

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
void dynarray_extend(struct dynarray *restrict self, void const *begin,
                     void const *end)
{
        size_t buf_size = end - begin;
        size_t remaining_capacity = self->cap - self->len;
        if (remaining_capacity < buf_size) {
                dynarray_resize_to_fit(self, buf_size);
        }
        dynarray_extend_unchecked(self, (uint8_t *)begin, buf_size);
}

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
void *dynarray_next(struct dynarray *restrict self, struct type val_type)
{
        if (self->cap <= self->len) {
                dynarray_resize(self, val_type);
        }
        return dynarray_next_unchecked(self, val_type);
}

/**
 * Pop an element from the end of this array. The returned pointer is valid for
 * reads of the internal type if it is not `NULL`. `NULL` is returned to 
 * indicate the array is empty.
 */
void *dynarray_pop(struct dynarray *restrict self, struct type val_type)
{
        if (self->len >= 0) {
                self->len -= val_type.size;
                return (void *)&((uint8_t *)self->data)[self->len];
        }
        return NULL;
}

/**
 * Return the length of a dynamic array in terms of the number of elements of 
 * the inner type.
 */
inline size_t dynarray_length(struct dynarray const *self, struct type val_type)
{
        return self->len / val_type.size;
}

/**
 * Return the capacity of a dynamic array in terms of the number of elements of 
 * the inner type.
 */
inline size_t dynarray_capacity(struct dynarray const *self,
                                struct type val_type)
{
        return self->cap / val_type.size;
}

/**
 * Free this dynamic array, invalidating it for any future use.
 */
void dynarray_free(struct dynarray *restrict self)
{
        free(self->data);
}

int main()
{
        struct dynarray arr = dynarray_new();
        int values[10] = { 1, 2, 4, 4, 5, 6, 7, 8, 9, 10 };
        dynarray_extend(&arr, &values[0], &values[10]);
        for (int *it = dynarray_begin(&arr); it != dynarray_end(&arr); ++it) {
                printf("%d ", *it);
        }
        puts("");
        for (size_t _ = 0; _ < 10; ++_) {
                int n = *(int *)dynarray_pop(&arr, TYPEINFO(int));
                printf("%d ", n);
        }
        puts("");
        dynarray_free(&arr);
}