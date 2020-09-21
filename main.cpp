#include <iostream>
#include <windows.h>
#include <vector>
#include <string> 

HANDLE clientWaitSemaphore = INVALID_HANDLE_VALUE;
HANDLE clientExit = INVALID_HANDLE_VALUE;
HANDLE dataReady = INVALID_HANDLE_VALUE;
HANDLE server = INVALID_HANDLE_VALUE;

CRITICAL_SECTION c;
LONG counter = 1;

std::vector<int> data;

DWORD WINAPI Client(LPVOID arg) {
    int id = reinterpret_cast<int>(arg);
    WaitForSingleObject(clientWaitSemaphore, INFINITE);
    WaitForSingleObject(dataReady, INFINITE);

    std::string str = std::to_string(id) + " Client recived hash: " + std::to_string(data[0]) + "\n";
    std::cout << str;

    SetEvent(clientExit);
    return 0;
}

DWORD WINAPI Server(LPVOID arg) {
    int semaphore = reinterpret_cast<int>(arg);
    LONG prevVal = 0;
    data.push_back(0);

    for (;;) {
        Sleep(300);
        std::cout << "---- Server load ----\n";
        ReleaseSemaphore(clientWaitSemaphore, semaphore, &prevVal);
    
        for (int j = 0; j < semaphore; ++j) {
            EnterCriticalSection(&c);
            data[0] = rand() % 999999000000 + (++counter);
            LeaveCriticalSection(&c);
            SetEvent(dataReady);
        }
            WaitForSingleObject(clientExit, INFINITE);
    }
    return 0;
}

int main()
{
    dataReady = CreateEventA(0, FALSE, FALSE, NULL);
    if (dataReady == INVALID_HANDLE_VALUE) return -1;

    clientExit = CreateEventA(0, FALSE, FALSE, NULL);
    if (clientExit == INVALID_HANDLE_VALUE) return -1;

    clientWaitSemaphore = CreateSemaphore(0, 0, 10, L"ServerDataSync");
    if (clientWaitSemaphore == INVALID_HANDLE_VALUE) return -1;

    for (int i = 0; i < 200; ++i) {
        HANDLE client = CreateThread(0, 0, Client, reinterpret_cast<LPVOID>(i), 0, 0);
        if (client == INVALID_HANDLE_VALUE) return -1;
    }

    InitializeCriticalSection(&c);
    server = CreateThread(0, 0, Server, reinterpret_cast<LPVOID>(5), 0, 0);
    if (server == INVALID_HANDLE_VALUE) return -1;

    Sleep(7000);
    //WaitForMultipleObjects();
    DeleteCriticalSection(&c);
    return 0;
}
