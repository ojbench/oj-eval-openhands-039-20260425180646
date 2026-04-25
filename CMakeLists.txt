

cmake_minimum_required(VERSION 3.10)
project(CSRMatrix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create an executable that will be used for testing
add_executable(code main.cpp)

# Include the current directory for header files
target_include_directories(code PRIVATE .)

