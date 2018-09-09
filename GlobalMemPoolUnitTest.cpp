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
    GlobalMemPool<1>::instance().alloc();
    GlobalMemPool<2>::instance().alloc();
    GlobalMemPool<3>::instance().alloc();
    GlobalMemPool<4>::instance().alloc();
    GlobalMemPool<5>::instance().alloc();
    GlobalMemPool<6>::instance().alloc();
    GlobalMemPool<7>::instance().alloc();
    GlobalMemPool<8>::instance().alloc();
    GlobalMemPool<9>::instance().alloc();
    GlobalMemPool<10>::instance().alloc();
    GlobalMemPool<11>::instance().alloc();
    GlobalMemPool<12>::instance().alloc();
    GlobalMemPool<13>::instance().alloc();
    GlobalMemPool<14>::instance().alloc();
    GlobalMemPool<15>::instance().alloc();
    GlobalMemPool<16>::instance().alloc();
    GlobalMemPool<17>::instance().alloc();
    GlobalMemPool<18>::instance().alloc();
    GlobalMemPool<19>::instance().alloc();
    GlobalMemPool<20>::instance().alloc();
    GlobalMemPool<21>::instance().alloc();
    GlobalMemPool<22>::instance().alloc();
    GlobalMemPool<23>::instance().alloc();
    GlobalMemPool<24>::instance().alloc();
    GlobalMemPool<25>::instance().alloc();
    GlobalMemPool<26>::instance().alloc();
    GlobalMemPool<27>::instance().alloc();
    GlobalMemPool<28>::instance().alloc();
    GlobalMemPool<29>::instance().alloc();
    GlobalMemPool<30>::instance().alloc();
    GlobalMemPool<31>::instance().alloc();
    GlobalMemPool<32>::instance().alloc();
    GlobalMemPool<33>::instance().alloc();
    GlobalMemPool<34>::instance().alloc();
    GlobalMemPool<35>::instance().alloc();
    GlobalMemPool<36>::instance().alloc();
    GlobalMemPool<37>::instance().alloc();
    GlobalMemPool<38>::instance().alloc();
    GlobalMemPool<39>::instance().alloc();
    GlobalMemPool<40>::instance().alloc();
    GlobalMemPool<41>::instance().alloc();
    GlobalMemPool<42>::instance().alloc();
    GlobalMemPool<43>::instance().alloc();
    GlobalMemPool<44>::instance().alloc();
    GlobalMemPool<45>::instance().alloc();
    GlobalMemPool<46>::instance().alloc();
    GlobalMemPool<47>::instance().alloc();
    GlobalMemPool<48>::instance().alloc();
    GlobalMemPool<49>::instance().alloc();
    GlobalMemPool<50>::instance().alloc();
    GlobalMemPool<51>::instance().alloc();
    GlobalMemPool<52>::instance().alloc();
    GlobalMemPool<53>::instance().alloc();
    GlobalMemPool<54>::instance().alloc();
    GlobalMemPool<55>::instance().alloc();
    GlobalMemPool<56>::instance().alloc();
    GlobalMemPool<57>::instance().alloc();
    GlobalMemPool<58>::instance().alloc();
    GlobalMemPool<59>::instance().alloc();
    GlobalMemPool<60>::instance().alloc();
    GlobalMemPool<61>::instance().alloc();
    GlobalMemPool<62>::instance().alloc();
    GlobalMemPool<63>::instance().alloc();
    GlobalMemPool<64>::instance().alloc();
    GlobalMemPool<65>::instance().alloc();
    GlobalMemPool<66>::instance().alloc();
    GlobalMemPool<67>::instance().alloc();
    GlobalMemPool<68>::instance().alloc();
    GlobalMemPool<69>::instance().alloc();
    GlobalMemPool<70>::instance().alloc();
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
        return GlobalMemPool<sizeof(Thing)>::instance().alloc();
    }
    void operator delete(void* aPtr)
    {
        GlobalMemPool<sizeof(Thing)>::instance().free(static_cast<char*>(aPtr));
    }
};

template <size_t S>
static void seq_test()
{
    ANNOUNCE_TMPL(S);
    size_t sSlabCountOrig = GlobalMemPool<S>::instance().slabCount();
    const size_t N = 8 * 1024;
    std::unordered_map<size_t, Thing<S>*> sMap;
    for (size_t i = 0; i < N; i++)
    {
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        sMap[i] = new Thing<S>(i);
        CHECK(GlobalMemPool<S>::instance().slabCount() > sSlabCountOrig);
    }
    for (size_t i = 0; i < N; i++)
    {
        delete sMap[i];
        sMap.erase(i);
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        CHECK(GlobalMemPool<S>::instance().slabCount() > sSlabCountOrig);
    }
    CHECK(GlobalMemPool<S>::instance().slabCount() <= GlobalMemPool<S>::MAX_MMAP_SLABS + 1);
}

template <size_t S>
static void seq_rev_test()
{
    ANNOUNCE_TMPL(S);
    size_t sSlabCountOrig = GlobalMemPool<S>::instance().slabCount();
    const size_t N = 8 * 1024;
    std::unordered_map<size_t, Thing<S>*> sMap;
    for (size_t i = 0; i < N; i++)
    {
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        sMap[i] = new Thing<S>(i);
        CHECK(GlobalMemPool<S>::instance().slabCount() > sSlabCountOrig);
    }
    for (size_t k = 0; k < N; k++)
    {
        size_t i = N - 1 - k;
        delete sMap[i];
        sMap.erase(i);
        for (const auto& sPair : sMap)
            CHECK(sPair.second->good(sPair.first));
        CHECK(GlobalMemPool<S>::instance().slabCount() > sSlabCountOrig);
    }
    CHECK(GlobalMemPool<S>::instance().slabCount() <= GlobalMemPool<S>::MAX_MMAP_SLABS + 1);
}

template <size_t S>
static void rand_test()
{
    ANNOUNCE_TMPL(S);
    size_t sSlabCountOrig = GlobalMemPool<S>::instance().slabCount();
    const size_t N = 8 * 1024;
    const size_t M = N / 16;
    std::unordered_map<size_t, Thing<S>*> sMap;
    for (size_t i = 0; i < N; i++)
    {
        sMap[i] = new Thing<S>(i);
        CHECK(GlobalMemPool<S>::instance().slabCount() > sSlabCountOrig);
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
        CHECK(GlobalMemPool<S>::instance().slabCount() > sSlabCountOrig);
    }
    CHECK(GlobalMemPool<S>::instance().slabCount() <= GlobalMemPool<S>::MAX_MMAP_SLABS + 1);
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