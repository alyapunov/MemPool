#pragma once

#include <GlobalMemPool.hpp>

template <size_t SIZE>
class LocalMemPool
{
public:
    static char* alloc() { return instance().internalAlloc(); }
    static void free(char* aBlock) { return instance().internalFree(aBlock); }

private:
    static constexpr size_t CACHE_SIZE = 16;
    char* m_Blocks[CACHE_SIZE * 2];
    size_t m_Pos = 0;

    static LocalMemPool& instance() { static thread_local LocalMemPool sInst; return sInst; }

    char* internalAlloc()
    {
        if (MEMPOOL_UNLIKELY(0 == m_Pos))
        {
            GlobalMemPool<SIZE>& sGlobal = GlobalMemPool<SIZE>::instance();
            std::lock_guard<std::mutex> sLock(sGlobal.internalMutex());
            do
            {
                m_Blocks[m_Pos] = sGlobal.internalAlloc();
                m_Pos++;
            } while(m_Pos != CACHE_SIZE);
        }
        return m_Blocks[--m_Pos];
    }

    void internalFree(char* aBlock)
    {
        if (MEMPOOL_UNLIKELY(CACHE_SIZE * 2 == m_Pos))
        {
            GlobalMemPool<SIZE>& sGlobal = GlobalMemPool<SIZE>::instance();
            std::lock_guard<std::mutex> sLock(sGlobal.internalMutex());
            do
            {
                sGlobal.internalFree(m_Blocks[--m_Pos]);
            } while (m_Pos != CACHE_SIZE);
        }
        m_Blocks[m_Pos++] = aBlock;
    }
};