# bls12_381
All things BLS! This library supports:

- G1/G2 Arithmetic & Pairing
- Aggregate Signatures

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