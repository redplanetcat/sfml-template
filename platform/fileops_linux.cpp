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

//#define PLATFORM_LOAD_SHADER(name) u32 name(char* VertPath, char* FragPath);
//typedef PLATFORM_LOAD_SHADER(platform_load_shader_t);
PLATFORM_LOAD_SHADER(PlatformLoadShader) {
  if (GetPlatformState().ShadersCount >= MAX_SHADERS) {
    return 0;
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
    return GetPlatformState().ShadersCount++;
  }
  return 0;
}

