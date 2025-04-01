#include <gtest/gtest.h>
#include "../lab2/lab2.h"

TEST(MinMaxTest, BasicTest) {
    int arr[] = { 5, -3, 10, 0, 7 };
    int min, max;
    std::cout << "Testing find_min_max..." << std::endl;
    std::cout.flush();
    find_min_max(arr, 5, min, max);
    ASSERT_EQ(min, -3);
    ASSERT_EQ(max, 10);
}

TEST(AverageTest, BasicTest) {
    int arr[] = { 1, 2, 3, 4, 5 };
    std::cout << "Testing Average..." << std::endl;
    double avg = calculate_average(arr, 5);
    ASSERT_DOUBLE_EQ(avg, 3.0);
}

TEST(ArrayModificationTest, ReplaceTest) {
    int arr[] = { 5, -3, 10 };
    double avg = 4.0;
    std::cout << "Testing replace..." << std::endl;
    replace_min_max(arr, 3, -3, 10, avg);
    ASSERT_EQ(arr[1], 4);
    ASSERT_EQ(arr[2], 4);
}

int main(int argc, char** argv) {
    std::cout << "Testing find_min_max..." << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}