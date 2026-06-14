inline FILETIME
Win32GetLastWriteTime(const char* Filename) {
    FILETIME LastWriteTime = {};

    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFileA(Filename, &FindData);
    if (FindHandle != INVALID_HANDLE_VALUE) {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }

    return (LastWriteTime);
}

internal void
Win32GetExecutableFilename(win32_state* State) {
    DWORD SizeOfFilename = GetModuleFileNameA(0, State->ExeFilename, sizeof(State->ExeFilename));
    State->BasePathPtr = State->ExeFilename;
    for (char* Scan = State->ExeFilename; *Scan; ++Scan) {
        if (*Scan == '\\') {
            State->BasePathPtr = Scan + 1;
        }
    }
}

internal void
Win32BuildExecutablePathFilename(win32_state* State, const char* Filename,
                                 size_t DestCount, char* Dest)
{
    CatStrings((size_t)(State->BasePathPtr - State->ExeFilename),
               State->ExeFilename,
               StringLength(Filename), Filename,
               DestCount, Dest);
}

//#define PLATFORM_LOAD_SHADER(name) u32 name(char* VertPath, char* FragPath);
//typedef PLATFORM_LOAD_SHADER(platform_load_shader_t);
PLATFORM_LOAD_SHADER(PlatformLoadShader) {
  if (GetPlatformState().ShadersCount >= MAX_SHADERS) {
    return 0;
  }
  shader_file& ShaderFile = GetPlatformState().Shaders[GetPlatformState().ShadersCount];
  Win32BuildExecutablePathFilename(&Win32State, VertPath,
                                   sizeof(ShaderFile.VertexFilename), 
                                   ShaderFile.VertexFilename);
  Win32BuildExecutablePathFilename(&Win32State, FragPath,
                                   sizeof(ShaderFile.FragmentFilename), 
                                   ShaderFile.FragmentFilename);
  if (ShaderFile.Shader.loadFromFile(ShaderFile.VertexFilename, 
                                     ShaderFile.FragmentFilename))
  {
    return GetPlatformState().ShadersCount++;
  }
  return 0;
}
