#include <gtest/gtest.h>
#include "array_manager.h"
#include "marker_thread.h"
#include <thread>
#include <chrono>
#include <vector>

// ����� ��� ArrayManager
class ArrayManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // ������� �������� ������ ArrayManager
        arrayManager = new ArrayManager(10);
    }

    void TearDown() override {
        delete arrayManager;
    }

    ArrayManager* arrayManager;
};

// �������� ������������� �������
TEST_F(ArrayManagerTest, Initialization) {
    EXPECT_EQ(arrayManager->getSize(), 10);
    for (int i = 0; i < arrayManager->getSize(); ++i) {
        EXPECT_EQ(arrayManager->getElement(i), 0);
    }
}

// �������� ���������� ���������
TEST_F(ArrayManagerTest, MarkElement) {
    // ��������� ������� � �������� 5 ��������� 1
    bool result = arrayManager->tryMarkElement(5, 1);
    EXPECT_TRUE(result);
    EXPECT_EQ(arrayManager->getElement(5), 1);

    // �������� ����������� ��� ���������� �������
    result = arrayManager->tryMarkElement(5, 2);
    EXPECT_FALSE(result);
    EXPECT_EQ(arrayManager->getElement(5), 1); // ������ �������� � �������� 1
}

// �������� ������ ���������� ���������
TEST_F(ArrayManagerTest, ResetMarkedElements) {
    // ��������� ��������� ���������
    arrayManager->tryMarkElement(1, 1);
    arrayManager->tryMarkElement(3, 1);
    arrayManager->tryMarkElement(5, 1);
    arrayManager->tryMarkElement(7, 2);

    // ���������, ��� �������� �������� ���������
    EXPECT_EQ(arrayManager->getElement(1), 1);
    EXPECT_EQ(arrayManager->getElement(3), 1);
    EXPECT_EQ(arrayManager->getElement(5), 1);
    EXPECT_EQ(arrayManager->getElement(7), 2);

    // ���������� ������� ������ 1
    arrayManager->resetMarkedElements(1);

    // ���������, ��� �������� �������� ���������
    EXPECT_EQ(arrayManager->getElement(1), 0);
    EXPECT_EQ(arrayManager->getElement(3), 0);
    EXPECT_EQ(arrayManager->getElement(5), 0);
    EXPECT_EQ(arrayManager->getElement(7), 2); // ������� ������ 2 ������ ��������
}

// �������� �������� ������������� �������
TEST_F(ArrayManagerTest, BlockedCount) {
    EXPECT_EQ(arrayManager->getBlockedCount(), 0);

    arrayManager->incrementBlockedCount();
    EXPECT_EQ(arrayManager->getBlockedCount(), 1);

    arrayManager->incrementBlockedCount();
    EXPECT_EQ(arrayManager->getBlockedCount(), 2);

    arrayManager->decrementBlockedCount();
    // ���������� ������ ����� �������, �� ������ �� �������������
    EXPECT_EQ(arrayManager->getBlockedCount(), 2);
}

// ����� ��� MarkerThread
class MarkerThreadTest : public ::testing::Test {
protected:
    void SetUp() override {
        arrayManager = new ArrayManager(10);
        markerThread = new MarkerThread(1, *arrayManager);
    }

    void TearDown() override {
        delete markerThread;
        delete arrayManager;
    }

    ArrayManager* arrayManager;
    MarkerThread* markerThread;
};

// �������� ������������ ��������
TEST_F(MarkerThreadTest, Getters) {
    EXPECT_EQ(markerThread->getId(), 1);
    EXPECT_EQ(markerThread->getMarkedCount(), 0);
}

// ���� �� ���������� ������ �� �������
TEST_F(MarkerThreadTest, TerminateSignal) {
    // ��������� ����� ������� � ��������� ������
    std::thread markerThreadRunner(&MarkerThread::run, markerThread);

    // ���� ������ �� �����
    arrayManager->signalAllMarkersToStart();

    // ���� ������ ����� �� ���������� ������ (���������� ���������)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // ������ ������ �� ����������
    markerThread->signalToTerminate();

    // ���� ������ ���� �������, ����� �������������� ����������� �����
    arrayManager->signalAllMarkersToContinue();

    // ���� ���������� ������
    markerThreadRunner.join();

    // ���������, ��� ��� �������� ��������
    bool allZero = true;
    for (int i = 0; i < arrayManager->getSize(); ++i) {
        if (arrayManager->getElement(i) != 0) {
            allZero = false;
            break;
        }
    }
    EXPECT_TRUE(allZero);
}

// ���� �� ���������� ����� ArrayManager � MarkerThread
TEST_F(MarkerThreadTest, Integration) {
    // ������� ��������� ������� ��������
    std::vector<MarkerThread*> markerThreads;
    std::vector<std::thread> threads;

    for (int i = 1; i <= 3; ++i) {
        MarkerThread* marker = new MarkerThread(i, *arrayManager);
        markerThreads.push_back(marker);
        threads.emplace_back(&MarkerThread::run, marker);
    }

    // ���� ������ �� �����
    arrayManager->signalAllMarkersToStart();

    // ���� ����� ��� ������
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // ��������� ��� ������
    for (auto marker : markerThreads) {
        marker->signalToTerminate();
    }

    // ������������� ���� ������� ���������� ������
    arrayManager->signalAllMarkersToContinue();

    // ���� ���������� ���� �������
    for (auto& t : threads) {
        t.join();
    }

    // ���������, ��� ��� �������� ��������
    bool allZero = true;
    for (int i = 0; i < arrayManager->getSize(); ++i) {
        if (arrayManager->getElement(i) != 0) {
            allZero = false;
            break;
        }
    }
    EXPECT_TRUE(allZero);

    // �������
    for (auto marker : markerThreads) {
        delete marker;
    }
}

// ���� �� ��������� ��� ������ ���������� �������
TEST_F(MarkerThreadTest, FullArray) {
    // ������� ������� ������ ��� �������� ����������
    ArrayManager smallManager(5);

    // ������� ��������� ���� ������
    for (int i = 0; i < smallManager.getSize(); ++i) {
        smallManager.tryMarkElement(i, 99); // ��������� ������ ID
    }

    // ������� ����� �������
    MarkerThread testMarker(1, smallManager);

    // ��������� ����� � ��������� ������
    std::thread markerThread(&MarkerThread::run, &testMarker);

    // ���� ������ �� �����
    smallManager.signalAllMarkersToStart();

    // ����, ���� ����� �������������
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // ���������, ��� ����� ������������
    EXPECT_EQ(smallManager.getBlockedCount(), 1);

    // ��������� �����
    testMarker.signalToTerminate();
    smallManager.signalAllMarkersToContinue();
    markerThread.join();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}