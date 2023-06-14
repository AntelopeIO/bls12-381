add_executable(chia_bench chia_bench.cpp)
target_link_libraries(chia_bench bls12-381)

add_executable(eth_bench eth_bench.cpp)
target_link_libraries(eth_bench bls12-381)