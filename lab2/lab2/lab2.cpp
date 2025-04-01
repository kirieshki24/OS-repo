#include "lab2.h"


int main() {
    int size;
    std::cout << "Enter array size: ";
    std::cin >> size;

    if (size <= 0) {
        std::cerr << "Invalid array size!" << std::endl;
        return 1;
    }

    int* arr = new int[size];
    std::cout << "Enter array elements: ";
    for (int i = 0; i < size; ++i) {
        std::cin >> arr[i];
    }

    ThreadData data{ arr, size, 0, 0, 0.0 };

    HANDLE hMinMax = CreateThread(nullptr, 0, min_max_thread, &data, 0, nullptr);
    HANDLE hAverage = CreateThread(nullptr, 0, average_thread, &data, 0, nullptr);

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    replace_min_max(arr, size, data.min, data.max, data.average);

    std::cout << "Modified array: ";
    for (int i = 0; i < size; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    delete[] arr;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return 0;
}