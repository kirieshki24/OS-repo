#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "marker_thread.h"
#include "array_manager.h"

int main() {
    // 1. Get array size from console
    int arraySize;
    std::cout << "Enter array size: ";
    std::cin >> arraySize;

    if (arraySize <= 0) {
        std::cerr << "Array size must be positive." << std::endl;
        return 1;
    }

    // Create array manager
    ArrayManager arrayManager(arraySize);

    // 3. Get number of marker threads
    int numThreads;
    std::cout << "Enter number of marker threads: ";
    std::cin >> numThreads;

    if (numThreads <= 0) {
        std::cerr << "Number of threads must be positive." << std::endl;
        return 1;
    }

    // 4. Create marker threads
    std::vector<std::shared_ptr<MarkerThread>> markerThreads;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        auto marker = std::make_shared<MarkerThread>(i + 1, arrayManager);
        markerThreads.push_back(marker);
        threads.emplace_back(&MarkerThread::run, marker);
    }

    // 5. Signal to start all marker threads
    arrayManager.signalAllMarkersToStart();

    // 6. Main loop
    while (!markerThreads.empty()) {
        // 6.1. Wait until all markers signal they can't continue
        arrayManager.waitForAllMarkersBlocked();

        // 6.2. Print array contents
        std::cout << "Current array state:" << std::endl;
        arrayManager.printArray();

        // 6.3. Get marker thread number to terminate
        int markerToTerminate;
        std::cout << "Enter marker thread number to terminate (1-" << markerThreads.size() << "): ";
        std::cin >> markerToTerminate;

        // Validate input
        if (markerToTerminate < 1 || markerToTerminate > static_cast<int>(markerThreads.size())) {
            std::cout << "Invalid thread number. Please try again." << std::endl;
            continue;
        }

        // Find the actual index in our vector
        int actualIndex = -1;
        for (size_t i = 0; i < markerThreads.size(); ++i) {
            if (markerThreads[i]->getId() == markerToTerminate) {
                actualIndex = static_cast<int>(i);
                break;
            }
        }

        if (actualIndex == -1) {
            std::cout << "Thread not found. Please try again." << std::endl;
            continue;
        }

        // 6.4. Signal the selected marker thread to terminate
        std::cout << "Signaling thread " << markerToTerminate << " to terminate..." << std::endl;
        markerThreads[actualIndex]->signalToTerminate();

        // Сигнализируем всем потокам продолжить работу, чтобы завершаемый поток мог проверить свой флаг
        std::cout << "Signaling all threads to continue..." << std::endl;
        arrayManager.signalAllMarkersToContinue();

        // 6.5. Wait for the thread to finish
        std::cout << "Waiting for thread " << markerToTerminate << " to terminate..." << std::endl;
        threads[actualIndex].join();
        std::cout << "Thread " << markerToTerminate << " has terminated." << std::endl;

        // 6.6. Print array contents again
        std::cout << "Array after thread " << markerToTerminate << " terminated:" << std::endl;
        arrayManager.printArray();

        // Remove the terminated thread from our vectors
        threads.erase(threads.begin() + actualIndex);
        markerThreads.erase(markerThreads.begin() + actualIndex);

        // 6.7. Signal remaining threads to continue
        arrayManager.signalAllMarkersToContinue();
    }

    // 7. Finish after all marker threads have completed
    std::cout << "All marker threads have finished. Program terminating." << std::endl;

    return 0;
}