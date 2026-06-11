inline FILETIME
LinuxGetLastWriteTime(const char* Filename) {
    FILETIME LastWriteTime = {};

    WIN32_FIND_DATA FindData;
    HANDLE FileHandle = FindFirstFileA(Filename, &FindData);
    if (FindHandle != INVALID_HANDLE_VALUE) {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }

    return (LastWriteTime);
}

