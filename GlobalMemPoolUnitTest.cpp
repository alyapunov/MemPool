#include <GlobalMemPool.hpp>

#include <iostream>
#include <unordered_map>
#include <vector>

int rc = 0;

void check(bool exp, const char* funcname, const char *filename, int line)
{
    if (!exp)
    {
        rc = 1;
        std::cerr << "Check failed in " << funcname << " at " << filename << ":" << line << std::endl;
    }
}

template<class T>
void check(const T& x, const T& y, const char* funcname, const char *filename, int line)
{
    if (x != y)
    {
        rc = 1;
        std::cerr << "Check failed: " << x << " != " << y <<  " in " << funcname << " at " << filename << ":" << line << std::endl;
    }
}


#define CHECK(...) check(__VA_ARGS__, __func__, __FILE__, __LINE__)

struct Announcer
{
    const char* m_Func;
    explicit Announcer(const char* aFunc) : m_Func(aFunc) { std::cout << "Test " << m_Func << " started" << std::endl; }
    ~Announcer() { std::cout << "Test " << m_Func << " finished" << std::endl; }
};

#define ANNOUNCE() Announcer sAnn(__func__)

struct AnnouncerTmpl
{
    const char* m_Func;
    size_t m_Tmpl;
    explicit AnnouncerTmpl(const char* aFunc, size_t aTmpl) : m_Func(aFunc), m_Tmpl(aTmpl) { std::cout << "Test " << m_Func << "<" << m_Tmpl << "> started" << std::endl; }
    ~AnnouncerTmpl() { std::cout << "Test " << m_Func << "<" << m_Tmpl << "> finished" << std::endl; }
};

#define ANNOUNCE_TMPL(N) AnnouncerTmpl sAnn(__func__, N)

static void compile_test()
{
    ANNOUNCE();
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
    GlobalMemPool<60>::alloc();
    GlobalMemPool<61>::alloc();
    GlobalMemPool<62>::alloc();
    GlobalMemPool<63>::alloc();
    GlobalMemPool<64>::alloc();
    GlobalMemPool<65>::alloc();
    GlobalMemPool<66>::alloc();
    GlobalMemPool<67>::alloc();
    GlobalMemPool<68>::alloc();
    GlobalMemPool<69>::alloc();
    GlobalMemPool<70>::alloc();
}

template <size_t S>
struct Thing
{
    unsigned char m_Arr[S];
    static_assert(S >= sizeof(size_t), "Size is too small");
    void set(size_t aNum)
    {
        for (size_t i = 0; i < sizeof(size_t); i++)
        {
            m_Arr[i] = static_cast<unsigned char>(aNum);
            aNum >>= 8;
        }
        for (size_t i = sizeof(size_t); i < S; i++)
        {
            m_Arr[i] = m_Arr[i - sizeof(size_t)] + static_cast<unsigned char>(1);
        }
    }
    bool good(size_t aNum) const
    {
        for (size_t i = 0; i < sizeof(size_t); i++)
        {
            if (m_Arr[i] != static_cast<unsigned char>(aNum))
                return false;
            aNum >>= 8;
        }
        for (size_t i = sizeof(size_t); i < S; i++)
        {
            unsigned char sCmp = m_Arr[i - sizeof(size_t)] + static_cast<unsigned char>(1);
            if (m_Arr[i] != sCmp)
                return false;
        }
        return true;
    }
    Thing(size_t aNum) { set(aNum); }
    void* operator new(size_t)
    {
        return GlobalMemPool<sizeof(Thing)>::alloc();
    }
    void operator delete(void* aPtr)
    {
        GlobalMemPool<sizeof(Thing)>::free(static_cast<char*>(aPtr));
    }
};

template <size_t S>
static void seq_test()
{
    ANNOUNCE_TMPL(S);
    size_t sSlabCountOrig = GlobalMemPool<S>::slabCount();
    const size_t N = 8 * 1024;
    std::unordered_map<size_t, Thing<S>*> sMap;
    for (size_t i = 0; i < N; i++)
    {
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        sMap[i] = new Thing<S>(i);
        CHECK(GlobalMemPool<S>::slabCount() > sSlabCountOrig);
    }
    for (size_t i = 0; i < N; i++)
    {
        delete sMap[i];
        sMap.erase(i);
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        CHECK(GlobalMemPool<S>::slabCount() > sSlabCountOrig);
    }
    CHECK(GlobalMemPool<S>::slabCount() <= GlobalMemPool<S>::MAX_MMAP_SLABS + 1);
}

template <size_t S>
static void seq_rev_test()
{
    ANNOUNCE_TMPL(S);
    size_t sSlabCountOrig = GlobalMemPool<S>::slabCount();
    const size_t N = 8 * 1024;
    std::unordered_map<size_t, Thing<S>*> sMap;
    for (size_t i = 0; i < N; i++)
    {
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        sMap[i] = new Thing<S>(i);
        CHECK(GlobalMemPool<S>::slabCount() > sSlabCountOrig);
    }
    for (size_t k = 0; k < N; k++)
    {
        size_t i = N - 1 - k;
        delete sMap[i];
        sMap.erase(i);
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        CHECK(GlobalMemPool<S>::slabCount() > sSlabCountOrig);
    }
    CHECK(GlobalMemPool<S>::slabCount() <= GlobalMemPool<S>::MAX_MMAP_SLABS + 1);
}

template <size_t S>
static void rand_test()
{
    ANNOUNCE_TMPL(S);
    size_t sSlabCountOrig = GlobalMemPool<S>::slabCount();
    const size_t N = 8 * 1024;
    const size_t M = N / 16;
    std::unordered_map<size_t, Thing<S>*> sMap;
    for (size_t i = 0; i < N; i++)
    {
        sMap[i] = new Thing<S>(i);
        CHECK(GlobalMemPool<S>::slabCount() > sSlabCountOrig);
    }

    for (size_t i = 0; i < N; i++)
    {
        int c = rand() % M;
        std::vector<size_t> xs;
        xs.reserve(c);
        for (int j = 0; j < c; j++)
        {
            size_t x = rand() % M;
            if (sMap.find(x) != sMap.end())
            {
                delete sMap[x];
                sMap.erase(x);
                xs.push_back(x);
            }
        }
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        for (size_t x : xs)
            sMap[x] = new Thing<S>(x);
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
    }

    for (size_t i = 0; i < N; i++)
    {
        delete sMap[i];
        sMap.erase(i);
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        CHECK(GlobalMemPool<S>::slabCount() > sSlabCountOrig);
    }
    CHECK(GlobalMemPool<S>::slabCount() <= GlobalMemPool<S>::MAX_MMAP_SLABS + 1);
}


int main()
{
    if (time(nullptr) == 0)
        compile_test();

    seq_test<64>();
    seq_rev_test<65>();
    rand_test<66>();
    seq_test<200>();
    seq_rev_test<201>();
    rand_test<202>();

    return rc;
}