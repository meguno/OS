#pragma once
#include <windows.h>
#include <vector>
#include <iostream>

extern int* arr;
extern int n;
extern CRITICAL_SECTION csConsole;

extern HANDLE hStartEvent;
extern HANDLE hContinueEvent;
extern std::vector<HANDLE> hCanStopEvents;
extern std::vector<HANDLE> hStopEvents;

struct ThreadParams {
    int threadID;
    int markedCount;
};

DWORD WINAPI MarkerThread(LPVOID lpParam);

void RunUnitTests();