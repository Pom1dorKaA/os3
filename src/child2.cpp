#include "os.h"
#include "shared.h"

#include <cstring>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 6) {
        std::cerr << "child2: args: <map> <evP2C1> <evC1C2> <evC2P> <evSTOP>\n";
        return 1;
    }

    const char* mapName = argv[1];
    const char* evC1C2  = argv[3];
    const char* evC2P   = argv[4];
    const char* evSTOP  = argv[5];

    HANDLE hMap  = MappingOpen(mapName);
    HANDLE hIn   = EventOpen(evC1C2);
    HANDLE hOut  = EventOpen(evC2P);
    HANDLE hStop = EventOpen(evSTOP);

    auto* block = static_cast<SharedBlock*>(MappingMap(hMap, sizeof(SharedBlock)));

    HANDLE waits[2] = { hStop, hIn };

    while (true) {
        DWORD r = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (r == WAIT_FAILED) Die("WaitForMultipleObjects");

        if (r == WAIT_OBJECT_0) {
            break;
        }

        std::uint32_t w = 0;
        bool prevSpace = false;

        for (std::uint32_t i = 0; i < block->length; ++i) {
            char c = block->text[i];
            if (c == ' ') {
                if (!prevSpace) {
                    block->text[w++] = c;
                    prevSpace = true;
                }
            } else {
                block->text[w++] = c;
                prevSpace = false;
            }
        }

        block->length = w;
        block->text[w] = '\0';

        EventSignal(hOut);
    }

    MappingUnmap(block);
    CloseHandle(hMap);
    CloseHandle(hIn);
    CloseHandle(hOut);
    CloseHandle(hStop);

    return 0;
}
