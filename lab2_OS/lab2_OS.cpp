#include <Windows.h>
#include "lab2_OS.h"

using namespace std;

int* arr = nullptr;
int size_n = 0;
int min_val = 0;
int max_val = 0;
int average_val = 0;

DWORD WINAPI MinMax(LPVOID lpParam) {
	min_val = arr[0];
	max_val = arr[0];

	for (int i = 0; i < size_n; i++) {
		if (arr[i] < min_val) {
			min_val = arr[i];
		}
		Sleep(7);

		if (arr[i] > max_val) {
			max_val = arr[i];
		}
		Sleep(7);
	}

	cout << "MIN VALUE: " << min_val << '\n' << "MAX VALUE: " << max_val << '\n';

	return 0;
}

DWORD WINAPI Average(LPVOID lpParam) {
	int sum = 0;

	for (int i = 0; i < size_n; i++) {
		sum += arr[i];

		Sleep(12);
	}

	average_val = sum / size_n;
	cout << "Aerage arr value: " << average_val << '\n';

	return 0;
}

void runProgram() {
	cout << "Enter array size: ";
	cin >> size_n;

	arr = new int[size_n];
	cout << "Enter the nums of massive: ";
	for (int i = 0; i < size_n; i++) {
		cin >> arr[i];
	}

	HANDLE hMinMax;
	HANDLE hAverage;

	hMinMax = CreateThread(NULL, 0, MinMax, NULL, 0, NULL);
	hAverage = CreateThread(NULL, 0, Average, NULL, 0, NULL);

	if (hMinMax == NULL || hAverage == NULL) {
		cout << "Error!\n";
		return;
	}

	WaitForSingleObject(hMinMax, INFINITE);
	WaitForSingleObject(hAverage, INFINITE);

	CloseHandle(hMinMax);
	CloseHandle(hAverage);

	cout << "All threads are completed!\n";

	for (int i = 0; i < size_n; i++) {
		if (arr[i] == min_val || arr[i] == max_val) {
			arr[i] = average_val;
		}
	}

	cout << "Final massive: ";
	for (int i = 0; i < size_n; i++) {
		cout << arr[i] << " ";
	}
	cout << '\n';

	delete[] arr;
}