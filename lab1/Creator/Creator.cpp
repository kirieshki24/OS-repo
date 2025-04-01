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
        std::cerr << "Использование: Creator.exe <имя_файла> <количество_записей>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    int recordCount;

    try {
        recordCount = std::stoi(argv[2]);
        if (recordCount <= 0) {
            std::cerr << "Количество записей должно быть положительным числом" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка преобразования количества записей: " << e.what() << std::endl;
        return 1;
    }

    std::ofstream outFile(fileName, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Ошибка создания файла " << fileName << std::endl;
        return 1;
    }

    std::cout << "Введите данные " << recordCount << " сотрудников:" << std::endl;

    for (int i = 0; i < recordCount; i++) {
        employee emp;
        std::string tempName;

        std::cout << "\nСотрудник #" << (i + 1) << std::endl;

        std::cout << "Идентификационный номер: ";
        std::cin >> emp.num;

        std::cout << "Имя (до 9 символов): ";
        std::cin >> tempName;
        strncpy(emp.name, tempName.c_str(), 9);
        emp.name[9] = '\0';

        std::cout << "Количество отработанных часов: ";
        std::cin >> emp.hours;

        outFile.write(reinterpret_cast<const char*>(&emp), sizeof(employee));

        if (outFile.fail()) {
            std::cerr << "Ошибка при записи в файл" << std::endl;
            outFile.close();
            return 1;
        }
    }

    outFile.close();
    std::cout << "Файл " << fileName << " успешно создан с " << recordCount << " записями." << std::endl;

    return 0;
}