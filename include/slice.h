#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * A more sane generic slice type. For example, we can use this type to 
 * represent string slices or a view into a `dynarray`.
 * 
 * Slices do not need to be freed. They are non-owning.
 */
typedef struct {
        /**
         * Inclusive begin of the slice.
         */
        void *begin;
        /**
	 * Exclusive end of the slice.
	 */
        void *end;
} slice;

/**
 * Return an iterator to the beginning of this slice. The returned pointer is
 * valid for the inner type.
 */
void *slice_begin(slice self);

/**
 * A pointer one past the end of this iterator.
 */
void *slice_end(slice self);

/**
 * Bounds-checked slice element access of the n-th element. The returned pointer
 * is valid for reads and writes of the inner type.
 */
void *slice_get(slice self, struct type val_type, size_t index);

/**
 * The length of this slice, in multiples of the inner type.
 */
static inline size_t slice_length(slice self, struct type val_type)
{
        return ((size_t)self.end - (size_t)self.begin) / val_type.size;
}

/**
 * Construct a slice with a beggining and an end. 
 * 
 * # Safety 
 * - `begin` and `end` must be pointers aligned to at least the alignment of the
 *   inner type.
 * - All offsets from `begin` in this range must be valid for reads and writes 
 *   of the inner type.
 */
slice slice_new(void *begin, void *end);