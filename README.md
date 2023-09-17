# BLS12-381
A high performance C++ BLS library. This library features:

- High speed assembly implementation for x86_64
- G1/G2 Arithmetic & Pairing
- Aggregate Signatures
- [EIP-2537](https://eips.ethereum.org/EIPS/eip-2537)
- [EIP-2333](https://eips.ethereum.org/EIPS/eip-2333)
- Zero dependencies
- WASM build

Reference Libraries:
- [zkcrypto/bls12_381](https://github.com/zkcrypto/bls12_381) (Rust)
- [zkcrypto/bellman](https://github.com/zkcrypto/bellman) (Rust)
- [go-ethereum/bls12381](https://github.com/ethereum/go-ethereum/tree/master/crypto/bls12381) (Go)
- [ConsenSys/bls12-381](https://github.com/ConsenSys/gnark-crypto/tree/master/ecc/bls12-381) (Go)
- [Chia-Network/bls-signatures](https://github.com/Chia-Network/bls-signatures) (C++)
- [Relic-Toolkit](https://github.com/relic-toolkit/relic) (C)

## Build
```
mkdir build
cd build
cmake ..
make -j
```

## Run unit tests
```
ctest
```

## Run benchmarks
```
./bench/eth_bench       # eth benchmarks
./bench/chia_bench      # chia benchmarks
```
The following numbers were generated running the above benchmarks (and their equivalents of Ethereum's and Chia's libraries) on an Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz.

### Ethereum Bench
Ethereum's Go implementation of [bls12381](https://github.com/ethereum/go-ethereum/tree/master/crypto/bls12381) vs. this library. Two different compilations are benchmarked:
- `go/cpp` - Native go/cpp implementation is benchmarked
- `asm` - The assembly implementation is benchmarked
```
Ethereum's library (go):                This library (cpp):                     Ethereum's library (asm):               This library (asm):
G1 Addition                             G1 Addition                             G1 Addition                             G1 Addition
886 ns/op                               875 ns/op                               743 ns/op                               747 ns/op

G1 Multiplication                       G1 Multiplication                       G1 Multiplication                       G1 Multiplication
338003 ns/op                            336085 ns/op                            281708 ns/op                            281852 ns/op

G2 Addition                             G2 Addition                             G2 Addition                             G2 Addition
2714 ns/op                              2451 ns/op                              1851 ns/op                              1848 ns/op

G2 Multiplication                       G2 Multiplication                       G2 Multiplication                       G2 Multiplication
1075886 ns/op                           1011230 ns/op                           821738 ns/op                            752900 ns/op

Pairing                                 Pairing                                 Pairing                                 Pairing
1372026 ns/op                           1231320 ns/op                           1022350 ns/op                           982783 ns/op
```
### Chia Bench
Chia Network's [bls-signatures](https://github.com/Chia-Network/bls-signatures) library vs. this library. Four different compilations are benchmarked:
- `cpu-ext-ct, asm` - CPU features ADX and BMI2 are enabled at compile-time, the x86_64 assembly implementation is used
- `cpu-ext-rt, asm` - CPU features ADX and BMI2 are enabled at run-time, the x86_64 assembly implementation is used
- `no cpu-ext, asm` - CPU features ADX and BMI2 are disabled, the x86_64 assembly implementation is used
- `no cpu-ext, cpp` - CPU features ADX and BMI2 are disabled, the C++ implementation is used
```
Chia Network's library:                 This library (cpu-ext-ct, asm):         This library (cpu-ext-rt, asm):         This library (no cpu-ext, asm):         This library (no cpu-ext, cpp):
Signing                                 Signing                                 Signing                                 Signing                                 Signing
Total: 5000 runs in 5447 ms             Total: 5000 runs in 4535 ms             Total: 5000 runs in 4726 ms             Total: 5000 runs in 6160 ms             Total: 5000 runs in 8344 ms
Avg: 1.0894 ms                          Avg: 0.907 ms                           Avg: 0.9452 ms                          Avg: 1.232 ms                           Avg: 1.6688 ms

Verification                            Verification                            Verification                            Verification                            Verification
Total: 10000 runs in 32285 ms           Total: 10000 runs in 19259 ms           Total: 10000 runs in 19727 ms           Total: 10000 runs in 24561 ms           Total: 10000 runs in 33982 ms
Avg: 3.2285 ms                          Avg: 1.9259 ms                          Avg: 1.9727 ms                          Avg: 2.4561 ms                          Avg: 3.3982 ms

Public key validation                   Public key validation                   Public key validation                   Public key validation                   Public key validation
Total: 100000 runs in 26464 ms          Total: 100000 runs in 2066 ms           Total: 100000 runs in 2092 ms           Total: 100000 runs in 3071 ms           Total: 100000 runs in 4146 ms
Avg: 0.26464 ms                         Avg: 0.02066 ms                         Avg: 0.02092 ms                         Avg: 0.03071 ms                         Avg: 0.04146 ms

Signature validation                    Signature validation                    Signature validation                    Signature validation                    Signature validation
Total: 100000 runs in 28818 ms          Total: 100000 runs in 12474 ms          Total: 100000 runs in 12665 ms          Total: 100000 runs in 17622 ms          Total: 100000 runs in 24255 ms
Avg: 0.28818 ms                         Avg: 0.12474 ms                         Avg: 0.12665 ms                         Avg: 0.17622 ms                         Avg: 0.24255 ms

Aggregation                             Aggregation                             Aggregation                             Aggregation                             Aggregation
Total: 100000 runs in 287 ms            Total: 100000 runs in 224 ms            Total: 100000 runs in 226 ms            Total: 100000 runs in 299 ms            Total: 100000 runs in 410 ms
Avg: 0.00287 ms                         Avg: 0.00224 ms                         Avg: 0.00226 ms                         Avg: 0.00299 ms                         Avg: 0.0041 ms

Batch verification                      Batch verification                      Batch verification                      Batch verification                      Batch verification
Total: 100000 runs in 138664 ms         Total: 100000 runs in 91959 ms          Total: 100000 runs in 95714 ms          Total: 100000 runs in 121560 ms         Total: 100000 runs in 164145 ms
Avg: 1.38664 ms                         Avg: 0.91959 ms                         Avg: 0.95714 ms                         Avg: 1.2156 ms                         Avg: 1.64145 ms

PopScheme Aggregation                   PopScheme Aggregation                   PopScheme Aggregation                   PopScheme Aggregation                   PopScheme Aggregation
Total: 5000 runs in 14 ms               Total: 5000 runs in 11 ms               Total: 5000 runs in 11 ms               Total: 5000 runs in 14 ms               Total: 5000 runs in 21 ms
Avg: 0.0028 ms                          Avg: 0.0022 ms                          Avg: 0.0022 ms                          Avg: 0.0028 ms                          Avg: 0.0042 ms

PopScheme Proofs verification           PopScheme Proofs verification           PopScheme Proofs verification           PopScheme Proofs verification           PopScheme Proofs verification
Total: 5000 runs in 16567 ms            Total: 5000 runs in 10315 ms            Total: 5000 runs in 10531 ms            Total: 5000 runs in 13400 ms            Total: 5000 runs in 18593 ms
Avg: 3.3134 ms                          Avg: 2.0630 ms                          Avg: 2.1062 ms                          Avg: 2.6800 ms                          Avg: 3.7186 ms

PopScheme verification                  PopScheme verification                  PopScheme verification                  PopScheme verification                  PopScheme verification
Total: 5000 runs in 10 ms               Total: 5000 runs in 5 ms                Total: 5000 runs in 6 ms                Total: 5000 runs in 7 ms                Total: 5000 runs in 10 ms
Avg: 0.0020 ms                          Avg: 0.0010 ms                          Avg: 0.0012 ms                          Avg: 0.0014 ms                          Avg: 0.0020 ms
```

## Clean
```
make clean
```

## Possible Improvements
- Modular Multiplication using AVX2 [section 11.16.2](https://www.cs.princeton.edu/courses/archive/spr18/cos217/reading/x86-64-opt.pdf)
