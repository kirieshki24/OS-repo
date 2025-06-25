#ifndef MARKER_THREAD_H
#define MARKER_THREAD_H

#include <mutex>
#include <condition_variable>
#include "array_manager.h"

class MarkerThread {
public:
    MarkerThread(int id, ArrayManager& arrayManager);

    void run();
    void signalToTerminate();
    int getId() const;
    int getMarkedCount() const;

private:
    int id;
    ArrayManager& arrayManager;

    // Флаг для сигнализации о завершении
    bool shouldTerminate;
    std::mutex terminateMutex;
    std::condition_variable terminateCondition;

    int markedCount;
};

#endif // MARKER_THREAD_H