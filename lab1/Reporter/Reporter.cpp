#define NOMINMAX
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <locale>
#include <cstring>
#include <windows.h>
#include "../Main/employee.h"
#include "employeeReport.h"

void DisplayReportFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла " << filename << " для чтения" << std::endl;
        return;
    }

    std::cout << "\nСодержимое файла отчета " << filename << ":\n";
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc != 4) {
        std::cerr << "Использование: Reporter.exe <имя_бинарного_файла> <имя_файла_отчета> <оплата_за_час>" << std::endl;
        return 1;
    }

    std::string binaryFileName = argv[1];
    std::string reportFileName = argv[2];
    double hourlyRate;

    try {
        hourlyRate = std::stod(argv[3]);
        if (hourlyRate <= 0) {
            std::cerr << "Оплата за час должна быть положительным числом" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка преобразования оплаты за час: " << e.what() << std::endl;
        return 1;
    }

    std::ifstream inFile(binaryFileName, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Ошибка открытия файла " << binaryFileName << " для чтения" << std::endl;
        return 1;
    }

    std::vector<employeeReport> employees;
    employee emp;

    while (inFile.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        employees.push_back(employeeReport(emp, hourlyRate));
    }

    inFile.close();

    std::sort(employees.begin(), employees.end());

    std::ofstream outFile(reportFileName);
    if (!outFile.is_open()) {
        std::cerr << "Ошибка создания файла отчета " << reportFileName << std::endl;
        return 1;
    }

    outFile << "Отчет по файлу \"" << binaryFileName << "\"" << std::endl;
    outFile << "Номер сотрудника Имя сотрудника\tЧасы\tЗарплата" << std::endl;
    outFile << "------------------------------------------------------" << std::endl;

    for (const auto& emp : employees) {
        outFile << std::setw(15) << emp.num << "\t"
            << std::setw(10) << emp.name << "\t"
            << std::fixed << std::setprecision(2) << std::setw(4) << emp.hours << "\t"
            << std::fixed << std::setprecision(2) << std::setw(5) << emp.salary << std::endl;
    }

    DisplayReportFile(reportFileName);

    outFile.close();
    std::cout << "Отчет успешно создан в файле " << reportFileName << std::endl;

    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return 0;
}