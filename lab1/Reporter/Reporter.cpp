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
        std::cerr << "������ �������� ����� " << filename << " ��� ������" << std::endl;
        return;
    }

    std::cout << "\n���������� ����� ������ " << filename << ":\n";
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
        std::cerr << "�������������: Reporter.exe <���_���������_�����> <���_�����_������> <������_��_���>" << std::endl;
        return 1;
    }

    std::string binaryFileName = argv[1];
    std::string reportFileName = argv[2];
    double hourlyRate;

    try {
        hourlyRate = std::stod(argv[3]);
        if (hourlyRate <= 0) {
            std::cerr << "������ �� ��� ������ ���� ������������� ������" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "������ �������������� ������ �� ���: " << e.what() << std::endl;
        return 1;
    }

    std::ifstream inFile(binaryFileName, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "������ �������� ����� " << binaryFileName << " ��� ������" << std::endl;
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
        std::cerr << "������ �������� ����� ������ " << reportFileName << std::endl;
        return 1;
    }

    outFile << "����� �� ����� \"" << binaryFileName << "\"" << std::endl;
    outFile << "����� ���������� ��� ����������\t����\t��������" << std::endl;
    outFile << "------------------------------------------------------" << std::endl;

    for (const auto& emp : employees) {
        outFile << std::setw(15) << emp.num << "\t"
            << std::setw(10) << emp.name << "\t"
            << std::fixed << std::setprecision(2) << std::setw(4) << emp.hours << "\t"
            << std::fixed << std::setprecision(2) << std::setw(5) << emp.salary << std::endl;
    }

    DisplayReportFile(reportFileName);

    outFile.close();
    std::cout << "����� ������� ������ � ����� " << reportFileName << std::endl;

    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return 0;
}