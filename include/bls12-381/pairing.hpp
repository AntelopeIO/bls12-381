#pragma once
#include <array>
#include <vector>
#include <span>
#include <functional>

namespace bls12_381
{

class fp;
class fp2;
class fp6;
class fp12;
class g1;
class g2;

namespace pairing
{
    void doubling_step(std::array<fp2, 3>& coeff, g2& r);
    void addition_step(std::array<fp2, 3>& coeff, g2& r, g2& tp);
    void pre_compute(std::array<std::array<fp2, 3>, 68>& ellCoeffs, g2& twistPoint);
    fp12 miller_loop(std::span<const std::tuple<g1, g2>> pairs, std::function<void()> yield);
    void final_exponentiation(fp12& f);
    fp12 calculate(std::span<const std::tuple<g1, g2>> pairs, std::function<void()> yield = std::function<void()>());
    void add_pair(std::vector<std::tuple<g1, g2>>& pairs, const g1& e1, const g2& e2);
} // namespace pairing

} // namespace bls12_381
