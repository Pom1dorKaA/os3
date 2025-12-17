#include "os.h"

#include <iostream>
#include <sstream>

[[noreturn]] void Die(const char* where) {
    DWORD e = GetLastError();
    std::ostringstream oss;
    oss << where << " failed. GetLastError=" << e;
    std::cerr << oss.str() << std::endl;
    ExitProcess(1);
}

HANDLE MappingCreate(const char* name, std::size_t sizeBytes) {
    HANDLE h = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
                                  0, static_cast<DWORD>(sizeBytes), name);
    if (!h) Die("CreateFileMappingA");
    return h;
}

HANDLE MappingOpen(const char* name) {
    HANDLE h = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
    if (!h) Die("OpenFileMappingA");
    return h;
}

void* MappingMap(HANDLE hMap, std::size_t sizeBytes) {
    void* p = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeBytes);
    if (!p) Die("MapViewOfFile");
    return p;
}

void MappingUnmap(void* view) {
    if (view && !UnmapViewOfFile(view)) Die("UnmapViewOfFile");
}

HANDLE EventCreate(const char* name, bool manualReset, bool initialState) {
    HANDLE h = CreateEventA(nullptr, manualReset ? TRUE : FALSE,
                            initialState ? TRUE : FALSE, name);
    if (!h) Die("CreateEventA");
    return h;
}

HANDLE EventOpen(const char* name) {
    HANDLE h = OpenEventA(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, name);
    if (!h) Die("OpenEventA");
    return h;
}

void EventSignal(HANDLE hEvent) {
    if (!SetEvent(hEvent)) Die("SetEvent");
}

DWORD WaitOne(HANDLE h, DWORD ms) {
    DWORD r = WaitForSingleObject(h, ms);
    if (r == WAIT_FAILED) Die("WaitForSingleObject");
    return r;
}

PROCESS_INFORMATION SpawnProcess(const std::string& exePath, const std::string& args) {
    STARTUPINFOA si{};
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi{};
    std::string cmd = "\"" + exePath + "\" " + args;

    std::string cmdMutable = cmd;

    BOOL ok = CreateProcessA(
        nullptr,
        cmdMutable.data(),
        nullptr, nullptr,
        FALSE,
        0,
        nullptr, nullptr,
        &si, &pi
    );
    if (!ok) Die("CreateProcessA");
    CloseHandle(pi.hThread);
    pi.hThread = nullptr;
    return pi;
}
