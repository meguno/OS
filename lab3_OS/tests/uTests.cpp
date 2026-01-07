#include "uTests.h"
#include <iostream>

using namespace std;

bool TestArrayLogic(int* testArr, int size, int index, int id) {
    if (index < 0 || index >= size) return false;
    if (testArr[index] == 0) {
        testArr[index] = id;
        return true;
    }
    return false;
}

void RunUnitTests() {
    cout << "Running Unit Tests...\n";

    int testSize = 5;
    int* testArr = new int[testSize];
    for (int i = 0; i < testSize; i++) testArr[i] = 0;

    if (TestArrayLogic(testArr, testSize, 2, 1) == true && testArr[2] == 1) {
        cout << "[PASS] Marking empty cell\n";
    }
    else {
        cout << "[FAIL] Marking empty cell\n";
    }

    if (TestArrayLogic(testArr, testSize, 2, 2) == false) {
        cout << "[PASS] Collision detection\n";
    }
    else {
        cout << "[FAIL] Collision detection\n";
    }

    delete[] testArr;
    cout << "Unit Tests Finished.\n\n";
}