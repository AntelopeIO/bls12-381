#pragma once
#include <cstdint>
#include <cstring>
#include <array>
#include <string>
#include <stdexcept>
#include <tuple>
#include "span.h"

namespace bls12_381
{

// element representation of 'fp' field which is the base field
class fp
{

public:
    std::array<uint64_t, 6> d;

    fp();
    fp(const std::array<uint64_t, 6>& d);
    fp(const fp& e);
    static fp fromBytesBE(const std::span<const uint8_t, 48> in);
    static fp fromBytesLE(const std::span<const uint8_t, 48> in);
    void toBytesBE(const std::span<uint8_t, 48> out) const;
    void toBytesLE(const std::span<uint8_t, 48> out) const;
    std::array<uint8_t, 48> toBytesBE() const;
    std::array<uint8_t, 48> toBytesLE() const;
    static fp zero();
    static fp one();
    bool isValid() const;
    bool isOdd() const;
    bool isEven() const;
    bool isZero() const;
    bool isOne() const;
    int64_t cmp(const fp& e) const;
    bool equal(const fp& e) const;
    bool sign() const;
    void div2(const uint64_t& e);
    uint64_t mul2();
    fp toMont() const;
    fp fromMont() const;
    template<size_t N> fp exp(const std::array<uint64_t, N>& s) const;
    fp inverse() const;
    bool sqrt(fp& c) const;
    bool isQuadraticNonResidue() const;
    bool isLexicographicallyLargest() const;
    template<size_t N> static fp modPrime(std::array<uint64_t, N> k);

    static const fp MODULUS;                            // base field modulus: p = 4002409555221667393417789825735904156556882819939007885332058136124031650490837864442687629129015664037894272559787 or 0x1A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAB
    static const uint64_t INP;                          // INP = -(p^{-1} mod 2^64) mod 2^64
    static const fp R1;                                 // base field identity: R1 = 2^384 mod p
    static const fp R2;                                 // fp identity squared: R2 = 2^(384*2) mod p
    static const fp B;                                  // B coefficient from cure equation: y^2 = x^3 + B
    static const fp twoInv;
    static const std::array<uint64_t, 4> Q;                  // scalar field modulus: q = 52435875175126190479447740508185965837690552500527637822603658699938581184513 or 0x73eda753299d7d483339d80809a1d80553bda402fffe5bfeffffffff00000001
    static const std::array<uint64_t, 6> pPlus1Over4;
    static const std::array<uint64_t, 6> pMinus1Over2;
    static const std::array<uint64_t, 6> pMinus3Over4;
};

// element representation of 'fp2' field which is quadratic extension of base field 'fp'
// encoding order: c0 + c1 * u
class fp2
{

public:
    fp c0;
    fp c1;

    fp2();
    fp2(const std::array<fp, 2>& e2);
    fp2(const fp2& e);
    static fp2 fromBytesBE(const std::span<const uint8_t, 96> in);
    static fp2 fromBytesLE(const std::span<const uint8_t, 96> in);
    void toBytesBE(const std::span<uint8_t, 96> out) const;
    void toBytesLE(const std::span<uint8_t, 96> out) const;
    std::array<uint8_t, 96> toBytesBE() const;
    std::array<uint8_t, 96> toBytesLE() const;
    static fp2 zero();
    static fp2 one();
    bool isZero() const;
    bool isOne() const;
    bool equal(const fp2& e) const;
    bool sign() const;
    fp2 add(const fp2& e) const;
    void addAssign(const fp2& e);
    fp2 ladd(const fp2& e) const;
    fp2 dbl() const;
    void doubleAssign();
    fp2 ldouble() const;
    fp2 sub(const fp2& e) const;
    void subAssign(const fp2& e);
    fp2 neg() const;
    fp2 mul(const fp2& e) const;
    void mulAssign(const fp2& e);
    fp2 square() const;
    void squareAssign();
    fp2 mulByNonResidue() const;
    fp2 mulByB() const;
    fp2 inverse() const;
    fp2 mulByFq(const fp& e) const;
    template<size_t N> fp2 exp(const std::array<uint64_t, N>& s) const;
    fp2 frobeniusMap(const uint64_t& power) const;
    void frobeniusMapAssign(const uint64_t& power);
    bool sqrt(fp2& c) const;
    bool isQuadraticNonResidue() const;
    bool isLexicographicallyLargest() const;

    static const fp2 negativeOne2;
    static const fp2 B;
};

// element representation of 'fp6' field which is cubic extension of 'fp2' field
// encoding order: c0 + c1 * v + c2 * v^2
class fp6
{

public:
    fp2 c0;
    fp2 c1;
    fp2 c2;

    fp6();
    fp6(const std::array<fp2, 3>& e3);
    fp6(const fp6& e);
    static fp6 fromBytesBE(const std::span<const uint8_t, 288> in);
    static fp6 fromBytesLE(const std::span<const uint8_t, 288> in);
    void toBytesBE(const std::span<uint8_t, 288> out) const;
    void toBytesLE(const std::span<uint8_t, 288> out) const;
    std::array<uint8_t, 288> toBytesBE() const;
    std::array<uint8_t, 288> toBytesLE() const;
    static fp6 zero();
    static fp6 one();
    bool isZero() const;
    bool isOne() const;
    bool equal(const fp6& e) const;
    fp6 add(const fp6& e) const;
    void addAssign(const fp6& e);
    fp6 dbl() const;
    void doubleAssign();
    fp6 sub(const fp6& e) const;
    void subAssign(const fp6& e);
    fp6 neg() const;
    fp6 mul(const fp6& e) const;
    void mulAssign(const fp6& e);
    fp6 square() const;
    void mulBy01Assign(const fp2& e0, const fp2& e1);
    fp6 mulBy01(const fp2& e0, const fp2& e1) const;
    fp6 mulBy1(const fp2& e1) const;
    fp6 mulByNonResidue() const;
    fp6 mulByBaseField(const fp2& e) const;
    template<size_t N> fp6 exp(const std::array<uint64_t, N>& s) const;
    fp6 inverse() const;
    fp6 frobeniusMap(const uint64_t& power) const;
    void frobeniusMapAssign(const uint64_t& power);

    static const std::array<fp2, 6> frobeniusCoeffs61;
    static const std::array<fp2, 6> frobeniusCoeffs62;
};

// element representation of 'fp12' field which is quadratic extension of 'fp6' field
// encoding order: c0 + c1 * w
class fp12
{

public:
    fp6 c0;
    fp6 c1;

    fp12();
    fp12(const std::array<fp6, 2>& e2);
    fp12(const fp12& e);
    static fp12 fromBytesBE(const std::span<const uint8_t, 576> in);
    static fp12 fromBytesLE(const std::span<const uint8_t, 576> in);
    void toBytesBE(const std::span<uint8_t, 576> out) const;
    void toBytesLE(const std::span<uint8_t, 576> out) const;
    std::array<uint8_t, 576> toBytesBE() const;
    std::array<uint8_t, 576> toBytesLE() const;
    static fp12 zero();
    static fp12 one();
    bool isZero() const;
    bool isOne() const;
    bool isGtValid() const;
    bool equal(const fp12& e) const;
    fp12 add(const fp12& e) const;
    fp12 dbl() const;
    fp12 sub(const fp12& e) const;
    fp12 neg() const;
    fp12 conjugate() const;
    fp12 square() const;
    fp12 cyclotomicSquare() const;
    fp12 mul(const fp12& e) const;
    void mulAssign(const fp12& e);
    static std::tuple<fp2, fp2> fp4Square(const fp2& e0, const fp2& e1);
    fp12 inverse() const;
    void mulBy014Assign(const fp2& e0, const fp2& e1, const fp2& e4);
    template<size_t N> fp12 exp(const std::array<uint64_t, N>& s) const;
    template<size_t N> fp12 cyclotomicExp(const std::array<uint64_t, N>& s) const;
    fp12 frobeniusMap(const uint64_t& power) const;
    void frobeniusMapAssign(const uint64_t& power);

    static const std::array<fp2, 12> frobeniusCoeffs12;
};

} // namespace bls12_381