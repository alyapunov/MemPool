#pragma once

#include <GlobalMemPool.hpp>

template <size_t SIZE>
class LocalMemPool
{
public:
    static LocalMemPool& instance() { static thread_local LocalMemPool sInst; return sInst; }

    char* alloc()
    {
        if (MEMPOOL_UNLIKELY(0 == m_Pos))
        {
            std::lock_guard<std::mutex> sLock(m_Global.mutex());
            do
            {
                m_Blocks[m_Pos] = m_Global.alloc();
                m_Pos++;
            } while(m_Pos != CACHE_SIZE);
        }
        return m_Blocks[--m_Pos];
    }

    void free(char* aBlock)
    {
        if (MEMPOOL_UNLIKELY(CACHE_SIZE * 2 == m_Pos))
        {
            std::lock_guard<std::mutex> sLock(m_Global.mutex());
            do
            {
                m_Global.free(m_Blocks[--m_Pos]);
            } while (m_Pos != CACHE_SIZE);
        }
        m_Blocks[m_Pos++] = aBlock;
    }

private:
    LocalMemPool() : m_Global(GlobalMemPool<SIZE>::instance()) {}

    GlobalMemPool<SIZE>& m_Global;
    static constexpr size_t CACHE_SIZE = 16;
    char* m_Blocks[CACHE_SIZE * 2];
    size_t m_Pos = 0;

};