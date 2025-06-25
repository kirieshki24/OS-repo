CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
LDFLAGS = -pthread
# Adjust these paths to match your vcpkg installation
GTEST_INCLUDE = C:/vcpkg/installed/x64-windows/include
GTEST_LIB = C:/vcpkg/installed/x64-windows/lib
# Try different library flag combinations
GTEST_FLAGS = -lgtest -lgtest_main
# Alternative library flag combinations you can try:
# GTEST_FLAGS = -lgtest -lgtest_main -pthread
# GTEST_FLAGS = -lgtest_main -lgtest

# Main targets
all: lab3 unit_tests

# Main program
lab3: main.o array_manager.o marker_thread.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Object files for main program
main.o: main.cpp array_manager.h marker_thread.h
	$(CXX) $(CXXFLAGS) -I$(GTEST_INCLUDE) -c $<

array_manager.o: array_manager.cpp array_manager.h
	$(CXX) $(CXXFLAGS) -I$(GTEST_INCLUDE) -c $<

marker_thread.o: marker_thread.cpp marker_thread.h array_manager.h
	$(CXX) $(CXXFLAGS) -I$(GTEST_INCLUDE) -c $<

# Unit tests executable
unit_tests: unit_tests.o array_manager.o marker_thread.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(GTEST_LIB) $(GTEST_FLAGS) $(LDFLAGS)

unit_tests.o: unit_tests.cpp array_manager.h marker_thread.h
	$(CXX) $(CXXFLAGS) -I$(GTEST_INCLUDE) -c $<

# Clean up
clean:
	del *.o lab3.exe unit_tests.exe

# Rebuild everything
rebuild: clean all

# Run the main program
run: lab3
	./lab3.exe

# Run the unit tests
test: unit_tests
	./unit_tests.exe

.PHONY: all clean rebuild run test