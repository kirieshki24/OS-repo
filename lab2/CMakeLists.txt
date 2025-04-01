cmake_minimum_required(VERSION 3.20)
project(Lab2_Threads)

# Указываем путь к vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(GTest_DIR "C:/vcpkg/installed/x64-windows/share/gtest")

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основной исполняемый файл
add_executable(lab2 lab2/lab2.cpp)

# Флаги компиляции
if(MSVC)
    target_compile_options(lab2 PRIVATE /W3 /WX)
else()
    target_compile_options(lab2 PRIVATE -Wall -Wextra -Werror)
endif()

# Подключение GTest
find_package(GTest REQUIRED CONFIG)

# Добавляем тесты
add_executable(tests test/tests.cpp)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)

enable_testing()
add_test(NAME ThreadTests COMMAND tests)