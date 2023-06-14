
add_library (
    bls12-381
    arithmetic.cpp
    fp.cpp
    g.cpp
    pairing.cpp
    scalar.cpp
    sha256.cpp
    signatures.cpp
)

target_include_directories(
    bls12-381
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
