#include <GlobalMemPool.hpp>

#include <alya.h>

int main()
{
    const size_t SIZE = 4 * 1024 * 1024;
    void* ptr = mmap(0, SIZE * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    COUTF(ptr);
    uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t next_p;
    if (p % SIZE == 0)
    {
        COUT("cool!");
        next_p = p + 2 * SIZE;
    }
    else
    {
        COUT("cutting");
        uintptr_t post = p % SIZE;
        uintptr_t pre = SIZE - post;
        next_p = p + pre + SIZE;
        int r1 = munmap(reinterpret_cast<void*>(p), pre);
        int r2 = munmap(reinterpret_cast<void*>(next_p), post);
        COUTF(r1, r2);
    }

    COUTF(reinterpret_cast<void*>(next_p));
    ptr = mmap(reinterpret_cast<void*>(next_p), SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    COUTF(ptr);
    next_p += SIZE;

    COUTF(reinterpret_cast<void*>(next_p));
    ptr = mmap(reinterpret_cast<void*>(next_p), SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    COUTF(ptr);
    next_p += SIZE;

    COUTF(reinterpret_cast<void*>(next_p));
    ptr = mmap(reinterpret_cast<void*>(next_p), SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    COUTF(ptr);
    next_p += SIZE;

    GlobalMemPool<1>::alloc();
    GlobalMemPool<2>::alloc();
    GlobalMemPool<3>::alloc();
    GlobalMemPool<4>::alloc();
    GlobalMemPool<5>::alloc();
    GlobalMemPool<6>::alloc();
    GlobalMemPool<7>::alloc();
    GlobalMemPool<8>::alloc();
    GlobalMemPool<9>::alloc();
    GlobalMemPool<10>::alloc();
    GlobalMemPool<11>::alloc();
    GlobalMemPool<12>::alloc();
    GlobalMemPool<13>::alloc();
    GlobalMemPool<14>::alloc();
    GlobalMemPool<15>::alloc();
    GlobalMemPool<16>::alloc();
    GlobalMemPool<17>::alloc();
    GlobalMemPool<18>::alloc();
    GlobalMemPool<19>::alloc();
    GlobalMemPool<20>::alloc();
    GlobalMemPool<21>::alloc();
    GlobalMemPool<22>::alloc();
    GlobalMemPool<23>::alloc();
    GlobalMemPool<24>::alloc();
    GlobalMemPool<25>::alloc();
    GlobalMemPool<26>::alloc();
    GlobalMemPool<27>::alloc();
    GlobalMemPool<28>::alloc();
    GlobalMemPool<29>::alloc();
    GlobalMemPool<30>::alloc();
    GlobalMemPool<31>::alloc();
    GlobalMemPool<32>::alloc();
    GlobalMemPool<33>::alloc();
    GlobalMemPool<34>::alloc();
    GlobalMemPool<35>::alloc();
    GlobalMemPool<36>::alloc();
    GlobalMemPool<37>::alloc();
    GlobalMemPool<38>::alloc();
    GlobalMemPool<39>::alloc();
    GlobalMemPool<40>::alloc();
    GlobalMemPool<41>::alloc();
    GlobalMemPool<42>::alloc();
    GlobalMemPool<43>::alloc();
    GlobalMemPool<44>::alloc();
    GlobalMemPool<45>::alloc();
    GlobalMemPool<46>::alloc();
    GlobalMemPool<47>::alloc();
    GlobalMemPool<48>::alloc();
    GlobalMemPool<49>::alloc();
    GlobalMemPool<50>::alloc();
    GlobalMemPool<51>::alloc();
    GlobalMemPool<52>::alloc();
    GlobalMemPool<53>::alloc();
    GlobalMemPool<54>::alloc();
    GlobalMemPool<55>::alloc();
    GlobalMemPool<56>::alloc();
    GlobalMemPool<57>::alloc();
    GlobalMemPool<58>::alloc();
    GlobalMemPool<59>::alloc();
}