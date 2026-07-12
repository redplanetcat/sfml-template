#ifndef ARENA_ALLOCATOR_H

#ifndef DEFAULT_ALIGNMENT
    #define DEFAULT_ALIGNMENT (2 * sizeof(void*))
#endif

typedef struct Arena {
    unsigned char*  buf;
    size_t          buf_len;
    size_t          prev_offset;
    size_t          curr_offset;
} Arena;

uintptr_t
align_forward(uintptr_t ptr, size_t align) {
    uintptr_t p, a, modulo;

    p = ptr;
    a = (uintptr_t)align;
    modulo = p & (a - 1);

    if (modulo != 0) {
        p += a - modulo;
    }
    return p;
}

internal inline void
arena_init(Arena* a, void* backing_buffer, size_t backing_buffer_length) {
    a->buf = (unsigned char*)backing_buffer;
    a->buf_len = backing_buffer_length;
    a->curr_offset = 0;
    a->prev_offset = 0;
}

void*
arena_alloc_align(Arena* a, size_t size, size_t align) {
    uintptr_t curr_ptr = (uintptr_t)a->buf + (uintptr_t)a->curr_offset;
    uintptr_t offset = align_forward(curr_ptr, align);
    offset -= (uintptr_t)a->buf;

    if ((offset + size) <= a->buf_len) {
        void* ptr = &a->buf[offset];
        a->prev_offset = offset;
        a->curr_offset = offset + size;

        SDL_memset(ptr, 0, size);
        return ptr;
    }

    Assert(0 && "Memory is out of bounds of the buffer in this arena");
    return NULL;
}

internal inline void*
arena_alloc(Arena* a, size_t size) {
    return arena_alloc_align(a, size, DEFAULT_ALIGNMENT);
}

void*
arena_resize_align(Arena* a, void* old_memory, size_t old_size, size_t new_size, size_t align) {
    unsigned char* old_mem = (unsigned char*) old_memory;
    assert(is_power_of_two(align));

    if (old_mem == NULL || old_size == 0) {
        return arena_alloc_align(a, new_size, align);
    } else if ((a->buf <= old_mem) && (old_mem < (a->buf + a->buf_len))) {
        if ((a->buf + a->prev_offset) == old_mem) {
            a->curr_offset = a->prev_offset + new_size;
            if (new_size > old_size) {
                SDL_memset(&a->buf[a->curr_offset], 0, new_size - old_size);
            }
            return old_memory;
        } else {
            void* new_memory = arena_alloc_align(a, new_size, align);
            size_t copy_size = old_size < new_size ? old_size : new_size;
            SDL_memmove(new_memory, old_memory, copy_size);
            return new_memory;
        }
    } else {
        assert(0 && "Memory is out of bounds of the buffer in this arena");
        return NULL;
    }
}

internal inline void*
arena_resize(Arena* a, void* old_memory, size_t old_size, size_t new_size) {
    return arena_resize_align(a, old_memory, old_size, new_size, DEFAULT_ALIGNMENT);
}

internal inline void*
arena_realloc_align(Arena* a, void* ptr, size_t size, size_t align) {
    return arena_resize_align(a, ptr, size, size, align);
}

internal inline void*
arena_realloc(Arena* a, void* ptr, size_t size) {
    return arena_realloc_align(a, ptr, size, DEFAULT_ALIGNMENT);
}

internal inline void
arena_free_all(Arena* a) {
    a->curr_offset = 0;
    a->prev_offset = 0;
}

typedef struct Temp_Arena_Memory {
    Arena* arena;
    size_t prev_offset;
    size_t curr_offset;
} Temp_Arena_Memory;

internal inline Temp_Arena_Memory
temp_arena_memory_begin(Arena* a) {
    Temp_Arena_Memory temp;
    temp.arena = a;
    temp.prev_offset = a->prev_offset;
    temp.curr_offset = a->curr_offset;
    return temp;
}

internal inline void
temp_arena_memory_end(Temp_Arena_Memory temp) {
    temp.arena->prev_offset = temp.prev_offset;
    temp.arena->curr_offset = temp.curr_offset;
}

typedef struct ScratchArena {
    Arena arena;
    size_t numAllocations;
} ScratchArena;

internal inline void
scratch_arena_init(ScratchArena* scratchArena, void* backing_buffer, size_t backing_buffer_length) {
    arena_init(&scratchArena->arena, backing_buffer, backing_buffer_length);
    scratchArena->numAllocations = 0;
}

void*
scratch_arena_alloc(ScratchArena* arena, size_t size) {
    void* result = arena_alloc(&arena->arena, size);
    if (result != NULL) {
        arena->numAllocations++;
    }
    return result;
}

void
scratch_arena_free(ScratchArena* arena, void* ptr) {
    arena->numAllocations--;
    if (arena->numAllocations == 0) {
        arena_free_all(&arena->arena);
    }
}

void
scratch_arena_flush(ScratchArena* arena) {
    arena->numAllocations = 0;
    arena_free_all(&arena->arena);
}

void*
scratch_arena_realloc(ScratchArena* arena, void* ptr, size_t size) {
    void* result = arena_realloc(&arena->arena, ptr, size);
    if ((result != NULL) && (result != ptr)) {
        arena->numAllocations++;
    }
    return result;
}

#define ARENA_ALLOCATOR_H
#endif
