cmake_minimum_required(VERSION 3.10)
project(CHTL CXX)

# Set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing with CTest
enable_testing()

# Add subdirectories for the different parts of the project
add_subdirectory(Util)
add_subdirectory(CHTL)
add_subdirectory(Test)

# Include third-party directory
include_directories(third-party)