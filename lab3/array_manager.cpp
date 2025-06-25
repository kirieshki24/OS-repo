#include "array_manager.h"

ArrayManager::ArrayManager(int size)
    : array(size, 0),
    startSignal(false),
    totalMarkers(0),
    blockedMarkers(0),
    markerElements(100) { // Assuming we won't have more than 100 markers
}

int ArrayManager::getSize() const {
    return static_cast<int>(array.size());
}

int ArrayManager::getElement(int index) const {
    std::lock_guard<std::mutex> lock(arrayMutex);
    return array[index];
}

bool ArrayManager::tryMarkElement(int index, int markerId) {
    std::lock_guard<std::mutex> lock(arrayMutex);
    if (array[index] == 0) {
        array[index] = markerId;
        markerElements[markerId].push_back(index);
        return true;
    }
    return false;
}

void ArrayManager::resetMarkedElements(int markerId) {
    std::lock_guard<std::mutex> lock(arrayMutex);
    for (int index : markerElements[markerId]) {
        array[index] = 0;
    }
    markerElements[markerId].clear();
}

void ArrayManager::printArray() const {
    std::lock_guard<std::mutex> lock(arrayMutex);
    for (size_t i = 0; i < array.size(); ++i) {
        std::cout << array[i] << " ";
        if ((i + 1) % 10 == 0) { // Print 10 elements per line
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void ArrayManager::signalMarkerBlocked() {
    std::lock_guard<std::mutex> lock(syncMutex);
    blockedMarkers++;
    if (blockedMarkers == totalMarkers) {
        allBlockedCondition.notify_one();
    }
}

void ArrayManager::waitForAllMarkersBlocked() {
    std::unique_lock<std::mutex> lock(syncMutex);
    allBlockedCondition.wait(lock, [this] {
        return blockedMarkers == totalMarkers && totalMarkers > 0;
        });
}

void ArrayManager::signalAllMarkersToStart() {
    std::lock_guard<std::mutex> lock(syncMutex);
    startSignal = true;
    startCondition.notify_all();
}

void ArrayManager::signalAllMarkersToContinue() {
    std::lock_guard<std::mutex> lock(syncMutex);
    blockedMarkers = 0;
    // Уведомить все потоки о продолжении работы
    continueCondition.notify_all();
}

void ArrayManager::waitForStartSignal() {
    std::unique_lock<std::mutex> lock(syncMutex);
    totalMarkers++;
    startCondition.wait(lock, [this] { return startSignal; });
}

void ArrayManager::waitForContinueOrTerminateSignal(bool& shouldTerminate) {
    // Если флаг уже установлен, сразу возвращаемся
    if (shouldTerminate) {
        return;
    }

    // Ждем сигнала продолжения работы
    std::unique_lock<std::mutex> lock(syncMutex);
    continueCondition.wait(lock);
}

void ArrayManager::incrementBlockedCount() {
    std::lock_guard<std::mutex> lock(syncMutex);
    blockedMarkers++;
    if (blockedMarkers == totalMarkers) {
        allBlockedCondition.notify_one();
    }
}

void ArrayManager::decrementBlockedCount() {
    std::lock_guard<std::mutex> lock(syncMutex);
    totalMarkers--;
}

int ArrayManager::getBlockedCount() const {
    std::lock_guard<std::mutex> lock(syncMutex);
    return blockedMarkers;
}