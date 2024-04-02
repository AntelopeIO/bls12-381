#pragma once
#include <cmath>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <bls12-381/fp.hpp>

namespace bls12_381
{

// g1 is type for point in G1.
// g1 is both used for Affine and Jacobian point representation.
// If z is equal to one the point is considered as in affine form.
class g1
{

public:
    fp x;
    fp y;
    fp z;

    g1();
    explicit g1(const std::array<fp, 3>& e3);
    g1(const g1& e);
    static std::optional<g1> fromJacobianBytesBE(const std::span<const uint8_t, 144> in,
                                                 conv_opt opt = { .check_valid = false, .to_mont = true });
    static std::optional<g1> fromJacobianBytesLE(const std::span<const uint8_t, 144> in,
                                                 conv_opt opt = { .check_valid = false, .to_mont = true });
    static std::optional<g1> fromAffineBytesBE(const std::span<const uint8_t, 96> in,
                                               conv_opt opt = { .check_valid = false, .to_mont = true });
    static std::optional<g1> fromAffineBytesLE(const std::span<const uint8_t, 96> in,
                                               conv_opt opt = { .check_valid = false, .to_mont = true });
    static std::optional<g1> fromCompressedBytesBE(const std::span<const uint8_t, 48> in);
    void toJacobianBytesBE(const std::span<uint8_t, 144> out, const from_mont fm = from_mont::yes) const;
    void toJacobianBytesLE(const std::span<uint8_t, 144> out, const from_mont fm = from_mont::yes) const;
    void toAffineBytesBE(const std::span<uint8_t, 96> out, const from_mont fm = from_mont::yes) const;
    void toAffineBytesLE(const std::span<uint8_t, 96> out, const from_mont fm = from_mont::yes) const;
    void toCompressedBytesBE(const std::span<uint8_t, 48> out) const;
    std::array<uint8_t, 144> toJacobianBytesBE(const from_mont fm = from_mont::yes) const;
    std::array<uint8_t, 144> toJacobianBytesLE(const from_mont fm = from_mont::yes) const;
    std::array<uint8_t, 96> toAffineBytesBE(const from_mont fm = from_mont::yes) const;
    std::array<uint8_t, 96> toAffineBytesLE(const from_mont fm = from_mont::yes) const;
    std::array<uint8_t, 48> toCompressedBytesBE() const;
    static g1 zero();
    static g1 one();
    bool isZero() const;
    bool equal(const g1& e) const;
    bool inCorrectSubgroup() const;
    bool isOnCurve() const;
    bool isAffine() const;
    g1 affine() const;
    g1 add(const g1& e) const;
    void addAssign(const g1& e);
    g1 dbl() const;
    void doubleAssign();
    g1 negate() const;
    g1 subtract(const g1& e) const;
    void subtractAssign(const g1& e);
    template<size_t N> g1 scale(const std::array<uint64_t, N>& s) const;
    g1 clearCofactor() const;
    g1 glvEndomorphism() const;
    
    // Those operators are defined to support set and map.
    // They are not mathematically correct.
    // DO NOT use them to compare g1.
    auto operator<=>(const g1&) const = default;
   
    static g1 weightedSum(std::span<const g1> points, std::span<const std::array<uint64_t, 4>> scalars, const std::function<void()>& yield = std::function<void()>());
    static g1 mapToCurve(const fp& e);
    static std::tuple<fp, fp> swuMapG1(const fp& e);
    static void isogenyMapG1(fp& x, fp& y);

    static const g1 BASE;
    static const std::array<uint64_t, 1> cofactorEFF;
};

// g2 is type for point in G2.
// g2 is both used for Affine and Jacobian point representation.
// If z is equal to one the point is considered as in affine form.
class g2
{

public:
    fp2 x;
    fp2 y;
    fp2 z;

    g2();
    explicit g2(const std::array<fp2, 3>& e3);
    g2(const g2& e);
    static std::optional<g2> fromJacobianBytesBE(const std::span<const uint8_t, 288> in,
                                                 conv_opt opt = { .check_valid = false, .to_mont = true });
    static std::optional<g2> fromJacobianBytesLE(const std::span<const uint8_t, 288> in,
                                                 conv_opt opt = { .check_valid = false, .to_mont = true });
    static std::optional<g2> fromAffineBytesBE(const std::span<const uint8_t, 192> in,
                                               conv_opt opt = { .check_valid = false, .to_mont = true });
    static std::optional<g2> fromAffineBytesLE(const std::span<const uint8_t, 192> in,
                                               conv_opt opt = { .check_valid = false, .to_mont = true });
    static std::optional<g2> fromCompressedBytesBE(const std::span<const uint8_t, 96> in);
    void toJacobianBytesBE(const std::span<uint8_t, 288> out, const from_mont fm = from_mont::yes) const;
    void toJacobianBytesLE(const std::span<uint8_t, 288> out, const from_mont fm = from_mont::yes) const;
    void toAffineBytesBE(const std::span<uint8_t, 192> out, const from_mont fm = from_mont::yes) const;
    void toAffineBytesLE(const std::span<uint8_t, 192> out, const from_mont fm = from_mont::yes) const;
    void toCompressedBytesBE(const std::span<uint8_t, 96> out) const;
    std::array<uint8_t, 288> toJacobianBytesBE(const from_mont fm = from_mont::yes) const;
    std::array<uint8_t, 288> toJacobianBytesLE(const from_mont fm = from_mont::yes) const;
    std::array<uint8_t, 192> toAffineBytesBE(const from_mont fm = from_mont::yes) const;
    std::array<uint8_t, 192> toAffineBytesLE(const from_mont fm = from_mont::yes) const;
    std::array<uint8_t, 96> toCompressedBytesBE() const;
    static g2 zero();
    static g2 one();
    bool isZero() const;
    bool equal(const g2& e) const;
    bool inCorrectSubgroup() const;
    bool isOnCurve() const;
    bool isAffine() const;
    g2 affine() const;
    g2 add(const g2& e) const;
    void addAssign(const g2& e);
    g2 dbl() const;
    void doubleAssign();
    g2 negate() const;
    g2 subtract(const g2& e) const;
    void subtractAssign(const g2& e);
    g2 psi() const;
    template<size_t N> g2 scale(const std::array<uint64_t, N>& s) const;
    g2 clearCofactor() const;
    g2 frobeniusMap(int64_t power) const;

    // Those operators are defined to support set and map.
    // They are not mathematically correct.
    // DO NOT use them to compare g2.
    auto operator<=>(const g2&) const = default;

    static g2 weightedSum(std::span<const g2> points, std::span<const std::array<uint64_t, 4>> scalars, const std::function<void()>& yield = std::function<void()>());
    static g2 mapToCurve(const fp2& e);
    static std::tuple<fp2, fp2> swuMapG2(const fp2& e);
    //static void isogenyMapG2(fp2& x, fp2& y);
    g2 isogenyMap() const;

    static const g2 BASE;
    static const std::array<uint64_t, 1> cofactorEFF;
};

} // namespace bls12_381
