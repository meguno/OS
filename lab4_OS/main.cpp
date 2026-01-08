#include "lab4_OS.h"
#include <iostream>

int main(int argc, char* argv[]) {
    
    TestFileCreation();
    TestCircularLogic();

    if (argc == 1) {
        RunReceiver();
    }
    else if (argc == 4 && std::string(argv[1]) == "sender") {
        string filename = argv[2];
        int id = std::stoi(argv[3]);
        RunSender(filename, id);
    }
    else {
        cout << "Unknown arguments." << endl;
    }

    return 0;
}