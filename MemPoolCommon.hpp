#pragma once

#include <climits>
#include <cstddef>

#define MEMPOOL_LIKELY(x) __builtin_expect((x), 1)
#define MEMPOOL_UNLIKELY(x) __builtin_expect((x), 0)

template<size_t SIZE>
struct MemPoolBlock
{
    union
    {
        char m_Data[SIZE];
    };
    static void check() { static_assert(sizeof(MemPoolBlock) == SIZE, "Smth went wrong"); }
};

inline constexpr size_t memPoolLog2(size_t a) noexcept
{
    return a <= 1 ? 0 : 1 + memPoolLog2(a / 2);
}

inline constexpr size_t memPoolCeil2(size_t a) noexcept
{
    return 0 == (a & (a - 1)) ? a : size_t(2) << memPoolLog2(a);
}

template <size_t SIZE>
class MemPoolBitset
{
    using type_t = unsigned long long; // corresponds to __builtin_ctzll(..)
    static constexpr size_t TYPE_BITS = sizeof(type_t) * CHAR_BIT;
    static constexpr type_t ZERO = 0;
    static constexpr type_t ONE = 1;
    static constexpr type_t LAST = ONE << (TYPE_BITS - 1);
    static constexpr type_t MAX = LAST | (LAST - 1);
    static constexpr size_t SHIFT = memPoolLog2(TYPE_BITS);
    static constexpr size_t MASK = TYPE_BITS - 1;
    static constexpr size_t NUM_QWORDS = (SIZE + TYPE_BITS - 1) / TYPE_BITS;
    static_assert(SIZE > 0, "Use nothing instead");
    static_assert(SIZE <= TYPE_BITS * TYPE_BITS, "Size too big for two levels");

public:
    MemPoolBitset()
    {
        m_Level0 = MAX;
        for (size_t i = 0; i < NUM_QWORDS; i++)
            m_Level1[i] = MAX;
    }
    void set(size_t aBit)
    {
        size_t sPos0 = aBit >> SHIFT;
        size_t sPos1 = aBit & MASK;
        m_Level0 |= ONE << sPos0;
        m_Level1[sPos0] |= ONE << sPos1;
    }
    size_t grab()
    {
        size_t sPos0 = __builtin_ctzll(m_Level0);
        size_t sPos1 = __builtin_ctzll(m_Level1[sPos0]);
        m_Level1[sPos0] ^= ONE << sPos1;
        if (m_Level1[sPos0] == 0)
            m_Level0 ^= ONE << sPos0;
//        m_Level0 ^= (m_Level1[sPos0] ? ZERO : ONE) << sPos0;
        return (sPos0 << SHIFT) | sPos1;
    }

private:
    uint64_t m_Level0;
    uint64_t m_Level1[NUM_QWORDS];
};
