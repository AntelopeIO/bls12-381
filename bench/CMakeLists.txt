add_executable(chia_bench chia_bench.cpp)
target_link_libraries(chia_bench bls12_381)

add_executable(bench bench.cpp)
target_link_libraries(bench bls12_381)