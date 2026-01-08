#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <cstring>
#include "C:\Users\acer\OneDrive\Desktop\прога\OS\lab5_OS\lab5_OS.h"

using namespace std;

void TestFileOperations() {
    string testFile = "test_data.bin";
    vector<employee> data = {
        {10, "Alice", 40.5},
        {20, "Bob", 35.0},
        {30, "Charlie", 50.0}
    };

    CreateEmployeeFile(testFile, data);
    ifstream in(testFile, ios::binary);
    assert(in.is_open() && "File failed to create");

    in.seekg(0, ios::end);
    int size = (int)in.tellg();
    assert(size == 3 * sizeof(employee) && "File size incorrect");
    in.close();
    cout << "[PASS] File Creation" << endl;

    fstream file(testFile, ios::in | ios::out | ios::binary);
    employee emp;
    bool found = false;
    int pos = 0;

    while (file.read((char*)&emp, sizeof(employee))) {
        if (emp.num == 20) {
            found = true;
            break;
        }
        pos++;
    }

    assert(found && "Record 20 should be found");
    assert(strcmp(emp.name, "Bob") == 0 && "Name should be Bob");
    cout << "[PASS] Record Search" << endl;

    file.clear();
    file.seekp(pos * sizeof(employee), ios::beg);

    emp.hours = 99.9;
    file.write((char*)&emp, sizeof(employee));
    file.close();

    ifstream check(testFile, ios::binary);
    check.seekg(pos * sizeof(employee), ios::beg);
    check.read((char*)&emp, sizeof(employee));

    assert(emp.num == 20);
    assert(emp.hours == 99.9 && "Record update failed");
    check.close();

    cout << "[PASS] Record Modification" << endl;
}