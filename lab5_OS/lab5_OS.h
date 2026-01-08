#pragma once
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

enum CommandType {
    CMD_READ,
    CMD_MODIFY,
    CMD_EXIT
};

struct Request {
    CommandType cmd;
    int empId;
};

struct Response {
    bool found;
    employee record;
};

const string PIPE_NAME = "\\\\.\\pipe\\Lab5_Pipe";

void RunServer();
void RunClient();

void CreateEmployeeFile(const string& filename, const vector<employee>& data);

void TestFileOperations();
