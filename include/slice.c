#include <stdint.h>
#include <stddef.h>
#include "./type.h"
#include "./slice.h"
#include "./panic.h"

void *slice_begin(slice self)
{
        return self.begin;
}

void *slice_end(slice self)
{
        return self.end;
}

void *slice_get(slice self, struct type val_type, size_t index)
{
        if (index < slice_length(self, val_type)) {
                return &((uint8_t *)self.begin)[index * val_type.size];
        }
        PANIC("index out of bounds: the length is %zu but the index is %zu",
              slice_length(self, val_type), index);
        return NULL; // unreachable
}

slice slice_new(void *begin, void *end)
{
        slice sl;
        sl.begin = begin;
        sl.end = end;
        return sl;
}