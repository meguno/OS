#include "lab5_OS.h"

int main(int argc, char* argv[]) {
 
    TestFileOperations();
    
    if (argc > 1 && string(argv[1]) == "client") {
        RunClient();
    }
    else {
        RunServer();
    }
    return 0;
}