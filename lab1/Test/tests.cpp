#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <cstring>
#include "../Reporter/employeeReport.h"

TEST(EmployeeTest, CreateEmployee) {
    employee emp = { 1, "Ivan", 40.5 };
    ASSERT_EQ(emp.num, 1);
    ASSERT_STREQ(emp.name, "Ivan");
    ASSERT_DOUBLE_EQ(emp.hours, 40.5);
}

TEST(ReporterTest, CalculateSalary) {
    employee emp = { 2, "Alex", 30.0 };
    employeeReport report(emp, 500.0);
    ASSERT_EQ(report.num, 2);
    ASSERT_STREQ(report.name, "Alex");
    ASSERT_DOUBLE_EQ(report.hours, 30.0);
    ASSERT_DOUBLE_EQ(report.salary, 15000.0);
}

TEST(FileTest, CreateAndReadFile) {
    std::string filename = "testfile.bin";
    {
        std::ofstream out(filename, std::ios::binary);
        employee emp = { 3, "John", 20.0 };
        out.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
        out.close();
    }

    std::ifstream in(filename, std::ios::binary);
    ASSERT_TRUE(in.is_open());

    employee empRead;
    in.read(reinterpret_cast<char*>(&empRead), sizeof(employee));
    ASSERT_EQ(empRead.num, 3);
    ASSERT_STREQ(empRead.name, "John");
    ASSERT_DOUBLE_EQ(empRead.hours, 20.0);

    in.close();
    remove(filename.c_str());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return result;
}
