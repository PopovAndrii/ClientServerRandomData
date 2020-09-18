#include <iostream>
#include <windows.h>
#include <vector>

HANDLE h = INVALID_HANDLE_VALUE;
HANDLE e = INVALID_HANDLE_VALUE;

std::vector<int> data;

DWORD WINAPI Client(LPVOID arg) {
    int id = (int)arg;

    DWORD res = WaitForSingleObject(h, INFINITE);
    std::cout << id << " Client recived data " << data[0] << "\n";
    
    e = OpenEventA(EVENT_MODIFY_STATE, true, "Server");
    if (e == INVALID_HANDLE_VALUE) return -1;
    SetEvent(e);

    return 0;
}

DWORD WINAPI Server(LPVOID arg) {
    LONG prevVal = 0;
    
    data.push_back(0);

    for (int i = 0; i < 20; ++i) {
        Sleep(1000);
        std::cout << "Server second data \n";
        ReleaseSemaphore(h, 1, &prevVal);
    
        data[0] = 10501 + i;

        DWORD res = WaitForSingleObject(e, INFINITE);
    }

    return 0;
}


int main()
{

    h = CreateSemaphore(0, 0, 10, L"ServerDataSync");

    CreateThread(0, 0, Server, 0, 0, 0);
    

    for (int i = 0; i < 20; ++i) {
        CreateThread(0, 0, Client, (LPVOID)i, 0, 0);
    }

    Sleep(50000);
}
