#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <locale>
#include <windows.h>
#include "../Main/employee.h"

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc != 3) {
        std::cerr << "�������������: Creator.exe <���_�����> <����������_�������>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    int recordCount;

    try {
        recordCount = std::stoi(argv[2]);
        if (recordCount <= 0) {
            std::cerr << "���������� ������� ������ ���� ������������� ������" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "������ �������������� ���������� �������: " << e.what() << std::endl;
        return 1;
    }

    std::ofstream outFile(fileName, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "������ �������� ����� " << fileName << std::endl;
        return 1;
    }

    std::cout << "������� ������ " << recordCount << " �����������:" << std::endl;

    for (int i = 0; i < recordCount; i++) {
        employee emp;
        std::string tempName;

        std::cout << "\n��������� #" << (i + 1) << std::endl;

        std::cout << "����������������� �����: ";
        std::cin >> emp.num;

        std::cout << "��� (�� 9 ��������): ";
        std::cin >> tempName;
        strncpy(emp.name, tempName.c_str(), 9);
        emp.name[9] = '\0';

        std::cout << "���������� ������������ �����: ";
        std::cin >> emp.hours;

        outFile.write(reinterpret_cast<const char*>(&emp), sizeof(employee));

        if (outFile.fail()) {
            std::cerr << "������ ��� ������ � ����" << std::endl;
            outFile.close();
            return 1;
        }
    }

    outFile.close();
    std::cout << "���� " << fileName << " ������� ������ � " << recordCount << " ��������." << std::endl;

    return 0;
}