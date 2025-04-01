#pragma once
#define NOMINMAX
#include <windows.h>
#include <iostream>

struct ThreadData {
    int* arr;
    int size;
    int min;
    int max;
    double average;
};

inline void find_min_max(const int* arr, int size, int& min, int& max) {
    if (size <= 0 || arr == nullptr) {
        std::cerr << "Error: Invalid array size in find_min_max()" << std::endl;
        return;
    }
    min = arr[0];
    max = arr[0];
    for (int i = 1; i < size; ++i) {
        if (arr[i] < min) min = arr[i];
        if (arr[i] > max) max = arr[i];
        Sleep(7);
    }
}
inline double calculate_average(const int* arr, int size) {
    double sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += arr[i];
        Sleep(12);
    }
    return sum / size;
}
inline void replace_min_max(int* arr, int size, int min, int max, double avg) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == min || arr[i] == max) {
            arr[i] = static_cast<int>(avg);
        }
    }
}
DWORD WINAPI min_max_thread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    find_min_max(data->arr, data->size, data->min, data->max);
    std::cout << "Min: " << data->min << "\nMax: " << data->max << std::endl;
    return 0;
}

DWORD WINAPI average_thread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    data->average = calculate_average(data->arr, data->size);
    std::cout << "Average: " << data->average << std::endl;
    return 0;
}