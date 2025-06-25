#include <gtest/gtest.h>
#include "array_manager.h"
#include "marker_thread.h"
#include <thread>
#include <chrono>
#include <vector>

// Тесты для ArrayManager
class ArrayManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем тестовый объект ArrayManager
        arrayManager = new ArrayManager(10);
    }

    void TearDown() override {
        delete arrayManager;
    }

    ArrayManager* arrayManager;
};

// Проверка инициализации массива
TEST_F(ArrayManagerTest, Initialization) {
    EXPECT_EQ(arrayManager->getSize(), 10);
    for (int i = 0; i < arrayManager->getSize(); ++i) {
        EXPECT_EQ(arrayManager->getElement(i), 0);
    }
}

// Проверка маркировки элементов
TEST_F(ArrayManagerTest, MarkElement) {
    // Маркируем элемент с индексом 5 значением 1
    bool result = arrayManager->tryMarkElement(5, 1);
    EXPECT_TRUE(result);
    EXPECT_EQ(arrayManager->getElement(5), 1);

    // Пытаемся маркировать уже помеченный элемент
    result = arrayManager->tryMarkElement(5, 2);
    EXPECT_FALSE(result);
    EXPECT_EQ(arrayManager->getElement(5), 1); // Должен остаться с маркером 1
}

// Проверка сброса помеченных элементов
TEST_F(ArrayManagerTest, ResetMarkedElements) {
    // Маркируем несколько элементов
    arrayManager->tryMarkElement(1, 1);
    arrayManager->tryMarkElement(3, 1);
    arrayManager->tryMarkElement(5, 1);
    arrayManager->tryMarkElement(7, 2);

    // Проверяем, что элементы помечены правильно
    EXPECT_EQ(arrayManager->getElement(1), 1);
    EXPECT_EQ(arrayManager->getElement(3), 1);
    EXPECT_EQ(arrayManager->getElement(5), 1);
    EXPECT_EQ(arrayManager->getElement(7), 2);

    // Сбрасываем маркеры потока 1
    arrayManager->resetMarkedElements(1);

    // Проверяем, что элементы сброшены корректно
    EXPECT_EQ(arrayManager->getElement(1), 0);
    EXPECT_EQ(arrayManager->getElement(3), 0);
    EXPECT_EQ(arrayManager->getElement(5), 0);
    EXPECT_EQ(arrayManager->getElement(7), 2); // Элемент потока 2 должен остаться
}

// Проверка счетчика блокированных потоков
TEST_F(ArrayManagerTest, BlockedCount) {
    EXPECT_EQ(arrayManager->getBlockedCount(), 0);

    arrayManager->incrementBlockedCount();
    EXPECT_EQ(arrayManager->getBlockedCount(), 1);

    arrayManager->incrementBlockedCount();
    EXPECT_EQ(arrayManager->getBlockedCount(), 2);

    arrayManager->decrementBlockedCount();
    // Уменьшение общего числа потоков, не влияет на блокированные
    EXPECT_EQ(arrayManager->getBlockedCount(), 2);
}

// Тесты для MarkerThread
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

// Проверка корректности геттеров
TEST_F(MarkerThreadTest, Getters) {
    EXPECT_EQ(markerThread->getId(), 1);
    EXPECT_EQ(markerThread->getMarkedCount(), 0);
}

// Тест на завершение потока по сигналу
TEST_F(MarkerThreadTest, TerminateSignal) {
    // Запускаем поток маркера в отдельном потоке
    std::thread markerThreadRunner(&MarkerThread::run, markerThread);

    // Даем сигнал на старт
    arrayManager->signalAllMarkersToStart();

    // Даем потоку время на выполнение работы (маркировку элементов)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Подаем сигнал на завершение
    markerThread->signalToTerminate();

    // Даем сигнал всем потокам, чтобы разблокировать завершаемый поток
    arrayManager->signalAllMarkersToContinue();

    // Ждем завершения потока
    markerThreadRunner.join();

    // Проверяем, что все элементы сброшены
    bool allZero = true;
    for (int i = 0; i < arrayManager->getSize(); ++i) {
        if (arrayManager->getElement(i) != 0) {
            allZero = false;
            break;
        }
    }
    EXPECT_TRUE(allZero);
}

// Тест на интеграцию между ArrayManager и MarkerThread
TEST_F(MarkerThreadTest, Integration) {
    // Создаем несколько потоков маркеров
    std::vector<MarkerThread*> markerThreads;
    std::vector<std::thread> threads;

    for (int i = 1; i <= 3; ++i) {
        MarkerThread* marker = new MarkerThread(i, *arrayManager);
        markerThreads.push_back(marker);
        threads.emplace_back(&MarkerThread::run, marker);
    }

    // Даем сигнал на старт
    arrayManager->signalAllMarkersToStart();

    // Даем время для работы
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Завершаем все потоки
    for (auto marker : markerThreads) {
        marker->signalToTerminate();
    }

    // Сигнализируем всем потокам продолжить работу
    arrayManager->signalAllMarkersToContinue();

    // Ждем завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }

    // Проверяем, что все элементы сброшены
    bool allZero = true;
    for (int i = 0; i < arrayManager->getSize(); ++i) {
        if (arrayManager->getElement(i) != 0) {
            allZero = false;
            break;
        }
    }
    EXPECT_TRUE(allZero);

    // Очистка
    for (auto marker : markerThreads) {
        delete marker;
    }
}

// Тест на поведение при полном заполнении массива
TEST_F(MarkerThreadTest, FullArray) {
    // Создаем меньший массив для быстрого заполнения
    ArrayManager smallManager(5);

    // Вручную заполняем весь массив
    for (int i = 0; i < smallManager.getSize(); ++i) {
        smallManager.tryMarkElement(i, 99); // Маркируем другим ID
    }

    // Создаем поток маркера
    MarkerThread testMarker(1, smallManager);

    // Запускаем поток в отдельном потоке
    std::thread markerThread(&MarkerThread::run, &testMarker);

    // Даем сигнал на старт
    smallManager.signalAllMarkersToStart();

    // Ждем, пока поток заблокируется
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Проверяем, что поток заблокирован
    EXPECT_EQ(smallManager.getBlockedCount(), 1);

    // Завершаем поток
    testMarker.signalToTerminate();
    smallManager.signalAllMarkersToContinue();
    markerThread.join();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}