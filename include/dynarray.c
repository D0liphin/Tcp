#include "./dynarray.h"
#include "./panic.h"

struct type type_new(size_t size)
{
        struct type type;
        type.size = size;
        return type;
}

#define TYPEINFO(T) type_new(sizeof(T))

#define DYN_ARRAY_MIN_CAP 4

struct dynarray dynarray_new()
{
        struct dynarray arr;
        arr.data = NULL;
        arr.cap = 0;
        arr.len = 0;
        return arr;
}

void *dynarray_begin(struct dynarray *self)
{
        return self->data;
}

void *dynarray_end(struct dynarray *self)
{
        return &((uint8_t *)self->data)[self->len];
}

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
                PANIC("malloc() returned NULL");
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
                PANIC("malloc() returned NULL");
        }
        self->data = newdata;
        self->cap = newcap;
}

void *dynarray_next_unchecked(struct dynarray *restrict self,
                              struct type val_type)
{
        void *next = dynarray_end(self);
        self->len += val_type.size;
        return next;
}

void dynarray_extend_unchecked(struct dynarray *restrict self,
                               uint8_t *restrict buf, size_t buf_size)
{
        void *end = dynarray_end(self);
        memcpy((void *)end, (void *)buf, buf_size);
        self->len += buf_size;
}

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

void *dynarray_next(struct dynarray *restrict self, struct type val_type)
{
        if (self->cap <= self->len) {
                dynarray_resize(self, val_type);
        }
        return dynarray_next_unchecked(self, val_type);
}

void *dynarray_pop(struct dynarray *restrict self, struct type val_type)
{
        if (self->len > 0) {
                self->len -= val_type.size;
                return (void *)&((uint8_t *)self->data)[self->len];
        }
        return NULL;
}

inline size_t dynarray_length(struct dynarray const *self, struct type val_type)
{
        printf("self->len / val_type.size = %zu / %zu = %zu\n", self->len,
               val_type.size, self->len / val_type.size);
        return self->len / val_type.size;
}

inline size_t dynarray_capacity(struct dynarray const *self,
                                struct type val_type)
{
        return self->cap / val_type.size;
}

void dynarray_free(struct dynarray *restrict self)
{
        free(self->data);
}

/**
 * This is not `restrict` because we use it for both mutable and immutable 
 * access and just hope that the compiler understands the provenance of other
 * shared pointers.
 */
void *dynarray_get(struct dynarray *self, struct type val_type, size_t index)
{
        if (index < dynarray_length(self, val_type)) {
                return &((uint8_t *)self->data)[index * val_type.size];
        }
        PANIC("index out of bounds: the length is %zu but the index is %zu",
              dynarray_length(self, val_type), index);
        return NULL; // unreachable
}