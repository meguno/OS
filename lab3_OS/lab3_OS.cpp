#include "lab3_OS.h"
#include <algorithm>

using namespace std;

int* arr = nullptr;
int n = 0;
CRITICAL_SECTION csConsole;

HANDLE hStartEvent = NULL;
HANDLE hContinueEvent = NULL;
vector<HANDLE> hCanStopEvents;
vector<HANDLE> hStopEvents;


DWORD WINAPI MarkerThread(LPVOID lpParam) {
    ThreadParams* params = (ThreadParams*)lpParam;
    int id = params->threadID;

    WaitForSingleObject(hStartEvent, INFINITE);

    srand(id);

    while (true) {
        int randomNum = rand();
        int index = randomNum % n;

        if (arr[index] == 0) {
            Sleep(5);
            arr[index] = id;
            params->markedCount++;
            Sleep(5);
            continue;
        }
        else {
            EnterCriticalSection(&csConsole);
            cout << "Thread " << id << ": Cannot mark index " << index
                << ". Held by " << arr[index]
                << ". Marked count: " << params->markedCount << "\n";
            LeaveCriticalSection(&csConsole);

            SetEvent(hCanStopEvents[id - 1]);

            HANDLE eventsToWaitFor[] = { hStopEvents[id - 1], hContinueEvent };
            DWORD waitResult = WaitForMultipleObjects(2, eventsToWaitFor, FALSE, INFINITE);

            if (waitResult == WAIT_OBJECT_0) {
                for (int i = 0; i < n; i++) {
                    if (arr[i] == id) arr[i] = 0;
                }
                return 0;
            }
            else {
                ResetEvent(hCanStopEvents[id - 1]);
                continue;
            }
        }
    }
    return 0;
}