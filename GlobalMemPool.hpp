#pragma once

#include <sys/mman.h>

#include <mutex>

#include <AutoList.hpp>
#include <MemPoolCommon.hpp>
#include <SlightlyOrderedList.hpp>

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
    static std::mutex& mutex() { return instance().internalMutex(); }
    static char* alloc() { return instance().internalAlloc(); }
    static void free(char* aBlock) { return instance().internalFree(aBlock); }
    static size_t slabCount() { return instance().m_SlabCount; }
    static size_t freeCount() { return instance().m_FreeCount; }

private:
    using Block = MemPoolBlock<SIZE>;
    static constexpr size_t BLOCK_SIZE = sizeof(Block);
    // Mempool calls mmap approximately once per SYSCALL_DIVIDER allocations.
    // By my measurements syscall performance is about 3Mrps.
    // Set BLOCKS_IN_SLAB big enough avoiding making the mmap call a bottleneck.
    static constexpr size_t SYSCALL_DIVIDER = 512;
public:
    static constexpr size_t SLAB_SIZE = memPoolCeil2(BLOCK_SIZE * SYSCALL_DIVIDER);
private:

    struct Slab;
    struct SlabHeader
    {
        AutoListLink m_AllLink;
        SlightlyOrderedListLink m_FreeLink;
        Block* m_FreeList = nullptr;
        size_t m_FreeCount = 0;
        Slab* m_NextNew = nullptr;
        bool m_IsInFreeList = false;
        void* operator new(size_t) = delete;
        void operator delete(void*) = delete;
    };

    static constexpr size_t PADDING_SIZE = (SLAB_SIZE - sizeof(SlabHeader)) % BLOCK_SIZE;
public:
    static constexpr size_t BLOCKS_IN_SLAB = (SLAB_SIZE - sizeof(SlabHeader)) / BLOCK_SIZE;
    static_assert(BLOCKS_IN_SLAB > 1, "Too small slab");
    static constexpr size_t MAX_MMAP_SLABS = 4;
private:
    static constexpr size_t MOVE_GROUP_THRESHOLD_PERCENT = 4;
    struct Slab : SlabHeader, GlobalMemPoolPadding<PADDING_SIZE>
    {
        Block m_Blocks[BLOCKS_IN_SLAB];
        void* operator new(size_t) = delete;
        void* operator new(size_t, void* sPtr) { return sPtr; }
        void operator delete(void* sPtr) { munmap(sPtr, SLAB_SIZE); }
    };
    static_assert(sizeof(Slab) == SLAB_SIZE, "Smth went wrong");

    static GlobalMemPool& instance() { static GlobalMemPool sInst; return sInst; }

    GlobalMemPool() = default;
    ~GlobalMemPool()
    {
        while(!m_AllList.empty())
            delete static_cast<Slab*>(&m_AllList.front());
    }

    std::mutex& internalMutex() { return m_Mutex; }

    static char* mmap(size_t aSize)
    {
        const int PROT = PROT_READ | PROT_WRITE;
        const int FLAGS = MAP_PRIVATE | MAP_ANONYMOUS;
        return static_cast<char*>(::mmap(nullptr, aSize, PROT, FLAGS, -1, 0));
    }

    Slab* newSlab(char* aMem)
    {
        Slab* sSlab = new (aMem) Slab;
        m_AllList.insertBack(*sSlab);
        ++m_SlabCount;
        return sSlab;
    }

    char* internalAlloc()
    {
        if (MEMPOOL_UNLIKELY(0 == m_FreeCount && nullptr == m_NewSlab))
            return mmapAndAlloc();
        if (MEMPOOL_UNLIKELY(nullptr != m_NewSlab))
        {
            char* sRes = m_NewSlab->m_Blocks[m_NewSlabPos].m_Data;
            if (MEMPOOL_UNLIKELY(BLOCKS_IN_SLAB == ++m_NewSlabPos))
            {
                m_NewSlab = m_NewSlab->m_NextNew;
                m_NewSlabPos = 0;
            }
            return sRes;
        }
        --m_FreeCount;
        SlabHeader* sSlab = &m_FreeList[m_FreeList[0].empty()].front();
        char* sRes = sSlab->m_FreeList->m_Data;
        sSlab->m_FreeList = sSlab->m_FreeList->m_Next;
        --sSlab->m_FreeCount;
        const size_t MOVE_GROUP_TRESHOLD = BLOCKS_IN_SLAB * (50 - MOVE_GROUP_THRESHOLD_PERCENT) / 100;
        if (MEMPOOL_UNLIKELY(0 == sSlab->m_FreeCount))
        {
            assert(!m_FreeList[0].empty());
            m_FreeList[0].remove(*sSlab);
            sSlab->m_IsInFreeList = false;
        }
        else if (MOVE_GROUP_TRESHOLD == sSlab->m_FreeCount)
        {
            assert(m_FreeList[0].empty());
            m_FreeList[1].remove(*sSlab);
            m_FreeList[0].insert(*sSlab);
        }
        return sRes;
    }

    char* mmapAndAlloc()
    {
        size_t sCount = 1;
        char* sPtr = mmap(SLAB_SIZE);
        uintptr_t sAddr = reinterpret_cast<uintptr_t>(sPtr);
        if (MAP_FAILED == sPtr || 0 != sAddr % SLAB_SIZE)
        {
            if (MEMPOOL_LIKELY(MAP_FAILED != sPtr))
                munmap(sPtr, SLAB_SIZE);
            sPtr = mmap(MAX_MMAP_SLABS * SLAB_SIZE);
            sAddr = reinterpret_cast<uintptr_t>(sPtr);
            if (MEMPOOL_UNLIKELY(MAP_FAILED == sPtr))
                throw std::bad_alloc();
            sCount = MAX_MMAP_SLABS;
            if (MEMPOOL_LIKELY(0 != sAddr % SLAB_SIZE))
            {
                --sCount;
                size_t sPostCut = sAddr % SLAB_SIZE;
                size_t sPreCut = SLAB_SIZE - sPostCut;
                munmap(sPtr, sPreCut);
                sPtr += sPreCut;
                munmap(sPtr + sCount * SLAB_SIZE, sPostCut);
            }
        }
        Slab* sSlab = m_NewSlab = newSlab(sPtr);
        for (size_t i = 1; i < sCount; i++)
            sSlab = sSlab->m_NextNew = newSlab(sPtr + i * SLAB_SIZE);
        sSlab->m_NextNew = nullptr;
        m_NewSlabPos = 1;
        return m_NewSlab->m_Blocks[0].m_Data;
    }

    void internalFree(char* aBlock)
    {
        Block* sBlock = reinterpret_cast<Block*>(aBlock);
        Slab* sSlab = slabByBlock(aBlock);
        sBlock->m_Next = sSlab->m_FreeList;
        sSlab->m_FreeList = sBlock;
        size_t sWasFreeCount = sSlab->m_FreeCount++;
        if (MEMPOOL_LIKELY(sSlab->m_IsInFreeList))
            ++m_FreeCount;
        const size_t INTRODUCE_GROUP_TRESHOLD = BLOCKS_IN_SLAB * MOVE_GROUP_THRESHOLD_PERCENT / 100;
        const size_t MOVE_GROUP_TRESHOLD = BLOCKS_IN_SLAB * (50 + MOVE_GROUP_THRESHOLD_PERCENT) / 100;
        if (MEMPOOL_UNLIKELY(INTRODUCE_GROUP_TRESHOLD == sWasFreeCount && !sSlab->m_IsInFreeList))
        {
            m_FreeCount += sSlab->m_FreeCount;
            m_FreeList[0].insert(*sSlab);
            sSlab->m_IsInFreeList = true;
        }
        else if (MEMPOOL_UNLIKELY(MOVE_GROUP_TRESHOLD == sWasFreeCount))
        {
            m_FreeList[0].remove(*sSlab);
            m_FreeList[1].insert(*sSlab);
        }
        else if(MEMPOOL_UNLIKELY(BLOCKS_IN_SLAB == sSlab->m_FreeCount && m_FreeCount >= 2 * BLOCKS_IN_SLAB))
        {
            m_FreeList[1].remove(*sSlab);
            m_FreeCount -= BLOCKS_IN_SLAB;
            --m_SlabCount;
            delete sSlab;
        }
    }

    Slab* slabByBlock(char* aBlock)
    {
        uintptr_t sAddr = reinterpret_cast<uintptr_t>(aBlock);
        uintptr_t sLowMask = SLAB_SIZE - 1;
        sAddr &= ~sLowMask;
        return reinterpret_cast<Slab*>(sAddr);
    }

    std::mutex m_Mutex;
    AutoList<SlabHeader, &SlabHeader::m_AllLink> m_AllList;
    // 0 - more than half full (but not 100% full), 1 - less than half full .
    // e.g. 0 : m_FreeCount <= BLOCKS_IN_SLAB / 2; 1 : m_FreeCount > BLOCKS_IN_SLAB / 2.
    // Actually that boundaries are approximate, the move from one group to another may be delayed.
    SlightlyOrderedList<SlabHeader, &SlabHeader::m_FreeLink, SLAB_SIZE> m_FreeList[2];
    size_t m_SlabCount = 0;
    size_t m_FreeCount = 0;
    Slab* m_NewSlab;
    size_t m_NewSlabPos;
};