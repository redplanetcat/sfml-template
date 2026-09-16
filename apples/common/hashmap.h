#ifndef HASHMAP_H

#ifdef __cplusplus
extern "C" {
#endif

struct hashmap_node {
    u32 KeyIdx;
    void* Value;
    struct hashmap_node* Next;
};

struct hashmap {
    u32 N;
    u32 Size;
    u32 KeySize;
    u32 BufIndex;
    u32 IterBufIndex;
    char* Keys;
    struct hashmap_node* Buffer;
    struct hashmap_node** IterBuffer;
    u32* Hash;
    arena* Arena;
};

internal inline void
HashmapInit(
    struct hashmap* Map,
    u32 NumElements,
    u32 ElementSize,
    u32 KeySize,
    arena* Arena
) {
    memset(Map, 0, sizeof(struct hashmap));
    Map->N = NumElements;
    printf("[%s:%i] creating hashmap with [%u] elements", __FILE__, __LINE__, NumElements);
    Map->Size = ElementSize;
    Map->KeySize = KeySize;
    Map->BufIndex = 0;
    Map->IterBufIndex = 0;
    Map->Arena = Arena;
    Map->Keys = (char*)ArenaAlloc(Arena, NumElements * KeySize);
    Map->Buffer = (struct hashmap_node*)ArenaAlloc(Arena, NumElements * sizeof(struct hashmap_node));
    Map->IterBuffer = (struct hashmap_node**)ArenaAlloc(Arena, NumElements * sizeof(struct hashmap_node*));
    Map->Hash = (u32*)ArenaAlloc(Arena, NumElements * sizeof(u32));
    memset(Map->Keys, 0, NumElements * KeySize);
    memset(Map->Buffer, 0, NumElements * sizeof(struct hashmap_node));
    memset(Map->IterBuffer, 0, NumElements * sizeof(struct hashmap_node*));
    memset(Map->Hash, 0, NumElements * sizeof(u32));
}

void* 
HashmapEmplaceByHash(struct hashmap* Map, const void* Key, u32 Len, u32 Hash) {
    u32 HashedKey = Hash % Map->N;
    Len = Len > Map->KeySize ? Map->KeySize : Len;
    if (Map->BufIndex >= Map->N) {
        printf("Error: Exceeded hashmap capacity! %i > %i\n", Map->BufIndex, Map->N);
        return NULL;
    }
    void* Result = NULL;
    if (Map->Hash[HashedKey] == 0) {
        Map->Hash[HashedKey] = Map->BufIndex + 1;
        struct hashmap_node* Node = &Map->Buffer[Map->BufIndex];
        Node->Value = ArenaAlloc(Map->Arena, Map->Size);
        memset(Node->Value, 0, Map->Size);
        Result = Node->Value;
        u32 IterIdx = Map->IterBufIndex++;
        Node->KeyIdx = IterIdx;
        memcpy(Map->Keys+IterIdx*Map->KeySize, Key, Len);
        Map->BufIndex++;
        Map->IterBuffer[IterIdx] = Node;
        return (Result);
    }
    struct hashmap_node* Node = &Map->Buffer[Map->Hash[HashedKey]-1];
    if (0 == memcmp(Map->Keys+Node->KeyIdx*Map->KeySize, Key, Len)) {
        // handle the same key:
        memset(Node->Value, 0, Map->Size);
        Result = Node->Value;
    }
    struct hashmap_node** NextNode = &Node->Next;
    while (*NextNode != NULL) {
        NextNode = &(*NextNode)->Next;
    }
    *NextNode = (struct hashmap_node*)ArenaAlloc(Map->Arena, sizeof(struct hashmap_node));
    (*NextNode)->Next = NULL;
    u32 IterIdx = Map->IterBufIndex++;
    (*NextNode)->KeyIdx = IterIdx;;
    (*NextNode)->Value = ArenaAlloc(Map->Arena, Map->Size);
    memset((*NextNode)->Value, 0, Map->Size);
    Result = (*NextNode)->Value;
    memcpy(Map->Keys+IterIdx*Map->KeySize, Key, Len);
    Map->IterBuffer[IterIdx] = *NextNode;
    return (Result);
}

internal inline void 
HashmapCopy(struct hashmap* Map, struct hashmap* Other) {
    memcpy(Map->Buffer, Other->Buffer, Map->N * sizeof(struct hashmap_node));
    Map->BufIndex = Other->BufIndex;
    memcpy(Map->Hash, Other->Hash, Map->N * sizeof(u32));
}

internal inline void* 
HashmapEmplace(struct hashmap* Map, const void* Key, u32 Len) {
    u32 Hash = 0;
    CalculateHash(&Hash, Key, Len);
    return HashmapEmplaceByHash(Map, Key, Len, Hash);
}

void* 
HashmapInsertByHash(struct hashmap* Map, const void* Key, u32 Len, u32 Hash, const void* Value) {
    u32 HashedKey = Hash % Map->N;
    Len = Len > Map->KeySize ? Map->KeySize : Len;
    if (Map->BufIndex >= Map->N) {
        printf("Exceeded hashmap capacity! %i > %i\n", Map->BufIndex, Map->N);
        return NULL;
    }
    void* Result = NULL;
    if (Map->Hash[HashedKey] == 0) {
        Map->Hash[HashedKey] = Map->BufIndex + 1;
        struct hashmap_node* Node = &Map->Buffer[Map->BufIndex];
        u32 IterIdx = Map->IterBufIndex++;
        Node->KeyIdx = IterIdx;
        Node->Value = ArenaAlloc(Map->Arena, Map->Size);
        memset(Node->Value, 0, Map->Size);
        Result = Node->Value;
        memcpy(Result, Value, Map->Size);
        memcpy(Map->Keys+IterIdx*Map->KeySize, Key, Len);
        Map->BufIndex++;
        Map->IterBuffer[IterIdx] = Node;
        return (Result);
    }
    struct hashmap_node* Node = &Map->Buffer[Map->Hash[HashedKey]-1];
    if (0 == memcmp(Map->Keys+Node->KeyIdx*Map->KeySize, Key, Len)) {
        // handle the same key:
        memcpy(Node->Value, Value, Map->Size);
        Result = Node->Value;
        Map->IterBuffer[Map->IterBufIndex++] = Node;
        return (Result);
    }
    struct hashmap_node** NextNode = &Node->Next;
    while (*NextNode != NULL) {
        NextNode = &(*NextNode)->Next;
    }
    *NextNode = (struct hashmap_node*)ArenaAlloc(Map->Arena, sizeof(struct hashmap_node));
    (*NextNode)->Next = NULL;
    u32 IterIdx = Map->IterBufIndex++;
    (*NextNode)->KeyIdx = IterIdx;
    (*NextNode)->Value = ArenaAlloc(Map->Arena, Map->Size);
    memset((*NextNode)->Value, 0, Map->Size);
    Result = (*NextNode)->Value;
    memcpy(Result, 0, Map->Size);
    memcpy(Map->Keys+IterIdx*Map->KeySize, Key, Len);
    Map->IterBuffer[IterIdx] = *NextNode;
    return (Result);
}

internal inline void* 
HashmapInsert(struct hashmap* Map, const void* Key, u32 Len, const void* Value) {
    u32 Hash = 0;
    CalculateHash(&Hash, Key, Len);
    return HashmapInsertByHash(Map, Key, Len, Hash, Value);
}

void* 
HashmapGetByHash(struct hashmap* Map, const void* Key, u32 Len, u32 Hash) {
    u32 HashedKey = Hash % Map->N;
    Len = Len > Map->KeySize ? Map->KeySize : Len;
    void* Result = NULL;
    if (Map->Hash[HashedKey] == 0) {
        return NULL;
    }
    struct hashmap_node* Node = &Map->Buffer[Map->Hash[HashedKey]-1];
    if (0 == memcmp(Map->Keys+Node->KeyIdx*Map->KeySize, Key, Len)) {
        // handle the same key:
        return Node->Value;
    }
    struct hashmap_node** NextNode = &Node->Next;
    while ((*NextNode != NULL) && 
    (0 != memcmp((Map->Keys+(*NextNode)->KeyIdx*Map->KeySize), Key, Len))) {
        NextNode = &(*NextNode)->Next;
    }
    if (*NextNode == NULL) {
        return NULL;
    }
    Result = (*NextNode)->Value;
    return (Result);
}

internal inline void* 
HashmapGet(struct hashmap* Map, const void* Key, u32 Len) {
    u32 Hash = 0;
    CalculateHash(&Hash, Key, Len);
    return (HashmapGetByHash(Map, Key, Len, Hash));
}

internal inline u32 
HashmapGetElementCount(struct hashmap* Map) {
    return (Map->IterBufIndex);
}

internal inline struct hashmap_node* 
HashmapGetNodeByIndex(struct hashmap* Map, u32 Index) {
    if (Index > Map->IterBufIndex) {
        return NULL;
    }
    return (Map->IterBuffer[Index]);
}

internal inline void* 
HashmapGetByIndex(struct hashmap* Map, u32 Index) {
    if (Index > Map->IterBufIndex) {
        return NULL;
    }
    return (Map->IterBuffer[Index]->Value);
}

bool 
HashmapIter(struct hashmap* Map, u32* Index, void** Item) {
    *Item = Map->IterBuffer[*Index]->Value;
    *Index = *Index + 1;
    if (*Index > Map->IterBufIndex) {
        return false;
    }
    return true;
}

bool 
HashmapIterNodes(struct hashmap* Map, u32* Index, struct hashmap_node** Item) {
    *Item = Map->IterBuffer[*Index];
    *Index = *Index + 1;
    if (*Index > Map->IterBufIndex) {
        return false;
    }
    return true;
}

#ifdef __cplusplus
}
#endif

#define HASHMAP_H
#endif
