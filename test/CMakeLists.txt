add_executable(unittests unittests.cpp)
target_link_libraries(unittests bls12-381)

add_test(NAME bls12-381 COMMAND unittests)