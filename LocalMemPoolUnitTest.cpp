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
    LocalMemPool<1>::instance().alloc();
    LocalMemPool<2>::instance().alloc();
    LocalMemPool<3>::instance().alloc();
    LocalMemPool<4>::instance().alloc();
    LocalMemPool<5>::instance().alloc();
    LocalMemPool<6>::instance().alloc();
    LocalMemPool<7>::instance().alloc();
    LocalMemPool<8>::instance().alloc();
    LocalMemPool<9>::instance().alloc();
    LocalMemPool<10>::instance().alloc();
    LocalMemPool<11>::instance().alloc();
    LocalMemPool<12>::instance().alloc();
    LocalMemPool<13>::instance().alloc();
    LocalMemPool<14>::instance().alloc();
    LocalMemPool<15>::instance().alloc();
    LocalMemPool<16>::instance().alloc();
    LocalMemPool<17>::instance().alloc();
    LocalMemPool<18>::instance().alloc();
    LocalMemPool<19>::instance().alloc();
    LocalMemPool<20>::instance().alloc();
    LocalMemPool<21>::instance().alloc();
    LocalMemPool<22>::instance().alloc();
    LocalMemPool<23>::instance().alloc();
    LocalMemPool<24>::instance().alloc();
    LocalMemPool<25>::instance().alloc();
    LocalMemPool<26>::instance().alloc();
    LocalMemPool<27>::instance().alloc();
    LocalMemPool<28>::instance().alloc();
    LocalMemPool<29>::instance().alloc();
    LocalMemPool<30>::instance().alloc();
    LocalMemPool<31>::instance().alloc();
    LocalMemPool<32>::instance().alloc();
    LocalMemPool<33>::instance().alloc();
    LocalMemPool<34>::instance().alloc();
    LocalMemPool<35>::instance().alloc();
    LocalMemPool<36>::instance().alloc();
    LocalMemPool<37>::instance().alloc();
    LocalMemPool<38>::instance().alloc();
    LocalMemPool<39>::instance().alloc();
    LocalMemPool<40>::instance().alloc();
    LocalMemPool<41>::instance().alloc();
    LocalMemPool<42>::instance().alloc();
    LocalMemPool<43>::instance().alloc();
    LocalMemPool<44>::instance().alloc();
    LocalMemPool<45>::instance().alloc();
    LocalMemPool<46>::instance().alloc();
    LocalMemPool<47>::instance().alloc();
    LocalMemPool<48>::instance().alloc();
    LocalMemPool<49>::instance().alloc();
    LocalMemPool<50>::instance().alloc();
    LocalMemPool<51>::instance().alloc();
    LocalMemPool<52>::instance().alloc();
    LocalMemPool<53>::instance().alloc();
    LocalMemPool<54>::instance().alloc();
    LocalMemPool<55>::instance().alloc();
    LocalMemPool<56>::instance().alloc();
    LocalMemPool<57>::instance().alloc();
    LocalMemPool<58>::instance().alloc();
    LocalMemPool<59>::instance().alloc();
    LocalMemPool<60>::instance().alloc();
    LocalMemPool<61>::instance().alloc();
    LocalMemPool<62>::instance().alloc();
    LocalMemPool<63>::instance().alloc();
    LocalMemPool<64>::instance().alloc();
    LocalMemPool<65>::instance().alloc();
    LocalMemPool<66>::instance().alloc();
    LocalMemPool<67>::instance().alloc();
    LocalMemPool<68>::instance().alloc();
    LocalMemPool<69>::instance().alloc();
    LocalMemPool<70>::instance().alloc();
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
        return LocalMemPool<sizeof(Thing)>::instance().alloc();
    }
    void operator delete(void* aPtr)
    {
        LocalMemPool<sizeof(Thing)>::instance().free(static_cast<char*>(aPtr));
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