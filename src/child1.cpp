#include "os.h"
#include "shared.h"

#include <cctype>
#include <cstring>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 6) {
        std::cerr << "child1: args: <map> <evP2C1> <evC1C2> <evC2P> <evSTOP>\n";
        return 1;
    }

    const char* mapName = argv[1];
    const char* evP2C1  = argv[2];
    const char* evC1C2  = argv[3];
    const char* evSTOP  = argv[5];

    HANDLE hMap  = MappingOpen(mapName);
    HANDLE hIn   = EventOpen(evP2C1);
    HANDLE hOut  = EventOpen(evC1C2);
    HANDLE hStop = EventOpen(evSTOP);

    auto* block = static_cast<SharedBlock*>(MappingMap(hMap, sizeof(SharedBlock)));

    HANDLE waits[2] = { hStop, hIn };

    while (true) {
        DWORD r = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (r == WAIT_FAILED) Die("WaitForMultipleObjects");

        if (r == WAIT_OBJECT_0) {
            break;
        }

        for (std::uint32_t i = 0; i < block->length; ++i) {
            unsigned char ch = static_cast<unsigned char>(block->text[i]);
            block->text[i] = static_cast<char>(std::toupper(ch));
        }
        block->text[block->length] = '\0';

        EventSignal(hOut);
    }

    MappingUnmap(block);
    CloseHandle(hMap);
    CloseHandle(hIn);
    CloseHandle(hOut);
    CloseHandle(hStop);

    return 0;
}
