#if !defined(GAME_H)

#include "../common/defines.h"
#include "../common/types.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define MAX_VERTICES 16384
#define MAX_ENTITIES 2048

struct game_input {

};

struct ivec2 {
  i32 x;
  i32 y;
};

struct vec2 {
  f32 x;
  f32 y;
};

struct color {
  u8 r;
  u8 g;
  u8 b;
  u8 a;
};

struct vertex {
  vec2 Position;
  color Color;
  vec2 TexCoord;
};

enum class primitive_type {
  Points,
  Lines,
  Triangles
};

#define PLATFORM_RENDER_TRIANGLE(name) void name()
typedef PLATFORM_RENDER_TRIANGLE(platform_render_triangle_t);

#define PLATFORM_HANDLE_INPUT(name) void name()
typedef PLATFORM_HANDLE_INPUT(platform_handle_input_t);

#define PLATFORM_DRAW_VERTICES(name) void name(vertex* Vertices, u32 NumVertices, \
                                               primitive_type PrimitiveType)
typedef PLATFORM_DRAW_VERTICES(platform_draw_vertices_t);

struct platform_callbacks {
  platform_render_triangle_t* PlatformRenderTriangle;
  platform_handle_input_t* PlatformHandleInput;
  platform_draw_vertices_t* PlatformDrawVertices;
};

struct game_memory {
  u64 PermanentStorageSize;
  void* PermanentStorage;
  u64 ScratchStorageSize;
  void* ScratchStorage;
  b32 IsInitialized;
  platform_callbacks PlatformCallbacks;
};

enum class entity_kind {
  Nil,
  Player,
  Apple,
  Stone
};

enum class entity_flags {
  Dead,
};

struct entity_ref {
  u32 Idx;
  u32 Gen;

  internal entity_ref Nil() {
    return { 0, 0 };
  }
};


struct entity {
  entity_ref Parent;
  entity_ref Next;
  vec2 Position;
  vec2 Size;
  vec2 ColliderSize;
  entity_kind Kind;
  color Color;
  f32 Rotation;
  f32 Timer;
  f32 TimerLength;
  u32 ZIndex;
  u32 Flags;

  operator bool() const {
    return Kind != entity_kind::Nil;
  }

};


struct entity_manager {
  entity Entities[MAX_ENTITIES];
  b32 UsedEntities[MAX_ENTITIES];
  u32 Gen[MAX_ENTITIES];
  u32 FirstFree;
  u32 NextFree[MAX_ENTITIES];
  u32 EntitiesCount;
  
  entity_ref 
  Add(entity_kind Kind) {
    u32 Slot = FindEmptySlot();

    if (Slot) {
      Entities[Slot] = { };
      Entities[Slot].Kind = Kind;
      UsedEntities[Slot] = true;
      Gen[Slot] += 1;
      EntitiesCount += 1;
      return { Slot, Gen[Slot] };
    } else {
      return entity_ref::Nil();
    }
  }

  void 
  Rem(entity_ref Ref) {
    if (u32 Slot = deref(Ref)) {
      UsedEntities[Slot] = false;
      if (FirstFree) {
        NextFree[Slot] = FirstFree;
      }
      FirstFree = Slot;
      EntitiesCount -= 1;
    }
  }

  entity& 
  Get(entity_ref Ref) {
    return Entities[deref(Ref)];
  }

  u32
  Count() {
    return EntitiesCount;
  }

private:
  u32 
  FindEmptySlot() {
    return FirstFree;
  }

  u32 
  deref(entity_ref Ref) {
    if (Ref.Idx > 0 && Ref.Idx < MAX_ENTITIES 
        && UsedEntities[Ref.Idx] && Ref.Gen == Gen[Ref.Idx]) 
    {
      return Ref.Idx;
    } else {
      return 0;
    }
  }

};

struct game_state {
  entity_manager Entities;
  entity_ref PlayerIdx;
  b32 IsInitialized;
};

#define GAME_UPDATE(name) void name(game_memory* Memory, game_input* Input)
typedef GAME_UPDATE(game_update_t);

#define GAME_RENDER(name) void name(game_memory* Memory)
typedef GAME_RENDER(game_render_t);


#define GAME_H
#endif
