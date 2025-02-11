cmake_minimum_required(VERSION 3.12)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(bls12-381 VERSION 1.0.0 LANGUAGES CXX ASM)

file(GLOB BLS12-381_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB BLS12-381_HEADERS CONFIGURE_DEPENDS include/bls12-381/*.hpp)

add_library(bls12-381 ${BLS12-381_SOURCES})
target_include_directories(bls12-381 PUBLIC include)
set_target_properties(bls12-381 PROPERTIES PUBLIC_HEADER "${BLS12-381_HEADERS}")
target_compile_features(bls12-381 PUBLIC cxx_std_20)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64)
  target_sources(bls12-381 PRIVATE src/arithmetic.s)
  set_source_files_properties(src/arithmetic.s PROPERTIES COMPILE_FLAGS "-Wno-unused-command-line-argument")
endif()

# the tests
enable_testing()
add_subdirectory(test)

# the benchmarks
add_subdirectory(bench)
