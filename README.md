# bls12_381
All things BLS! This library supports:

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

## Clean
```
make clean
```