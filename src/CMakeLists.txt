
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

