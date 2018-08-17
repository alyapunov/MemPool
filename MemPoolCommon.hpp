#pragma once

#include <cstddef>

template<size_t SIZE>
struct MemPoolBlock
{
    union
    {
        char m_Data[SIZE];
        MemPoolBlock* m_Next;
    };
    static void check() { static_assert(sizeof(MemPoolBlock) == SIZE, "Smth went wrong"); }
};

inline constexpr size_t memPoolLog2(size_t a)
{
    return a <= 1 ? 0 : 1 + memPoolLog2(a / 2);
}

inline constexpr size_t memPoolCeil2(size_t a)
{
    return 0 == (a & (a - 1)) ? a : size_t(2) << memPoolLog2(a);
}