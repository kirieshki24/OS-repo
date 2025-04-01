#define NOMINMAX
#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <locale>
#include "employee.h"

void DisplayBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "������ �������� ����� " << filename << " ��� ������" << std::endl;
        return;
    }

    employee emp;
    std::cout << "\n���������� ��������� ����� " << filename << ":\n";
    std::cout << "�����\t���\t\t����\n";
    std::cout << "--------------------------------\n";

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        std::cout << emp.num << "\t" << emp.name << "\t\t" << emp.hours << std::endl;
    }

    file.close();
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);;

    std::string binaryFileName;
    int recordCount;
    std::string reportFileName;
    double hourlyRate;

    std::cout << "������� ��� ��������� �����: ";
    std::cin >> binaryFileName;

    std::cout << "������� ���������� �������: ";
    std::cin >> recordCount;

    wchar_t currentDir[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, currentDir);

    std::wstring creatorPath = std::wstring(currentDir) + L"\\Creator.exe";

    if (GetFileAttributesW(creatorPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "������: Creator.exe �� ������ � ������� ����������." << std::endl;
        std::cerr << "��������� Creator.exe � ����������: " << std::string(currentDir, currentDir + wcslen(currentDir)) << std::endl;
        return 1;
    }

    std::wstring wCreatorCmd = creatorPath + L" " + std::wstring(binaryFileName.begin(), binaryFileName.end()) + L" " + std::to_wstring(recordCount);

    STARTUPINFOW siCreator = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION piCreator;

    if (!CreateProcessW(NULL, const_cast<LPWSTR>(wCreatorCmd.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &siCreator, &piCreator)) {
        DWORD error = GetLastError();
        wchar_t errorMsg[256];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, errorMsg, 256, NULL);
        std::wcerr << L"������ ������� Creator.exe: " << error << L" - " << errorMsg << std::endl;
        return 1;
    }

    WaitForSingleObject(piCreator.hProcess, INFINITE);

    CloseHandle(piCreator.hProcess);
    CloseHandle(piCreator.hThread);

    DisplayBinaryFile(binaryFileName);

    std::cout << "\n������� ��� ����� ������: ";
    std::cin >> reportFileName;

    std::cout << "������� ������ �� ��� ������: ";
    std::cin >> hourlyRate;

    GetCurrentDirectoryW(MAX_PATH, currentDir);

    std::wstring reporterPath = std::wstring(currentDir) + L"\\Reporter.exe";

    if (GetFileAttributesW(reporterPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "������: Reporter.exe �� ������ � ������� ����������." << std::endl;
        std::cerr << "��������� Reporter.exe � ����������: " << std::string(currentDir, currentDir + wcslen(currentDir)) << std::endl;
        return 1;
    }

    std::wstring wReporterCmd = reporterPath + L" " + std::wstring(binaryFileName.begin(), binaryFileName.end()) + L" "
        + std::wstring(reportFileName.begin(), reportFileName.end()) + L" " + std::to_wstring(hourlyRate);

    STARTUPINFOW siReporter = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION piReporter;

    if (!CreateProcessW(NULL, const_cast<LPWSTR>(wReporterCmd.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &siReporter, &piReporter)) {
        DWORD error = GetLastError();
        wchar_t errorMsg[256];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, errorMsg, 256, NULL);
        std::wcerr << L"������ ������� Reporter.exe: " << error << L" - " << errorMsg << std::endl;
        return 1;
    }

    WaitForSingleObject(piReporter.hProcess, INFINITE);

    CloseHandle(piReporter.hProcess);
    CloseHandle(piReporter.hThread);

    std::cout << "\n������ ��������� Main ���������.\n";

    return 0;
}