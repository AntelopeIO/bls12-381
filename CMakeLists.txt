cmake_minimum_required(VERSION 3.8)
project(bls12_381 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall)

# compile with "-march" or enable cpu features explicitly for best performance:
#set(CMAKE_CXX_FLAGS "-madx -mbmi2")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release"
    CACHE STRING "Possible values are empty, Debug, Release, RelWithDebInfo, MinSizeRel, ..."
    FORCE
  )
endif()

# the library
add_subdirectory(src)

# the tests
enable_testing()
add_subdirectory(test)

# the benchmarks
add_subdirectory(bench)
