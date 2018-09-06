#include <LocalMemPool.hpp>

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
    LocalMemPool<1>::alloc();
    LocalMemPool<2>::alloc();
    LocalMemPool<3>::alloc();
    LocalMemPool<4>::alloc();
    LocalMemPool<5>::alloc();
    LocalMemPool<6>::alloc();
    LocalMemPool<7>::alloc();
    LocalMemPool<8>::alloc();
    LocalMemPool<9>::alloc();
    LocalMemPool<10>::alloc();
    LocalMemPool<11>::alloc();
    LocalMemPool<12>::alloc();
    LocalMemPool<13>::alloc();
    LocalMemPool<14>::alloc();
    LocalMemPool<15>::alloc();
    LocalMemPool<16>::alloc();
    LocalMemPool<17>::alloc();
    LocalMemPool<18>::alloc();
    LocalMemPool<19>::alloc();
    LocalMemPool<20>::alloc();
    LocalMemPool<21>::alloc();
    LocalMemPool<22>::alloc();
    LocalMemPool<23>::alloc();
    LocalMemPool<24>::alloc();
    LocalMemPool<25>::alloc();
    LocalMemPool<26>::alloc();
    LocalMemPool<27>::alloc();
    LocalMemPool<28>::alloc();
    LocalMemPool<29>::alloc();
    LocalMemPool<30>::alloc();
    LocalMemPool<31>::alloc();
    LocalMemPool<32>::alloc();
    LocalMemPool<33>::alloc();
    LocalMemPool<34>::alloc();
    LocalMemPool<35>::alloc();
    LocalMemPool<36>::alloc();
    LocalMemPool<37>::alloc();
    LocalMemPool<38>::alloc();
    LocalMemPool<39>::alloc();
    LocalMemPool<40>::alloc();
    LocalMemPool<41>::alloc();
    LocalMemPool<42>::alloc();
    LocalMemPool<43>::alloc();
    LocalMemPool<44>::alloc();
    LocalMemPool<45>::alloc();
    LocalMemPool<46>::alloc();
    LocalMemPool<47>::alloc();
    LocalMemPool<48>::alloc();
    LocalMemPool<49>::alloc();
    LocalMemPool<50>::alloc();
    LocalMemPool<51>::alloc();
    LocalMemPool<52>::alloc();
    LocalMemPool<53>::alloc();
    LocalMemPool<54>::alloc();
    LocalMemPool<55>::alloc();
    LocalMemPool<56>::alloc();
    LocalMemPool<57>::alloc();
    LocalMemPool<58>::alloc();
    LocalMemPool<59>::alloc();
    LocalMemPool<60>::alloc();
    LocalMemPool<61>::alloc();
    LocalMemPool<62>::alloc();
    LocalMemPool<63>::alloc();
    LocalMemPool<64>::alloc();
    LocalMemPool<65>::alloc();
    LocalMemPool<66>::alloc();
    LocalMemPool<67>::alloc();
    LocalMemPool<68>::alloc();
    LocalMemPool<69>::alloc();
    LocalMemPool<70>::alloc();
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
        return LocalMemPool<sizeof(Thing)>::alloc();
    }
    void operator delete(void* aPtr)
    {
        LocalMemPool<sizeof(Thing)>::free(static_cast<char*>(aPtr));
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