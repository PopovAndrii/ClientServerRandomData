#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <list>
#include <Windows.h>
#include <cstdlib>
#include <set>
#include <ctime>
#include <random>
#include <string>
CRITICAL_SECTION CriticalSection;
std::set<std::string> allRandoms;
std::string RandomString()
{
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, 128);
}

std::set<std::string> rands;

class RandomGenerator {
public:
    RandomGenerator()
    {
        m_semaphore = CreateSemaphore(0, m_maxRandoms, 1000, L"RandomGenerator");
        m_randoms.resize(m_maxRandoms + 1);
        GenerateRandoms();
    }
    ~RandomGenerator() {
        if (m_semaphore) {
            CloseHandle(m_semaphore);
        }
    }
    void GenerateRandoms() {
        for (int i = 0; i < m_maxRandoms + 1; ++i) {
            m_randoms[i] = RandomString();
        }
    }

    std::string GetRandom() {
        WaitForSingleObject(m_semaphore, INFINITE);
        std::string rnd;
        LONG curId = InterlockedIncrement(&m_curId);
        if (curId == m_maxRandoms) {
            GenerateRandoms();
            m_curId = 0;
            LONG prevCount = 0;
            ReleaseSemaphore(m_semaphore, m_maxRandoms, &prevCount);
        }
        rnd = m_randoms[curId];

        return rnd;
    }

private:
    HANDLE m_semaphore = NULL;
    std::vector<std::string> m_randoms;
    LONG volatile m_curId = NULL;
    const int m_maxRandoms = 100;
};

DWORD WINAPI ShowRandom(LPVOID param) {
    RandomGenerator* rg = (RandomGenerator*)param;
    std::string rnd = rg->GetRandom();
    auto res = allRandoms.insert(rnd);
    if (!res.second) {
        std::cout << "duplicate";
    }
    std::cout << rnd << " " << std::endl;
    return 0;
}

int main()
{
    InitializeCriticalSection(&CriticalSection);
    RandomGenerator rg;
    for (int i = 0; i < 10000; ++i) {
        CreateThread(0, 0, ShowRandom, &rg, 0, 0);

    }
    Sleep(100000);
    return 0;
}
