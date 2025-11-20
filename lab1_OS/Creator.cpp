#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "employee.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: Creator.exe <binary_file> <num_records>" << endl;
        return 1;
    }

    string filename = argv[1];
    int n = atoi(argv[2]);

    ofstream file(filename, ios::binary);
    if (!file) {
        cout << "Error opening the file for writing" << endl;
        return 1;
    }

    for (int i = 0; i < n; ++i) {
        employee emp;
        cout << "Enter worker name: ";
        cin >> emp.num;
        cout << "Enter name (<=10 symbols): ";
        cin >> emp.name;
        cout << "Enter hours: ";
        cin >> emp.hours;
        file.write(reinterpret_cast<const char*>(&emp), sizeof(emp));
    }

    file.close();
    cout << "Binary file has been created" << endl;
    return 0;
}