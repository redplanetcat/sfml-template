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

