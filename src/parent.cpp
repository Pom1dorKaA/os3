#include "os.h"
#include "shared.h"

#include <iostream>
#include <string>
#include <cstring>

static std::string MakeName(const char* base, DWORD pid) {
    return std::string("Local\\") + base + "_" + std::to_string(pid);
}

int main() {
    const DWORD pid = GetCurrentProcessId();

    const std::string mapName   = MakeName("OS3_MAP", pid);
    const std::string evP2C1    = MakeName("OS3_P2C1", pid);
    const std::string evC1C2    = MakeName("OS3_C1C2", pid);
    const std::string evC2P     = MakeName("OS3_C2P", pid);
    const std::string evSTOP    = MakeName("OS3_STOP", pid);

    HANDLE hMap   = MappingCreate(mapName.c_str(), sizeof(SharedBlock));
    HANDLE hP2C1  = EventCreate(evP2C1.c_str(), false, false);
    HANDLE hC1C2  = EventCreate(evC1C2.c_str(), false, false);
    HANDLE hC2P   = EventCreate(evC2P.c_str(),  false, false);
    HANDLE hStop  = EventCreate(evSTOP.c_str(), true,  false);

    auto* block = static_cast<SharedBlock*>(MappingMap(hMap, sizeof(SharedBlock)));
    block->length = 0;
    block->text[0] = '\0';

    const std::string args =
        mapName + " " + evP2C1 + " " + evC1C2 + " " + evC2P + " " + evSTOP;

    PROCESS_INFORMATION c1 = SpawnProcess("child1.exe", args);
    PROCESS_INFORMATION c2 = SpawnProcess("child2.exe", args);

    std::cout << "Enter lines (Ctrl+Z then Enter to stop):\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.size() >= sizeof(block->text)) {
            std::cerr << "Line too long, max=" << (sizeof(block->text) - 1) << "\n";
            continue;
        }

        block->length = static_cast<std::uint32_t>(line.size());
        std::memcpy(block->text, line.c_str(), line.size());
        block->text[line.size()] = '\0';

        EventSignal(hP2C1);

        WaitOne(hC2P, INFINITE);

        std::cout << block->text << "\n";
    }

    EventSignal(hStop);

    EventSignal(hP2C1);
    EventSignal(hC1C2);

    WaitOne(c1.hProcess, INFINITE);
    WaitOne(c2.hProcess, INFINITE);

    CloseHandle(c1.hProcess);
    CloseHandle(c2.hProcess);

    MappingUnmap(block);
    CloseHandle(hMap);
    CloseHandle(hP2C1);
    CloseHandle(hC1C2);
    CloseHandle(hC2P);
    CloseHandle(hStop);

    return 0;
}
