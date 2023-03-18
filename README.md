# BLS12-381
All things BLS! Features include:

- High speed assembly implementation for x86_64
- G1/G2 Arithmetic & Pairing
- Aggregate Signatures
- [EIP-2537](https://eips.ethereum.org/EIPS/eip-2537)
- [EIP-2333](https://eips.ethereum.org/EIPS/eip-2333)

Coming soon:
- Groth16 Proving System
- SIMD support (including WASM)

## Build
```
mkdir build
cd build
cmake ..
make -j
```

## Run unit tests
```
./test/unittests
```

## Run benchmarks
```
./bench/chia_bench
```

## Clean
```
make clean
```

## Possible Improvements
- Modular Multiplication using AVX2 [section 11.16.2](https://www.cs.princeton.edu/courses/archive/spr18/cos217/reading/x86-64-opt.pdf)