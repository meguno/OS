#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include "C:\Users\acer\OneDrive\Desktop\прога\OS\lab4_OS\lab4_OS.h"

void TestFileCreation() {
    string testFile = "test_buffer.bin";
    int size = 5;
    InitializeFile(testFile, size);

    ifstream file(testFile, ios::binary);
    assert(file.is_open() && "File should be created");

    FileHeader header;
    file.read((char*)&header, sizeof(FileHeader));
    assert(header.head == 0 && "Head must be 0");
    assert(header.tail == 0 && "Tail must be 0");
    assert(header.count == 0 && "Count must be 0");

    file.seekg(0, ios::end);
    int fileSize = (int)file.tellg();
    int expectedSize = sizeof(FileHeader) + size * sizeof(Message);
    assert(fileSize == expectedSize && "File size is incorrect");

    file.close();
    cout << "[PASS] TestFileCreation" << endl;
}

void TestCircularLogic() {
    string testFile = "test_logic.bin";
    int size = 3;
    InitializeFile(testFile, size);

    fstream file(testFile, ios::in | ios::out | ios::binary);

    FileHeader header;
    file.read((char*)&header, sizeof(FileHeader));
    Message msg1; strcpy_s(msg1.text, "Msg1");
    file.seekp(sizeof(FileHeader) + header.tail * sizeof(Message), ios::beg);
    file.write((char*)&msg1, sizeof(Message));
    header.tail = (header.tail + 1) % size;
    header.count++;
    file.seekp(0, ios::beg);
    file.write((char*)&header, sizeof(FileHeader));

    assert(header.tail == 1);
    assert(header.count == 1);

    for (int i = 0; i < 2; i++) {
        file.seekg(0, ios::beg); file.read((char*)&header, sizeof(FileHeader));
        file.seekp(sizeof(FileHeader) + header.tail * sizeof(Message), ios::beg);
        file.write((char*)&msg1, sizeof(Message));
        header.tail = (header.tail + 1) % size;
        header.count++;
        file.seekp(0, ios::beg);
        file.write((char*)&header, sizeof(FileHeader));
    }

    assert(header.tail == 0 && "Tail should wrap around");
    assert(header.count == 3 && "Buffer should be full");

    file.seekg(0, ios::beg); file.read((char*)&header, sizeof(FileHeader));
    Message readMsg;
    file.seekg(sizeof(FileHeader) + header.head * sizeof(Message), ios::beg);
    file.read((char*)&readMsg, sizeof(Message));

    assert(strcmp(readMsg.text, "Msg1") == 0 && "Message content mismatch");

    header.head = (header.head + 1) % size;
    header.count--;

    assert(header.head == 1 && "Head should move");
    assert(header.count == 2 && "Count should decrease");

    file.close();
    cout << "[PASS] TestCircularLogic" << endl;
}