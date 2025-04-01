cmake_minimum_required(VERSION 3.20)
project(EmployeeApp)

# Указываем путь к vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Путь к заголовочным файлам проекта (папка Main)
include_directories(${CMAKE_SOURCE_DIR}/Main)

# Настройка выходного каталога
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Основные исполняемые файлы
add_executable(Creator ../Creator/Creator.cpp)
add_executable(Reporter ../Reporter/Reporter.cpp)
add_executable(Main ../Main/Main.cpp)

# Поиск GTest
find_package(GTest REQUIRED CONFIG)

if(GTest_FOUND AND TARGET GTest::gtest AND TARGET GTest::gtest_main)
    # Тесты
    add_executable(EmployeeTests ../Test/tests.cpp)
    target_link_libraries(EmployeeTests 
        PRIVATE 
        GTest::gtest 
        GTest::gtest_main
    )
    # Добавляем тест в CTest
    add_test(NAME EmployeeTests COMMAND EmployeeTests)
else()
    message(WARNING "GTest не найден. Тесты не будут собраны.")
endif()