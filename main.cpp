#include <iostream>
#include <windows.h>
#include <vector>

HANDLE h = INVALID_HANDLE_VALUE;
CRITICAL_SECTION   section;

std::vector<int> data;

DWORD WINAPI Client(LPVOID arg) {
    int id = (int)arg;
    
    //EnterCriticalSection(&section);
    DWORD res = WaitForSingleObject(h, INFINITE);
    std::cout << id << " Client recived data " << data[0] << "\n";
    //LeaveCriticalSection(&section);
    return 0;
}

DWORD WINAPI Server(LPVOID arg) {
    LONG prevVal = 0;
    
    EnterCriticalSection(&section);
    data.push_back(10501);

    for (int i = 0; i < 20; ++i) {
        Sleep(3000);
        std::cout << "Server second data \n";
      ReleaseSemaphore(h, 1, &prevVal);
    
        data[0] = 10501 + i;
    }
    LeaveCriticalSection(&section);

    return 0;
}


int main()
{

    h = CreateSemaphore(0, 0, 10, L"ServerDataSync");
    
    InitializeCriticalSection(&section);
    CreateThread(0, 0, Server, 0, 0, 0);
    

    for (int i = 0; i < 20; ++i) {
        CreateThread(0, 0, Client, (LPVOID)i, 0, 0);
    }
    DeleteCriticalSection(&section);

    Sleep(50000);
}
