#include "lab4_OS.h"

const string MUTEX_NAME = "Lab4_FileMutex";
const string SEM_FREE_NAME = "Lab4_SemFree";
const string SEM_FILLED_NAME = "Lab4_SemFilled";
const string SEM_READY_NAME = "Lab4_SemReady";


void SetupSyncObjects(HANDLE& hMutex, HANDLE& hSemFree, HANDLE& hSemFilled, HANDLE& hSemReady, int queueSize) {
    hMutex = CreateMutexA(NULL, FALSE, MUTEX_NAME.c_str());
    hSemFree = CreateSemaphoreA(NULL, queueSize, queueSize, SEM_FREE_NAME.c_str());
    hSemFilled = CreateSemaphoreA(NULL, 0, queueSize, SEM_FILLED_NAME.c_str());
    hSemReady = CreateSemaphoreA(NULL, 0, 100, SEM_READY_NAME.c_str());
}

void InitializeFile(const std::string& filename, int queueSize) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        cerr << "Error creating file!" << endl;
        return;
    }

    FileHeader header = { 0, 0, 0 };
    outFile.write((char*)&header, sizeof(FileHeader));

    Message emptyMsg = { "" };
    for (int i = 0; i < queueSize; ++i) {
        outFile.write((char*)&emptyMsg, sizeof(Message));
    }
    outFile.close();
}


void RunReceiver() {
    string filename;
    int queueSize, senderCount;

    cout << "[Receiver] Enter binary filename: ";
    cin >> filename;
    cout << "[Receiver] Enter number of records (queue size): ";
    cin >> queueSize;

    InitializeFile(filename, queueSize);

    HANDLE hMutex, hSemFree, hSemFilled, hSemReady;
    SetupSyncObjects(hMutex, hSemFree, hSemFilled, hSemReady, queueSize);

    cout << "[Receiver] Enter number of Sender processes: ";
    cin >> senderCount;

    vector<PROCESS_INFORMATION> pi(senderCount);
    vector<STARTUPINFOA> si(senderCount);

    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    string currentExe = buffer;

    for (int i = 0; i < senderCount; ++i) {
        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);
        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));

        string cmdArgs = "\"" + currentExe + "\" sender " + filename + " " + to_string(i + 1);

        if (!CreateProcessA(NULL, (LPSTR)cmdArgs.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) {
            cerr << "Failed to create Sender process!" << endl;
        }
    }

    cout << "[Receiver] Waiting for all Senders to be ready..." << endl;
    for (int i = 0; i < senderCount; ++i) {
        WaitForSingleObject(hSemReady, INFINITE);
    }
    cout << "[Receiver] All Senders are ready." << endl;

    while (true) {
        cout << "\n[Receiver] Choose action: 1. Read message, 2. Exit\n> ";
        int choice;
        cin >> choice;

        if (choice == 2) break;

        if (choice == 1) {
            cout << "[Receiver] Waiting for message..." << endl;
            WaitForSingleObject(hSemFilled, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);

            fstream file(filename, ios::in | ios::out | ios::binary);
            FileHeader header;
            file.read((char*)&header, sizeof(FileHeader));

            Message msg;
            file.seekg(sizeof(FileHeader) + header.head * sizeof(Message), ios::beg);
            file.read((char*)&msg, sizeof(Message));

            cout << "[Receiver] Read message: " << msg.text << endl;

            header.head = (header.head + 1) % queueSize;
            header.count--;

            file.seekp(0, ios::beg);
            file.write((char*)&header, sizeof(FileHeader));
            file.close();

            ReleaseMutex(hMutex);
            ReleaseSemaphore(hSemFree, 1, NULL);
        }
    }

    for (int i = 0; i < senderCount; ++i) {
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }
    CloseHandle(hMutex);
    CloseHandle(hSemFree);
    CloseHandle(hSemFilled);
    CloseHandle(hSemReady);
}

void RunSender(const std::string& filename, int senderId) {
    HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME.c_str());
    HANDLE hSemFree = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, SEM_FREE_NAME.c_str());
    HANDLE hSemFilled = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, SEM_FILLED_NAME.c_str());
    HANDLE hSemReady = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, SEM_READY_NAME.c_str());

    if (!hMutex || !hSemFree || !hSemFilled || !hSemReady) {
        cerr << "[Sender " << senderId << "] Error opening sync objects." << endl;
        system("pause");
        return;
    }

    cout << "[Sender " << senderId << "] Ready to work." << endl;
    ReleaseSemaphore(hSemReady, 1, NULL);

    while (true) {
        cout << "\n[Sender " << senderId << "] Choose action: 1. Send message, 2. Exit\n> ";
        int choice;
        cin >> choice;

        if (choice == 2) break;

        if (choice == 1) {
            char textBuffer[MAX_MSG_SIZE];
            cout << "Enter message (max 20 chars): ";
            cin >> textBuffer;

            cout << "[Sender " << senderId << "] Waiting for free slot..." << endl;
            WaitForSingleObject(hSemFree, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);

            fstream file(filename, ios::in | ios::out | ios::binary);

            FileHeader header;
            file.read((char*)&header, sizeof(FileHeader));

            file.seekg(0, ios::end);
            int fileSize = (int)file.tellg();
            int capacity = (fileSize - sizeof(FileHeader)) / sizeof(Message);

            Message msg;
            strncpy_s(msg.text, textBuffer, MAX_MSG_SIZE - 1);

            file.seekp(sizeof(FileHeader) + header.tail * sizeof(Message), ios::beg);
            file.write((char*)&msg, sizeof(Message));

            header.tail = (header.tail + 1) % capacity;
            header.count++;

            file.seekp(0, ios::beg);
            file.write((char*)&header, sizeof(FileHeader));
            file.close();

            ReleaseMutex(hMutex);
            ReleaseSemaphore(hSemFilled, 1, NULL);

            cout << "[Sender " << senderId << "] Message sent." << endl;
        }
    }
}