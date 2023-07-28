cmake_minimum_required(VERSION 3.12)
project(bls12-381 VERSION 1.0.0)

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
