#ifndef ARRAY_MANAGER_H
#define ARRAY_MANAGER_H

#include <vector>
#include <mutex>
#include <condition_variable>
#include <iostream>

class ArrayManager {
public:
    explicit ArrayManager(int size);

    // Array operations
    int getSize() const;
    int getElement(int index) const;
    bool tryMarkElement(int index, int markerId);
    void resetMarkedElements(int markerId);
    void printArray() const;

    // Synchronization methods
    void signalMarkerBlocked();
    void waitForAllMarkersBlocked();
    void signalAllMarkersToStart();
    void signalAllMarkersToContinue();
    void waitForStartSignal();
    void waitForContinueOrTerminateSignal(bool& shouldTerminate);
    void incrementBlockedCount();
    void decrementBlockedCount();
    int getBlockedCount() const;

private:
    std::vector<int> array;
    mutable std::mutex arrayMutex;

    // Synchronization variables
    std::condition_variable startCondition;
    std::condition_variable continueCondition;
    std::condition_variable allBlockedCondition;
    mutable std::mutex syncMutex;

    bool startSignal;
    int totalMarkers;
    int blockedMarkers;

    // Keeps track of which elements were marked by which marker
    std::vector<std::vector<int>> markerElements;
};

#endif // ARRAY_MANAGER_H