#ifndef SORT_H

#define COMPARE_FUNCTION(name) i32 name(const void* A, const void* B)
typedef COMPARE_FUNCTION(compare_function_t);

#define SORT_FUNCTION(name) void name(void* Ptr, u32 Count, u32 ElementSize, compare_function_t* Compare)
typedef SORT_FUNCTION(sort_function_t);

void Swap(void* A, void* B, void* Temp, u32 ElementSize) {
  memcpy(Temp, A, ElementSize);
  memcpy(A, B, ElementSize);
  memcpy(B, Temp, ElementSize);
}

u32
Partition(void* Array, u32 Low, u32 High, u32 ElementSize, compare_function_t* Compare) {
  void* Pivot = (void*)((uintptr_t)Array + ElementSize * Low);
  u32 I = Low;
  u32 J = High;
  void* Temp = ArenaAlloc(&ScratchArena, ElementSize);

  while (I < J) {
    void* IElement = (void*)((uintptr_t)Array + ElementSize * I);
    void* JElement = (void*)((uintptr_t)Array + ElementSize * J);
    while ((Compare(IElement, Pivot) <= 0) && (I <= High - 1)) {
      I++;
      IElement = (void*)((uintptr_t)Array + ElementSize * I);
    }
    while ((Compare(JElement, Pivot) > 0) && (J >= Low + 1)) {
      J--;
      JElement = (void*)((uintptr_t)Array + ElementSize * J);
    }
    if (I < J) {
      IElement = (void*)((uintptr_t)Array + ElementSize * I);
      JElement = (void*)((uintptr_t)Array + ElementSize * J);
      Swap(IElement, JElement, Temp, ElementSize);
    }
  }
  void* LowElement = (void*)((uintptr_t)Array + Low * ElementSize);
  void* JElement = (void*)((uintptr_t)Array + J * ElementSize);
  Swap(LowElement, JElement, Temp, ElementSize);
  return J;
}

SORT_FUNCTION(QuickSort) {
  if (Count <= 1) {
    return;
  }
  u32 Low = 0;
  u32 High = Count - 1;
  if (Low < High) {
    u32 PartitionIndex = Partition(Ptr, Low, High, ElementSize, Compare);
    u32 LowCount = Count - Low - PartitionIndex - 1;
    u32 HighCount = Count - (PartitionIndex + 1);
    void* LowPtr = (void*)((uintptr_t)Ptr + Low * ElementSize);
    void* HighPtr = (void*)((uintptr_t)Ptr + (PartitionIndex + 1) * ElementSize);
    QuickSort(LowPtr, LowCount, ElementSize, Compare);
    QuickSort(HighPtr, HighCount, ElementSize, Compare);
  }
}

#define SORT_H
#endif
