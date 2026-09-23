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

PLATFORM_GET_FILE_LENGTH(PlatformGetFileLength) {
  char FullPath[PLATFORM_FILENAME_COUNT];
  Win32BuildExecutablePathFilename(&Win32State, FilePath,
                                   sizeof(FullPath), FullPath);
  HANDLE FileHandle = CreateFile(
    FullPath,
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  if (FileHandle == INVALID_HANDLE_VALUE) {
    printf("Failed to get file handle: %s\n", FullPath);
    return 0;
  }

  DWORD FileSizeLow = GetFileSize(FileHandle, 0);
  
  if (FileSizeLow == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) {
    CloseHandle(FileHandle);
    return 0;
  }

  CloseHandle(FileHandle);

  return ((u32)FileSizeLow);
}

PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile) {
  char FullPath[PLATFORM_FILENAME_COUNT];
  Win32BuildExecutablePathFilename(&Win32State, FilePath,
                                   sizeof(FullPath), FullPath);

  HANDLE FileHandle = CreateFile(
    FullPath,
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  if (FileHandle == INVALID_HANDLE_VALUE) {
    printf("Failed to get file handle: %s\n", FullPath);
    return;
  }

  DWORD BytesRead = 0;
  BOOL result = ReadFile(
    FileHandle,
    Output,
    Length,
    &BytesRead,
    NULL
  );

  if (!result) {
    CloseHandle(FileHandle);
    printf("Failed to read file: %s, %u\n", FullPath, GetLastError());
    return;
  }

  CloseHandle(FileHandle);
  return;
}

PLATFORM_LOAD_TEXTURE(PlatformLoadTexture) {
  texture_id TextureID = { };
  if (GetPlatformState().TexturesCount >= MAX_TEXTURES) {
    return TextureID;
  }
  texture_file& TextureFile = GetPlatformState().Textures[GetPlatformState().TexturesCount];
  Win32BuildExecutablePathFilename(&Win32State, TexturePath,
                               sizeof(TextureFile.Filename),
                               TextureFile.Filename);
  if (TextureFile.Texture.loadFromFile(TextureFile.Filename)) {
    TextureID.Handle = GetPlatformState().TexturesCount++;
    sf::Vector2u size = TextureFile.Texture.getSize();
    TextureID.Width = (f32)size.x;
    TextureID.Height = (f32)size.y;
  }
  return TextureID;
}

PLATFORM_LOAD_SHADER(PlatformLoadShader) {
  shader_id ShaderID = { };
  if (GetPlatformState().ShadersCount >= MAX_SHADERS) {
    return ShaderID;
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
    ShaderID.Handle = GetPlatformState().ShadersCount++;
  }
  return ShaderID;
}

PLATFORM_LOAD_SOUND(PlatformLoadSound) {
  sound_id SoundID = { };
  if (GetPlatformState().SoundsCount >= MAX_SOUNDS) {
    return SoundID;
  }
  sound_file& SoundFile = GetPlatformState().Sounds[GetPlatformState().SoundsCount];
  Win32BuildExecutablePathFilename(&Win32State, SoundPath,
                                   sizeof(SoundFile.Filename),
                                   SoundFile.Filename);
  if (SoundFile.Buffer.loadFromFile(SoundFile.Filename)) {
    SoundFile.Sound.setBuffer(SoundFile.Buffer);
    SoundID.Handle = GetPlatformState().SoundsCount++;
  }
  return SoundID;
}
