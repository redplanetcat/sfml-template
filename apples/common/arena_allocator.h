#ifndef ARENA_ALLOCATOR_H

#ifndef DEFAULT_ALIGNMENT
    #define DEFAULT_ALIGNMENT (2 * sizeof(void*))
#endif

static inline bool IsPowerOfTwo(uintptr_t X) {
    return (X & (X - 1)) == 0;
}

struct arena {
    unsigned char*  Buf;
    size_t          BufLen;
    size_t          PrevOffset;
    size_t          CurrOffset;
};

uintptr_t
AlignForward(uintptr_t Ptr, size_t Align) {
    uintptr_t P, A, Modulo;

    P = Ptr;
    A = (uintptr_t)Align;
    Modulo = P & (A - 1);

    if (Modulo != 0) {
        P += A - Modulo;
    }
    return P;
}

internal inline void
ArenaInit(arena* A, void* BackingBuffer, size_t BackingBufferLength) {
    A->Buf = (unsigned char*)BackingBuffer;
    A->BufLen = BackingBufferLength;
    A->CurrOffset = 0;
    A->PrevOffset = 0;
}

void*
ArenaAllocAlign(arena* A, size_t Size, size_t Align) {
    uintptr_t CurrPtr = (uintptr_t)A->Buf + (uintptr_t)A->CurrOffset;
    uintptr_t Offset = AlignForward(CurrPtr, Align);
    Offset -= (uintptr_t)A->Buf;

    if ((Offset + Size) <= A->BufLen) {
        void* Ptr = &A->Buf[Offset];
        A->PrevOffset = Offset;
        A->CurrOffset = Offset + Size;

        memset(Ptr, 0, Size);
        return (Ptr);
    }

    Assert(0 && "Memory is out of bounds of the buffer in this arena");
    return (NULL);
}

internal inline void*
ArenaAlloc(arena* A, size_t Size) {
    return (ArenaAllocAlign(A, Size, DEFAULT_ALIGNMENT));
}

void*
ArenaResizeAlign(arena* A, void* OldMemory, size_t OldSize, size_t NewSize, size_t Align) {
    unsigned char* OldMem = (unsigned char*) OldMemory;
    Assert(IsPowerOfTwo(Align));

    if (OldMem == NULL || OldSize == 0) {
        return ArenaAllocAlign(A, NewSize, Align);
    } else if ((A->Buf <= OldMem) && (OldMem < (A->Buf + A->BufLen))) {
        if ((A->Buf + A->PrevOffset) == OldMem) {
            A->CurrOffset = A->PrevOffset + NewSize;
            if (NewSize > OldSize) {
                memset(&A->Buf[A->CurrOffset], 0, NewSize - OldSize);
            }
            return (OldMemory);
        } else {
            void* NewMemory = ArenaAllocAlign(A, NewSize, Align);
            size_t CopySize = OldSize < NewSize ? OldSize : NewSize;
            memmove(NewMemory, OldMemory, CopySize);
            return (NewMemory);
        }
    } else {
        Assert(0 && "Memory is out of bounds of the buffer in this arena");
        return (NULL);
    }
}

internal inline void*
ArenaResize(arena* A, void* OldMemory, size_t OldSize, size_t NewSize) {
    return ArenaResizeAlign(A, OldMemory, OldSize, NewSize, DEFAULT_ALIGNMENT);
}

internal inline void*
ArenaReallocAlign(arena* A, void* Ptr, size_t Size, size_t Align) {
    return ArenaResizeAlign(A, Ptr, Size, Size, Align);
}

internal inline void*
ArenaRealloc(arena* A, void* Ptr, size_t Size) {
    return ArenaReallocAlign(A, Ptr, Size, DEFAULT_ALIGNMENT);
}

internal inline void
ArenaFreeAll(arena* A) {
    A->CurrOffset = 0;
    A->PrevOffset = 0;
}

struct temp_arena_memory {
    arena* Arena;
    size_t PrevOffset;
    size_t CurrOffset;
};

internal inline temp_arena_memory
TempArenaMemoryBegin(arena* A) {
    temp_arena_memory Temp;
    Temp.Arena = A;
    Temp.PrevOffset = A->PrevOffset;
    Temp.CurrOffset = A->CurrOffset;
    return (Temp);
}

internal inline void
TempArenaMemoryEnd(temp_arena_memory Temp) {
    Temp.Arena->PrevOffset = Temp.PrevOffset;
    Temp.Arena->CurrOffset = Temp.CurrOffset;
}

#define ARENA_ALLOCATOR_H
#endif
