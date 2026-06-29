#ifndef ENTITIES_H

#define MAX_ENTITIES 2048

enum class kind {
  Nil,
  Player,
  Apple,
  Stone
};

enum class entity_flags : u32 {
  Drawable = 1 << 0,
  Dead = 1 << 1,
  Flip = 1 << 2,
  Damager = 1 << 3,
  Pickup = 1 << 4,
  SpawnAnimated = 1 << 5,
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
  vec2 Pos;
  vec2 Dir;
  vec2 Size;
  vec2 Collider;
  texture_id Texture;
  shader_id Shader;
  kind Kind;
  color Color;
  f32 Scale;
  f32 Rot;
  f32 Timer;
  f32 TimerLength;
  u32 Z;
  u32 Flags;

  operator bool() const {
    return Kind != kind::Nil;
  }

};



struct entity_manager {
  entity Entities[MAX_ENTITIES];
  b32 Used[MAX_ENTITIES];
  u32 Gen[MAX_ENTITIES];
  u32 FirstFree;
  u32 NextFree[MAX_ENTITIES];
  u32 EntitiesCount;

  entity_manager() {
    memset(Entities, 0, sizeof(Entities));
    memset(Gen, 0, sizeof(Gen));
    memset(Used, 0, sizeof(Used));
    for (u32 I = 1; I < MAX_ENTITIES-1; ++I) {
      NextFree[I] = I+1;
    }
    FirstFree = 1;
    EntitiesCount = 0;
  }
  
  struct entity_iter {
    entity_manager* Entities;
    entity_ref Ref;

    entity_iter(entity_manager* EntityManager, entity_ref EntityRef)
      : Entities(EntityManager)
      , Ref(EntityRef) { };

    // Dereference operator (*iter)
    entity 
    operator*() const {
      return Entities->Get(Ref);
    }

    // Arrow operator (iter->)
    entity* 
    operator->() {
      return &Entities->Get(Ref);
    }

    // Prefix increment
    entity_iter& 
    operator++() {
      Ref.Idx += 1;
      while ((Ref.Idx < MAX_ENTITIES-1) && !Entities->Used[Ref.Idx]) {
        Ref.Idx += 1;
      }
      Ref.Gen = Entities->Gen[Ref.Idx];
      return *this;
    }

    // Postfix increment
    entity_iter 
    operator++(int) {
      entity_iter tmp = *this;
      ++(*this);
      return tmp;
    }

    friend b32 
    operator==(const entity_iter& IterA, const entity_iter& IterB) {
      return IterA.Ref.Idx == IterB.Ref.Idx;
    }

    friend b32 
    operator!=(const entity_iter& IterA, const entity_iter& IterB) {
      return !(IterA == IterB);
    }
  };
  
  entity_ref 
  Add(kind Kind) {
    u32 Slot = FindEmptySlot();
    if (Slot) {
      Entities[Slot] = { };
      Entities[Slot].Kind = Kind;
      Used[Slot] = true;
      Gen[Slot] += 1;
      FirstFree = NextFree[Slot];
      EntitiesCount += 1;
      return { Slot, Gen[Slot] };
    } else {
      return entity_ref::Nil();
    }
  }

  void 
  Rem(entity_ref Ref) {
    if (u32 Slot = deref(Ref)) {
      Used[Slot] = false;
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

  entity_iter begin() {
    return entity_iter(this, entity_ref{ 1, Gen[1] });
  }

  entity_iter end() {
    return entity_iter(this, entity_ref{ MAX_ENTITIES-1, Gen[MAX_ENTITIES-1] });
  }

private:
  u32 
  FindEmptySlot() {
    return FirstFree;
  }

  u32 
  deref(entity_ref Ref) {
    if (Ref.Idx > 0 && Ref.Idx < MAX_ENTITIES 
        && Used[Ref.Idx] && Ref.Gen == Gen[Ref.Idx]) 
    {
      return Ref.Idx;
    } else {
      return 0;
    }
  }

};

#define ENTITIES_H
#endif
