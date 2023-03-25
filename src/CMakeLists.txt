
add_library (
    bls12_381
    arithmetic.cpp
    fp.cpp
    g.cpp
    pairing.cpp
    scalar.cpp
    sha256.cpp
    signatures.cpp
)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    # On 64 bit machines enable CPU extensions for fast ECC per default.
    # ADX:  available in Intel CPUs since 2014 and AMD CPUs since 2017
    # BMI2: available in Intel CPUs since 2013 and AMD CPUs since 2015
    set(CMAKE_CXX_FLAGS "-madx -mbmi2")
    message(STATUS "X86_64 architecture detected: enable support for ADX and BMI2")
endif()

find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARY gmp)
if(GMP_LIBRARY MATCHES ${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(gmp_library_type SHARED)
    message(STATUS "GMP: ${GMP_LIBRARY}, ${GMP_INCLUDE_DIR}")
    add_library(GMP::gmp ${gmp_library_type} IMPORTED)
    set_target_properties(
        GMP::gmp PROPERTIES
        IMPORTED_LOCATION ${GMP_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${GMP_INCLUDE_DIR}
    )
    add_definitions(-D__USE_GMP)
    target_include_directories(
        bls12_381
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${GMP_INCLUDE_DIR}
    )
    target_link_libraries(bls12_381 PUBLIC GMP::gmp)
else()
    message(STATUS "GMP shared library not found - building without" )
    target_include_directories(
        bls12_381
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )
endif()

