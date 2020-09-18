#include <iostream>
#include <windows.h>
#include <vector>

HANDLE h = INVALID_HANDLE_VALUE;
HANDLE ping = INVALID_HANDLE_VALUE;

std::vector<int> data;

DWORD WINAPI Client(LPVOID arg) {
    int id = (int)arg;
    
    DWORD res = WaitForSingleObject(h, INFINITE);
    std::cout << id << " Client recived data " << data[0] << "\n";

    HANDLE e = OpenEventA(EVENT_MODIFY_STATE, true, "Server");
    if (e == INVALID_HANDLE_VALUE) return -1;
    SetEvent(e);

    return 0;
}

DWORD WINAPI Server(LPVOID arg) {
    LONG prevVal = 0;

    data.push_back(0);

    for (int i = 0; i < 6; ++i) {
        Sleep(1000);
        std::cout << "Server second data \n";
        ReleaseSemaphore(h, 2, &prevVal);
    
        //for (int j = 0; j < 2; ++j) {

            DWORD res = WaitForSingleObject(ping, INFINITE);
            data[0] = rand() % 10;
            DWORD res1 = WaitForSingleObject(ping, INFINITE);
            data[0] = rand() % 10 + 1;

        //}
        std::cout << "UnSleepServer\n";
    }

    return 0;
}


int main()
{

    ping = CreateEventA(0, FALSE, FALSE, "Server");
    if (ping == INVALID_HANDLE_VALUE) return -1;

    h = CreateSemaphore(0, 0, 10, L"ServerDataSync");
    if (h == INVALID_HANDLE_VALUE) return -1;

    CreateThread(0, 0, Server, 0, 0, 0);
    Sleep(2000);
    for (int i = 0; i < 6; ++i) {
        CreateThread(0, 0, Client, (LPVOID)i, 0, 0);
    }

    Sleep(50000);
}
