#include "lab5_OS.h"

void CreateEmployeeFile(const string& filename, const vector<employee>& data) {
    ofstream out(filename, ios::binary);
    for (const auto& emp : data) {
        out.write((char*)&emp, sizeof(employee));
    }
    out.close();
}

void PrintFileContent(const string& filename) {
    ifstream in(filename, ios::binary);
    employee emp;
    cout << "\n--- File Content (" << filename << ") ---\n";
    cout << "ID\tName\tHours\n";
    while (in.read((char*)&emp, sizeof(employee))) {
        cout << emp.num << "\t" << emp.name << "\t" << emp.hours << "\n";
    }
    cout << "------------------------------\n";
    in.close();
}

void RunServer() {
    string filename;
    int count;

    cout << "[Server] Enter filename: ";
    cin >> filename;
    cout << "[Server] Enter number of employees: ";
    cin >> count;

    vector<employee> employees(count);
    for (int i = 0; i < count; ++i) {
        cout << "Employee " << i + 1 << "\nID: ";
        cin >> employees[i].num;
        cout << "Name (max 9 chars): ";
        cin >> employees[i].name;
        cout << "Hours: ";
        cin >> employees[i].hours;
    }

    CreateEmployeeFile(filename, employees);
    PrintFileContent(filename);

    cout << "[Server] Launching Client process...\n";

    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    string currentExe = buffer;
    string cmdArgs = "\"" + currentExe + "\" client";

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(NULL, (LPSTR)cmdArgs.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        cerr << "[Server] Failed to launch client.\n";
        return;
    }

    HANDLE hPipe = CreateNamedPipeA(
        PIPE_NAME.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        1024, 1024, 0, NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "[Server] Failed to create pipe.\n";
        return;
    }

    cout << "[Server] Waiting for client to connect...\n";
    if (ConnectNamedPipe(hPipe, NULL)) {
        cout << "[Server] Client connected.\n";

        while (true) {
            Request req;
            DWORD bytesRead;
            if (!ReadFile(hPipe, &req, sizeof(Request), &bytesRead, NULL) || bytesRead == 0) {
                break;
            }

            if (req.cmd == CMD_EXIT) {
                cout << "[Server] Client requested exit.\n";
                break;
            }

            fstream file(filename, ios::in | ios::out | ios::binary);
            employee emp;
            bool found = false;
            int pos = 0;

            while (file.read((char*)&emp, sizeof(employee))) {
                if (emp.num == req.empId) {
                    found = true;
                    break;
                }
                pos++;
            }

            Response resp;
            resp.found = found;
            resp.record = emp;

            DWORD bytesWritten;
            WriteFile(hPipe, &resp, sizeof(Response), &bytesWritten, NULL);

            if (req.cmd == CMD_MODIFY && found) {
                cout << "[Server] Record " << req.empId << " locked for modification.\n";

                if (ReadFile(hPipe, &emp, sizeof(employee), &bytesRead, NULL)) {
                    file.clear();
                    file.seekp(pos * sizeof(employee), ios::beg);
                    file.write((char*)&emp, sizeof(employee));
                    file.flush();
                    cout << "[Server] Record updated.\n";
                }
            }
            else if (req.cmd == CMD_READ && found) {
                cout << "[Server] Record " << req.empId << " requested for reading.\n";
                char buffer;
                ReadFile(hPipe, &buffer, 1, &bytesRead, NULL);
            }
            file.close();
        }
    }

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    PrintFileContent(filename);

    cout << "[Server] Press Enter to finish...";
    cin.ignore(); cin.get();
}

void RunClient() {
    cout << "[Client] Connecting to server...\n";
    WaitNamedPipeA(PIPE_NAME.c_str(), NMPWAIT_WAIT_FOREVER);

    HANDLE hPipe = CreateFileA(
        PIPE_NAME.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "[Client] Failed to connect to pipe.\n";
        system("pause");
        return;
    }

    while (true) {
        cout << "\n[Client] Choose action: 1. Modify, 2. Read, 3. Exit\n> ";
        int choice;
        cin >> choice;

        if (choice == 3) {
            Request req = { CMD_EXIT, 0 };
            DWORD bytesWritten;
            WriteFile(hPipe, &req, sizeof(Request), &bytesWritten, NULL);
            break;
        }

        int id;
        cout << "Enter Employee ID: ";
        cin >> id;

        Request req;
        req.cmd = (choice == 1) ? CMD_MODIFY : CMD_READ;
        req.empId = id;

        DWORD bytesWritten, bytesRead;
        if (!WriteFile(hPipe, &req, sizeof(Request), &bytesWritten, NULL)) {
            cerr << "[Client] Server disconnected.\n";
            break;
        }

        Response resp;
        if (!ReadFile(hPipe, &resp, sizeof(Response), &bytesRead, NULL)) break;

        if (!resp.found) {
            cout << "[Client] Employee with ID " << id << " not found.\n";
            continue;
        }

        cout << "Current Data: ID=" << resp.record.num
            << ", Name=" << resp.record.name
            << ", Hours=" << resp.record.hours << "\n";

        if (choice == 1) {
            cout << "Enter NEW Name: ";
            cin >> resp.record.name;
            cout << "Enter NEW Hours: ";
            cin >> resp.record.hours;

            WriteFile(hPipe, &resp.record, sizeof(employee), &bytesWritten, NULL);
            cout << "[Client] Modification sent.\n";
        }

        cout << "[Client] Press any key to finish access to this record...";
        cin.ignore(); cin.get();

        if (choice == 2) {
            char signal = 1;
            WriteFile(hPipe, &signal, 1, &bytesWritten, NULL);
        }
    }

    CloseHandle(hPipe);
}