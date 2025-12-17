#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstddef>
#include <string>

[[noreturn]] void Die(const char* where);

HANDLE MappingCreate(const char* name, std::size_t sizeBytes);
HANDLE MappingOpen(const char* name);
void*  MappingMap(HANDLE hMap, std::size_t sizeBytes);
void   MappingUnmap(void* view);

HANDLE EventCreate(const char* name, bool manualReset, bool initialState);
HANDLE EventOpen(const char* name);
void   EventSignal(HANDLE hEvent);
DWORD  WaitOne(HANDLE h, DWORD ms);

PROCESS_INFORMATION SpawnProcess(const std::string& exePath, const std::string& args);
