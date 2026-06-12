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

internal void
Win32GetExecutableFilename(win32_state* State) {
    DWORD SizeOfFilename = GetModuleFilename(0, State->ExeFilename, sizeof(State->ExeFilename));
    State->BasePath = State->ExeFilename;
    for (char* Scan = State->ExeFilename; *Scan; ++Scan) {
        if (*Scan == '\\') {
            State->BasePath = Scan + 1;
        }
    }
}

internal void
Win32BuildExecutablePathFilename(win32_state* State, const char* Filename,
                                 size_t DestCount, char* Dest)
{
    CatStrings((size_t)(State->BasePath - State->ExeFilename),
               State->ExeFilename,
               StringLength(Filename), Filename,
               DestCount, Dest);
}

