#include "marker_thread.h"
#include <iostream>
#include <thread>
#include <random>

MarkerThread::MarkerThread(int id, ArrayManager& arrayManager)
    : id(id),
    arrayManager(arrayManager),
    shouldTerminate(false),
    markedCount(0) {
}

void MarkerThread::run() {
    // 1. Wait for start signal from main thread
    arrayManager.waitForStartSignal();

    // 2. Initialize random number generation
    srand(id);

    // 3. Main work loop
    while (true) {
        // Проверяем флаг завершения
        {
            std::lock_guard<std::mutex> lock(terminateMutex);
            if (shouldTerminate) {
                break;
            }
        }

        // 3.1 Generate random number
        int randomIndex = rand() % arrayManager.getSize();

        // 3.3 Try to mark the element
        if (arrayManager.tryMarkElement(randomIndex, id)) {
            // 3.3.1 Sleep for 5 ms
            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            // Element already marked in tryMarkElement
            markedCount++;

            // 3.3.3 Sleep for 5 ms
            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            // 3.3.4 Continue loop
        }
        else {
            // 3.4.1 Print information
            std::cout << "Marker " << id << " blocked:" << std::endl;
            std::cout << " - Marker ID: " << id << std::endl;
            std::cout << " - Marked elements: " << markedCount << std::endl;
            std::cout << " - Cannot mark element at index: " << randomIndex << std::endl;

            // 3.4.2 Signal main thread
            arrayManager.incrementBlockedCount();

            // 3.4.3 Wait for signal to continue or terminate
            // Создаем локальную копию флага для передачи в метод
            bool localShouldTerminate = false;
            {
                std::lock_guard<std::mutex> lock(terminateMutex);
                localShouldTerminate = shouldTerminate;
            }

            if (!localShouldTerminate) {
                arrayManager.waitForContinueOrTerminateSignal(localShouldTerminate);
            }

            // После возвращения из ожидания проверяем глобальный флаг завершения
            {
                std::lock_guard<std::mutex> lock(terminateMutex);
                if (shouldTerminate) {
                    break;
                }
            }
        }
    }

    // 4.1 Reset all elements marked by this thread
    arrayManager.resetMarkedElements(id);

    // 4.2 Decrement blocked count and terminate
    arrayManager.decrementBlockedCount();
    std::cout << "Marker " << id << " terminated." << std::endl;
}

void MarkerThread::signalToTerminate() {
    std::lock_guard<std::mutex> lock(terminateMutex);
    shouldTerminate = true;
}

int MarkerThread::getId() const {
    return id;
}

int MarkerThread::getMarkedCount() const {
    return markedCount;
}