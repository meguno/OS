#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include "employee.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Usage: Reporter.exe <binary_file> <report_file> <pay_per_hour>" << endl;
        return 1;
    }

    string binfile = argv[1];
    string reportfile = argv[2];
    double pay = atof(argv[3]);

    ifstream infile(binfile, ios::binary);
    if (!infile) {
        cout << "Error opening the binary file" << endl;
        return 1;
    }

    vector<employee> employees;
    employee emp;
    while (infile.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        employees.push_back(emp);
    }
    infile.close();

    sort(employees.begin(), employees.end(), [](const employee& a, const employee& b) {
        return a.num < b.num;
        });

    ofstream outfile(reportfile);
    if (!outfile) {
        cout << "Error opening the report file" << endl;
        return 1;
    }

    string header = "File report" + binfile;
    cout << header << endl;
    outfile << header << endl;

    cout << left << setw(10) << "Num" << setw(12) << "Name" << setw(10) << "Hours" << setw(12) << "Salary" << endl;
    outfile << left << setw(10) << "Num" << setw(12) << "Name" << setw(10) << "Hours" << setw(12) << "Salary" << endl;

    for (const auto& e : employees) {
        double salary = e.hours * pay;
        cout << left << setw(10) << e.num << setw(12) << e.name << setw(10) << e.hours << setw(12) << salary << endl;
        outfile << left << setw(10) << e.num << setw(12) << e.name << setw(10) << e.hours << setw(12) << salary << endl;
    }

    outfile.close();
    cout << "The report has been created and displayed on the console" << endl;
    return 0;
}