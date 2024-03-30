#include <bls12-381/bls12-381.hpp>

using namespace std;

namespace bls12_381
{
namespace pairing
{

void doubling_step(array<fp2, 3>& coeff, g2& r)
{
    // Adaptation of Formula 3 in https://eprint.iacr.org/2010/526.pdf
    fp2 t[10];
    t[0] = r.x.multiply(r.y);
    t[0] = t[0].mulByFq(fp::twoInv);
    t[1] = r.y.square();
    t[2] = r.z.square();
    t[7] = t[2].dbl();
    t[7] = t[7].add(t[2]);
    t[3] = t[7].mulByB();
    t[4] = t[3].dbl();
    t[4] = t[4].add(t[3]);
    t[5] = t[1].add(t[4]);
    t[5] = t[5].mulByFq(fp::twoInv);
    t[6] = r.y.add(r.z);
    t[6] = t[6].square();
    t[7] = t[2].add(t[1]);
    t[6] = t[6].subtract(t[7]);
    coeff[0] = t[3].subtract(t[1]);
    t[7] = r.x.square();
    t[4] = t[1].subtract(t[4]);
    r.x = t[4].multiply(t[0]);
    t[2] = t[3].square();
    t[3] = t[2].dbl();
    t[3] = t[3].add(t[2]);
    t[5] = t[5].square();
    r.y = t[5].subtract(t[3]);
    r.z = t[1].multiply(t[6]);
    t[0] = t[7].dbl();
    coeff[1] = t[0].add(t[7]);
    coeff[2] = t[6].negate();
}

void addition_step(array<fp2, 3>& coeff, g2& r, const g2& tp)
{
    // Algorithm 12 in https://eprint.iacr.org/2010/526.pdf
    fp2 t[10];
    t[0] = tp.y.multiply(r.z);
    t[0] = t[0].negate();
    t[0] = t[0].add(r.y);
    t[1] = tp.x.multiply(r.z);
    t[1] = t[1].negate();
    t[1] = t[1].add(r.x);
    t[2] = t[0].square();
    t[3] = t[1].square();
    t[4] = t[1].multiply(t[3]);
    t[2] = r.z.multiply(t[2]);
    t[3] = r.x.multiply(t[3]);
    t[5] = t[3].dbl();
    t[5] = t[4].subtract(t[5]);
    t[5] = t[5].add(t[2]);
    r.x = t[1].multiply(t[5]);
    t[2] = t[3].subtract(t[5]);
    t[2] = t[2].multiply(t[0]);
    t[3] = r.y.multiply(t[4]);
    r.y = t[2].subtract(t[3]);
    r.z = r.z.multiply(t[4]);
    t[2] = t[1].multiply(tp.y);
    t[3] = t[0].multiply(tp.x);
    coeff[0] = t[3].subtract(t[2]);
    coeff[1] = t[0].negate();
    coeff[2] = t[1];
}

void pre_compute(array<array<fp2, 3>, 68>& ellCoeffs, const g2& twistPoint)
{
    // Algorithm 5 in  https://eprint.iacr.org/2019/077.pdf
    if(twistPoint.isZero())
    {
        return;
    }
    g2 r = twistPoint;
    int64_t k = 0;
    for(int64_t i = 64 - 2; i >= 0; i--)
    {
        doubling_step(ellCoeffs[k], r);
        if(((g2::cofactorEFF[0] >> i) & 1) == 1)
        {
            k++;
            addition_step(ellCoeffs[k], r, twistPoint);
        }
        k++;
    }
}

fp12 miller_loop(std::span<const std::tuple<g1, g2>> pairs, std::function<void()> yield)
{
    vector<array<array<fp2, 3>, 68>> ellCoeffs;
    ellCoeffs.resize(pairs.size());
    for(uint64_t i = 0; i < pairs.size(); i++)
    {
        pre_compute(ellCoeffs[i], get<g2>(pairs[i]));
    }
    if(pairs.size() >= 20 && yield)
    {
        yield();
    }
    fp2 t[10];
    fp12 f = fp12::one();
    int64_t k = 0;
    for(int64_t i = 64 - 2; i >= 0; i--)
    {
        if(i != 64 - 2)
        {
            f = f.square();
        }
        for(uint64_t j = 0; j <= pairs.size()-1; j++)
        {
            t[0] = ellCoeffs[j][k][2].mulByFq(get<g1>(pairs[j]).y);
            t[1] = ellCoeffs[j][k][1].mulByFq(get<g1>(pairs[j]).x);
            f.mulBy014Assign(ellCoeffs[j][k][0], t[1], t[0]);
        }
        if(((g2::cofactorEFF[0] >> i) & 1) == 1)
        {
            k++;
            for(uint64_t j = 0; j <= pairs.size()-1; j++)
            {
                t[0] = ellCoeffs[j][k][2].mulByFq(get<g1>(pairs[j]).y);
                t[1] = ellCoeffs[j][k][1].mulByFq(get<g1>(pairs[j]).x);
                f.mulBy014Assign(ellCoeffs[j][k][0], t[1], t[0]);
            }
        }
        k++;
    }
    f = f.conjugate();
    return f;
}

void final_exponentiation(fp12& f)
{
    fp12 t[9];
    // easy part
    t[0] = f.frobeniusMap(6);
    t[1] = f.inverse();
    t[2] = t[0].multiply(t[1]);
    t[1] = t[2];
    t[2].frobeniusMapAssign(2);
    t[2].multiplyAssign(t[1]);
    t[1] = t[2].cyclotomicSquare();
    t[1] = t[1].conjugate();
    // hard part
    t[3] = t[2].cyclotomicExp(g2::cofactorEFF);
    t[3] = t[3].conjugate();
    t[4] = t[3].cyclotomicSquare();
    t[5] = t[1].multiply(t[3]);
    t[1] = t[5].cyclotomicExp(g2::cofactorEFF);
    t[1] = t[1].conjugate();
    t[0] = t[1].cyclotomicExp(g2::cofactorEFF);
    t[0] = t[0].conjugate();
    t[6] = t[0].cyclotomicExp(g2::cofactorEFF);
    t[6] = t[6].conjugate();
    t[6].multiplyAssign(t[4]);
    t[4] = t[6].cyclotomicExp(g2::cofactorEFF);
    t[4] = t[4].conjugate();
    t[5] = t[5].conjugate();
    t[4].multiplyAssign(t[5]);
    t[4].multiplyAssign(t[2]);
    t[5] = t[2].conjugate();
    t[1].multiplyAssign(t[2]);
    t[1].frobeniusMapAssign(3);
    t[6].multiplyAssign(t[5]);
    t[6].frobeniusMapAssign(1);
    t[3].multiplyAssign(t[0]);
    t[3].frobeniusMapAssign(2);
    t[3].multiplyAssign(t[1]);
    t[3].multiplyAssign(t[6]);
    f = t[3].multiply(t[4]);
}

fp12 calculate(std::span<const std::tuple<g1, g2>> pairs, std::function<void()> yield)
{
    fp12 f = fp12::one();
    if(pairs.empty())
    {
        return f;
    }
    f = miller_loop(pairs, yield);
    final_exponentiation(f);
    return f;
}

void add_pair(vector<tuple<g1, g2>>& pairs, const g1& e1, const g2& e2)
{
    if(!(e1.isZero() || e2.isZero()))
    {
        pairs.emplace_back(e1.affine(), e2.affine());
    }
}

} // namespace pairing
} // namespace bls12_381
