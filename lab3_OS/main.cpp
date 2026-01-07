#include "lab3_OS.h"
#include "tests/uTests.h"

using namespace std;

int main() {
    RunUnitTests();

    InitializeCriticalSection(&csConsole);

    cout << "Enter array size: ";
    cin >> n;
    arr = new int[n];
    for (int i = 0; i < n; i++) arr[i] = 0;

    int numThreads;
    cout << "Enter number of threads: ";
    cin >> numThreads;

    hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    hContinueEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    vector<HANDLE> threads;
    vector<ThreadParams*> paramsList;
    vector<bool> threadAlive(numThreads, true);
    int activeCount = numThreads;

    for (int i = 0; i < numThreads; i++) {
        hCanStopEvents.push_back(CreateEvent(NULL, TRUE, FALSE, NULL));
        hStopEvents.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));

        ThreadParams* p = new ThreadParams;
        p->threadID = i + 1;
        p->markedCount = 0;
        paramsList.push_back(p);

        HANDLE h = CreateThread(NULL, 0, MarkerThread, p, 0, NULL);
        threads.push_back(h);
    }

    SetEvent(hStartEvent);

    while (activeCount > 0) {
        vector<HANDLE> activeHandles;
        for (int i = 0; i < numThreads; i++) {
            if (threadAlive[i]) {
                activeHandles.push_back(hCanStopEvents[i]);
            }
        }

        if (!activeHandles.empty()) {
            WaitForMultipleObjects((DWORD)activeHandles.size(), activeHandles.data(), TRUE, INFINITE);
        }

        EnterCriticalSection(&csConsole);
        cout << "\nArray: ";
        for (int i = 0; i < n; i++) cout << arr[i] << " ";
        cout << "\n";
        LeaveCriticalSection(&csConsole);

        int killID;
        while (true) {
            EnterCriticalSection(&csConsole);
            cout << "Thread to stop (1-" << numThreads << "): ";
            LeaveCriticalSection(&csConsole);
            cin >> killID;
            if (killID >= 1 && killID <= numThreads && threadAlive[killID - 1]) break;
            cout << "Invalid ID or thread already dead.\n";
        }

        SetEvent(hStopEvents[killID - 1]);
        WaitForSingleObject(threads[killID - 1], INFINITE);
        threadAlive[killID - 1] = false;
        activeCount--;

        EnterCriticalSection(&csConsole);
        cout << "Array after death: ";
        for (int i = 0; i < n; i++) cout << arr[i] << " ";
        cout << "\n";
        LeaveCriticalSection(&csConsole);

        SetEvent(hContinueEvent);
        
        Sleep(100);
        
        ResetEvent(hContinueEvent);
    }

    cout << "All finished.\n";
    for (auto h : threads) CloseHandle(h);
    for (auto h : hCanStopEvents) CloseHandle(h);
    for (auto h : hStopEvents) CloseHandle(h);
    CloseHandle(hStartEvent);
    CloseHandle(hContinueEvent);
    for (auto p : paramsList) delete p;
    delete[] arr;
    DeleteCriticalSection(&csConsole);

    return 0;
}