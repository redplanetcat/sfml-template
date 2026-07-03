#ifndef GAME_H

#include <cstring>
#include <cstdio>
#include <cmath>
#include "../common/defines.h"
#include "../common/types.h"
#include "../common/linalg.h"
#include "../common/graphics.h"
#include "../common/input.h"
#include "../common/hash.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define PLATFORM_DRAW_VERTICES(name) void name(vertex* Vertices, u32 NumVertices, \
                                               primitive_type PrimitiveType)
typedef PLATFORM_DRAW_VERTICES(platform_draw_vertices_t);

#define PLATFORM_LOAD_TEXTURE(name) texture_id name(const char* TexturePath)
typedef PLATFORM_LOAD_TEXTURE(platform_load_texture_t);

#define PLATFORM_USE_TEXTURE(name) void name(texture_id TextureHandle)
typedef PLATFORM_USE_TEXTURE(platform_use_texture_t);

#define PLATFORM_LOAD_SHADER(name) shader_id name(const char* VertPath, const char* FragPath)
typedef PLATFORM_LOAD_SHADER(platform_load_shader_t);

#define PLATFORM_USE_SHADER(name) void name(shader_id ShaderHandle)
typedef PLATFORM_USE_SHADER(platform_use_shader_t);

#define PLATFORM_SET_SHADER_UNIFORM_VEC2(name) void name(shader_id ShaderHandle, \
                                                    const char* UniformName, \
                                                    vec2 Vector)
typedef PLATFORM_SET_SHADER_UNIFORM_VEC2(platform_set_shader_uniform_vec2_t);

#define PLATFORM_LOAD_SOUND(name) sound_id name(const char* SoundPath)
typedef PLATFORM_LOAD_SOUND(platform_load_sound_t);

#define PLATFORM_PLAY_SOUND(name) void name(sound_id SoundHandle)
typedef PLATFORM_PLAY_SOUND(platform_play_sound_t);

#define PLATFORM_CREATE_TEXT(name) text_id name(const char* String, u32 Size, color Color)
typedef PLATFORM_CREATE_TEXT(platform_create_text_t);

#define PLATFORM_UPDATE_TEXT(name) void name(text_id TextHandle, \
                                             const char* String, \
                                             color Color, \
                                             vec2 Position, \
                                             vec2 Scale, \
                                             rect_alignment Alignment)
typedef PLATFORM_UPDATE_TEXT(platform_update_text_t);

#define PLATFORM_DRAW_TEXT(name) void name(text_id TextHandle)
typedef PLATFORM_DRAW_TEXT(platform_draw_text_t);

struct platform_callbacks {
  platform_draw_vertices_t* PlatformDrawVertices;

  platform_load_texture_t* PlatformLoadTexture;
  platform_use_texture_t* PlatformUseTexture;

  platform_load_shader_t* PlatformLoadShader;
  platform_use_shader_t* PlatformUseShader;
  platform_set_shader_uniform_vec2_t* PlatformSetShaderUniformVec2;

  platform_load_sound_t* PlatformLoadSound;
  platform_play_sound_t* PlatformPlaySound;

  platform_create_text_t* PlatformCreateText;
  platform_update_text_t* PlatformUpdateText;
  platform_draw_text_t* PlatformDrawText;
};

struct game_memory {
  u64 PermanentStorageSize;
  void* PermanentStorage;
  u64 ScratchStorageSize;
  void* ScratchStorage;
  b32 IsInitialized;
  platform_callbacks PlatformCallbacks;
};


#define GAME_UPDATE(name) void name(game_memory* Memory, game_input* Input, f32 Delta)
typedef GAME_UPDATE(game_update_t);

#define GAME_RENDER(name) void name(game_memory* Memory)
typedef GAME_RENDER(game_render_t);


#define GAME_H
#endif
