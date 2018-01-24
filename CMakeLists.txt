cmake_minimum_required(VERSION 3.10)
project(hash_brute_force)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCL REQUIRED)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Ofast")

add_executable(hash_brute_force main.cpp crc32.hpp)
target_link_libraries (hash_brute_force ${OpenCL_LIBRARY})

