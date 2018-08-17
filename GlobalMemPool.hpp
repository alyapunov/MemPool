#pragma once

#include <sys/mman.h>

#include <mutex>

#include <MemPoolCommon.hpp>

template <size_t PADDING_SIZE>
struct GlobalMemPoolPadding
{
    char m_Padding[PADDING_SIZE];
};

template <>
struct GlobalMemPoolPadding<0>
{
};

template <size_t SIZE>
class GlobalMemPool
{
public:
    static std::mutex& mutex() { return instance().imutex(); }
    static char* alloc() { return instance().ialloc(); }
    static void free(char* aBlock) { return instance().free(aBlock); }

private:
    using Block = MemPoolBlock<SIZE>;
    static constexpr size_t BLOCK_SIZE = sizeof(Block);
    // Mempool calls mmap once per SYSCALL_DIVIDER allocations.
    // By my measurements syscall performance is about 3Mrps.
    // Set BLOCKS_IN_SLAB big enough avoiding making the mmap call a bottleneck.
    static constexpr size_t SYSCALL_DIVIDER = 256;
    static constexpr size_t SLAB_SIZE = memPoolCeil2(BLOCK_SIZE * SYSCALL_DIVIDER);

    struct Slab;
    struct SlabHeader
    {
        Slab* m_Next;
    };
    static constexpr size_t PADDING_SIZE = (SLAB_SIZE - sizeof(SlabHeader)) % BLOCK_SIZE;
    static constexpr size_t BLOCKS_IN_SLAB = (SLAB_SIZE - sizeof(SlabHeader)) / BLOCK_SIZE;
    struct Slab : SlabHeader, GlobalMemPoolPadding<PADDING_SIZE>
    {
        Block m_Blocks[BLOCKS_IN_SLAB];
    };
    static_assert(sizeof(Slab) == SLAB_SIZE, "Smth went wrong");

    static GlobalMemPool& instance() { static GlobalMemPool sInst; return sInst; }
    std::mutex& imutex() { return m_Mutex; }
    char* ialloc()
    {
        Slab s;
        s.m_Next = &s;
        return nullptr;
    }
    void ifree(char* aBlock);

    std::mutex m_Mutex;
};