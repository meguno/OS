#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <vector>
#include "employee.h"

using namespace std;

int main() {
    string binfile;
    int n;
    cout << "Enter the binary file name: ";
    cin >> binfile;
    cout << "Enter the name of records: ";
    cin >> n;

    string cmd = "Creator.exe " + binfile + " " + to_string(n);
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, const_cast<char*>(cmd.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cout << "Creator launch error" << endl;
        return 1;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    ifstream infile(binfile, ios::binary);
    if (!infile) {
        cout << "Binary file opening error" << endl;
        return 1;
    }
    vector<employee> employees;
    employee emp;
    while (infile.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        employees.push_back(emp);
    }
    infile.close();

    cout << "Binary file content:" << endl;
    for (const auto& e : employees) {
        cout << "Num: " << e.num << ", Name: " << e.name << ", Hours: " << e.hours << endl;
    }

    string reportfile;
    double pay;
    cout << "Enter the name of report file: ";
    cin >> reportfile;
    cout << "Enter the payment per hour: ";
    cin >> pay;

    string cmd2 = "Reporter.exe " + binfile + " " + reportfile + " " + to_string(pay);
    STARTUPINFO si2 = { sizeof(si2) };
    PROCESS_INFORMATION pi2;
    if (!CreateProcess(NULL, const_cast<char*>(cmd2.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2)) {
        cout << "Reporter launch error" << endl;
        return 1;
    }
    WaitForSingleObject(pi2.hProcess, INFINITE);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    cout << "The program is completed" << endl;
    return 0;
}