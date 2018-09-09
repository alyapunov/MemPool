#include <LocalMemPool.hpp>

#include <chrono>
#include <iostream>
#include <vector>

size_t side_effect = 0;

static void checkpoint(const char* aText, size_t aOpCount)
{
    using namespace std::chrono;
    high_resolution_clock::time_point now = high_resolution_clock::now();
    static high_resolution_clock::time_point was;
    duration<double> time_span = duration_cast<duration<double>>(now - was);
    if (0 != aOpCount)
    {
        double Mrps = aOpCount / 1000000. / time_span.count();
        std::cout << aText << ": " << Mrps << " Mrps" << std::endl;
    }
    was = now;
}

int main()
{
    const size_t N = 16 * 1024 * 1024;
    using All = LocalMemPool<64>;
    All& sAllInst = All::instance();
    using Gall = GlobalMemPool<64>;
    Gall& sGallInst = Gall::instance();
    std::vector<char*> sAll(N, nullptr);
    checkpoint("", 0);

    for (size_t i = 0; i < N; i++)
        sAll[i] = sAllInst.alloc();
    checkpoint("Alloc", N);

    for (size_t i = 0; i < N; i++)
        sAllInst.free(sAll[i]);
    checkpoint("Free", N);

    for (size_t i = 0; i < N; i++)
        sAll[i] = sAllInst.alloc();
    checkpoint("Alloc", N);

    for (size_t i = 0; i < N; i++)
        sAllInst.free(sAll[i]);
    checkpoint("Free", N);

    for (size_t i = 0; i < N; i++)
    {
        sAll[i] = sAllInst.alloc();
        sAll[i][0] = static_cast<char>(i);
    }
    checkpoint("Alloc touch", N);

    for (size_t i = 0; i < N; i++)
    {
        if (i + 32 < N && i % 32 == 0)
        {
            for (size_t j = 0; j < 32; j++)
            {
                __builtin_prefetch(sAll[i + j], 1);
                uintptr_t sAddr = reinterpret_cast<uintptr_t>(sAll[i + j]);
                sAddr &= ~(Gall::SLAB_SIZE - 1);
                __builtin_prefetch(reinterpret_cast<void*>(sAddr), 1);
            }
        }
        side_effect ^= sAll[i][0];
        sAllInst.free(sAll[i]);
    }
    checkpoint("Free prefetch", N);

    for (size_t i = 0; i < N; i++)
        sAll[i] =sAllInst.alloc();
    checkpoint("Alloc", N);

    for (size_t i = 0; i < N; i += 4)
    {
        sAllInst.free(sAll[i]);
        sAllInst.free(sAll[i + 1]);
        sAllInst.free(sAll[i + 2]);
    }
    checkpoint("Free sparse", N * 3 / 4);

    for (size_t i = 0; i < N; i += 4)
    {
        sAll[i] = sAllInst.alloc();
        sAll[i + 1] = sAllInst.alloc();
        sAll[i + 2] = sAllInst.alloc();
    }
    checkpoint("Alloc sparse", N * 3 / 4);

    for (size_t i = 0; i < N; i++)
        sAllInst.free(sAll[i]);
    checkpoint("Free", N);

    std::cout << "slabCount = " << sGallInst.slabCount() << std::endl;
    std::cout << "freeCount = " << sGallInst.freeCount() << std::endl;

    std::cout << "Side effect (ignore it): " << (side_effect & 1) << std::endl;
}
