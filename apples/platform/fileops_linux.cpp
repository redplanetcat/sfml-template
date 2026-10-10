inline i64
LinuxGetLastWriteTime(const char* Filename) {
  struct stat FileStat = {};
  i64 LastWriteTime = 0;
  if (stat(Filename, &FileStat) == 0) {
    LastWriteTime = FileStat.st_mtim.tv_nsec;
  }
  return LastWriteTime;
}

internal i32
LinuxCopyFile(const char* Src, const char* Dst) {
  i32 InputFD = open(Src, O_RDONLY);
  if (InputFD < 0) return -1;

  i32 OutputFD = open(Dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (OutputFD < 0) {
    close(InputFD);
    return -1;
  }

  struct stat StatBuf;
  if (fstat(InputFD, &StatBuf) < 0) {
    close(InputFD);
    close(OutputFD);
    return -1;
  }

  off_t BytesToCopy = StatBuf.st_size;

  while (BytesToCopy > 0) {
    ssize_t Ret = copy_file_range(InputFD, NULL, OutputFD, NULL, (size_t)BytesToCopy, 0);
    if (Ret < 0) {
      close(InputFD);
      close(OutputFD);
      return -1;
    }
    if (Ret == 0) break;

    BytesToCopy -= Ret;
  }

  close(InputFD);
  close(OutputFD);
  return 0;
}

internal void
LinuxGetExecutableFilename(linux_state* State) {
  ssize_t BinFilenameLength = readlink("/proc/self/exe", State->BinFilename, sizeof(State->BinFilename) - 1);
  if (BinFilenameLength != -1) {
    State->BinFilename[BinFilenameLength] = '\0';
  } else {
    perror("readlink");
    return;
  }
  
  State->BasePathPtr = State->BinFilename;
  for (char* Scan = State->BinFilename; *Scan; ++Scan) {
    if (*Scan == '/') {
      State->BasePathPtr = Scan + 1;
    }
  }
}

internal void
LinuxBuildExecutablePathFilename(linux_state* State, const char* Filename,
                                 size_t DestCount, char* Dest)
{
  CatStrings((size_t)(State->BasePathPtr - State->BinFilename), 
             State->BinFilename,
             StringLength(Filename), Filename,
             DestCount, Dest);
}

PLATFORM_GET_FILE_LENGTH(PlatformGetFileLength) {
  char FullPath[PLATFORM_FILENAME_COUNT];
  LinuxBuildExecutablePathFilename(&LinuxState, FilePath,
                                   sizeof(FullPath), FullPath);

  i32 InputFD = open(FullPath, O_RDONLY);
  if (InputFD < 0) return 0;

  struct stat StatBuf;
  if (fstat(InputFD, &StatBuf) < 0) {
    close(InputFD);
    return 0;
  }

  close(InputFD);

  off_t FileSize = StatBuf.st_size;
  return ((u32)FileSize + 1);
}

PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile) {
  char FullPath[PLATFORM_FILENAME_COUNT];
  LinuxBuildExecutablePathFilename(&LinuxState, FilePath,
                                   sizeof(FullPath), FullPath);

  i32 InputFD = open(FullPath, O_RDONLY);
  if (InputFD < 0) return;

  size_t TotalBytesRead = 0;
  while (TotalBytesRead < Length) {
    ssize_t BytesRead = read(InputFD, Output + TotalBytesRead, Length - TotalBytesRead);
    if (BytesRead < 0) {
      perror("Error reading file");
      close(InputFD);
      return;
    }
    if (BytesRead <= 0) {
      break;
    }
    TotalBytesRead += (size_t)BytesRead;
  }

  Output[TotalBytesRead] = '\0';

  close(InputFD);
}

PLATFORM_WRITE_TO_FILE(PlatformWriteToFile) {
  char FullPath[PLATFORM_FILENAME_COUNT];
  LinuxBuildExecutablePathFilename(&LinuxState, FilePath,
                                   sizeof(FullPath), FullPath);
  
  i32 OutputFD = open(FullPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (OutputFD < 0) return;

  ssize_t BytesWritten = write(OutputFD, Input, Length - 1);

  if (BytesWritten == -1) {
    perror("Error writing to file");
  } else {
    printf("Successfully wrote %ld bytes.\n", (long)BytesWritten);
  }

  close(OutputFD);
}

PLATFORM_LOAD_TEXTURE(PlatformLoadTexture) {
  texture_id TextureID = { };
  if (GetPlatformState().TexturesCount >= MAX_TEXTURES) {
    return TextureID;
  }
  texture_file& TextureFile = GetPlatformState().Textures[GetPlatformState().TexturesCount];
  LinuxBuildExecutablePathFilename(&LinuxState, TexturePath,
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
  LinuxBuildExecutablePathFilename(&LinuxState, VertPath,
                                   sizeof(ShaderFile.VertexFilename), 
                                   ShaderFile.VertexFilename);
  LinuxBuildExecutablePathFilename(&LinuxState, FragPath,
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
  LinuxBuildExecutablePathFilename(&LinuxState, SoundPath,
                                   sizeof(SoundFile.Filename),
                                   SoundFile.Filename);
  if (SoundFile.Buffer.loadFromFile(SoundFile.Filename)) {
    SoundFile.Sound.setBuffer(SoundFile.Buffer);
    SoundID.Handle = GetPlatformState().SoundsCount++;
  }
  return SoundID;
}
