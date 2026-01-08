#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

const int MAX_MSG_SIZE = 20;

struct Message {
    char text[MAX_MSG_SIZE];
};

struct FileHeader {
    int head;
    int tail;
    int count;
};

void RunReceiver();
void RunSender(const string& filename, int senderId);

void SetupSyncObjects(HANDLE& hMutex, HANDLE& hSemFree, HANDLE& hSemFilled, HANDLE& hSemReady, int queueSize);
void InitializeFile(const string& filename, int queueSize);

void TestFileCreation();
void TestCircularLogic();