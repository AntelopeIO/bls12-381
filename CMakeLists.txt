cmake_minimum_required(VERSION 3.8)
project(bls12_381 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall)

# the library
add_subdirectory(src)

# the tests
enable_testing()
add_subdirectory(test)