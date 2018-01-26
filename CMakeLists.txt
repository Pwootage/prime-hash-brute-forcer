cmake_minimum_required(VERSION 3.9)
project(hash_brute_force)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Ofast")

add_executable(hash_brute_force_suffix force_hash_suffix.cpp crc32.hpp)
add_executable(hash_brute_force_nosuffix force_hash_no_suffix.cpp crc32.hpp)
add_executable(find_suffix_hash find_suffix_hash.cpp crc32.hpp)


