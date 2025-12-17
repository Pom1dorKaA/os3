#pragma once
#include <cstdint>

static constexpr std::size_t SHM_SIZE = 4096;

struct SharedBlock {
    std::uint32_t length;
    char text[SHM_SIZE - sizeof(std::uint32_t)];
};
