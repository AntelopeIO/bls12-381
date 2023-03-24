# BLS12-381
All things BLS! Features include:

- High speed assembly implementation for x86_64
- G1/G2 Arithmetic & Pairing
- Aggregate Signatures
- [EIP-2537](https://eips.ethereum.org/EIPS/eip-2537)
- [EIP-2333](https://eips.ethereum.org/EIPS/eip-2333)
- Zero dependency build possible (only stdlib, WASM support)
- Uses GMP (if available) for modular reduction

Coming soon:
- Groth16

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

Chia Network's [bls-signatures](https://github.com/Chia-Network/bls-signatures) library vs. this library:
```
Chia Network's library:                 This library:
Signing                                 Signing
Total: 5000 runs in 5447 ms             Total: 5000 runs in 4666 ms
Avg: 1.0894 ms                          Avg: 0.9332 ms

Verification                            Verification
Total: 10000 runs in 32285 ms           Total: 10000 runs in 24773 ms
Avg: 3.2285 ms                          Avg: 2.4773 ms

Public key validation                   Public key validation
Total: 100000 runs in 26464 ms          Total: 100000 runs in 2066 ms
Avg: 0.26464 ms                         Avg: 0.02066 ms

Signature validation                    Signature validation
Total: 100000 runs in 28818 ms          Total: 100000 runs in 12575 ms
Avg: 0.28818 ms                         Avg: 0.12575 ms

Aggregation                             Aggregation
Total: 100000 runs in 287 ms            Total: 100000 runs in 226 ms
Avg: 0.00287 ms                         Avg: 0.00226 ms

Batch verification                      Batch verification
Total: 100000 runs in 138664 ms         Total: 100000 runs in 101991 ms
Avg: 1.38664 ms                         Avg: 1.01991 ms

PopScheme Aggregation                   PopScheme Aggregation
Total: 5000 runs in 14 ms               Total: 5000 runs in 11 ms
Avg: 0.0028 ms                          Avg: 0.0022 ms

PopScheme Proofs verification           PopScheme Proofs verification
Total: 5000 runs in 16567 ms            Total: 5000 runs in 13017 ms
Avg: 3.3134 ms                          Avg: 2.6034 ms

PopScheme verification                  PopScheme verification
Total: 5000 runs in 10 ms               Total: 5000 runs in 5 ms
Avg: 0.002 ms                           Avg: 0.001 ms
```

## Clean
```
make clean
```

## Possible Improvements
- Modular Multiplication using AVX2 [section 11.16.2](https://www.cs.princeton.edu/courses/archive/spr18/cos217/reading/x86-64-opt.pdf)