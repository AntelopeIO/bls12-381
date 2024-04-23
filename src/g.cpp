#include <bls12-381/bls12-381.hpp>

using namespace std;

namespace bls12_381
{

g1::g1() : x(fp()), y(fp()), z(fp())
{
}

g1::g1(const array<fp, 3>& e3) : x(e3[0]), y(e3[1]), z(e3[2])
{
}

g1::g1(const g1& e) : x(e.x), y(e.y), z(e.z)
{
}

optional<g1> g1::fromJacobianBytesBE(const span<const uint8_t, 144> in, conv_opt opt)
{
    // We decided to always validate the input here. Check flag will only affect on-curve checks.
    bool curve_check = opt.check_valid;
    opt.check_valid = true;
    optional<fp> x = fp::fromBytesBE(span<const uint8_t, 48>(&in[ 0], 48), opt);
    optional<fp> y = fp::fromBytesBE(span<const uint8_t, 48>(&in[48], 48), opt);
    optional<fp> z = fp::fromBytesBE(span<const uint8_t, 48>(&in[96], 48), opt);
    if(!x || !y || !z) return {};
    g1 p = g1({*x, *y, *z});
    if(curve_check && !p.isOnCurve())
    {
        return {};
    }
    return p;
}

optional<g1> g1::fromJacobianBytesLE(const span<const uint8_t, 144> in, conv_opt opt)
{
    // We decided to always validate the input here. Check flag will only affect on-curve checks.
    bool curve_check = opt.check_valid;
    opt.check_valid = true;
    optional<fp> x = fp::fromBytesLE(span<const uint8_t, 48>(&in[ 0], 48), opt);
    optional<fp> y = fp::fromBytesLE(span<const uint8_t, 48>(&in[48], 48), opt);
    optional<fp> z = fp::fromBytesLE(span<const uint8_t, 48>(&in[96], 48), opt);
    if(!x || !y || !z) return {};
    g1 p = g1({*x, *y, *z});
    if(curve_check && !p.isOnCurve())
    {
        return {};
    }
    return p;
}

optional<g1> g1::fromAffineBytesBE(const span<const uint8_t, 96> in, conv_opt opt)
{
    // We decided to always validate the input here. Check flag will only affect on-curve checks.
    bool curve_check = opt.check_valid;
    opt.check_valid = true;
    optional<fp> x = fp::fromBytesBE(span<const uint8_t, 48>(&in[ 0], 48), opt);
    optional<fp> y = fp::fromBytesBE(span<const uint8_t, 48>(&in[48], 48), opt);
    if(!x || !y) return {};
    // check if given input points to infinity
    if(x->isZero() && y->isZero())
    {
        return zero();
    }
    fp z = fp::one();
    g1 p = g1({*x, *y, z});
    if(curve_check && !p.isOnCurve())
    {
        return {};
    }
    return p;
}

optional<g1> g1::fromAffineBytesLE(const span<const uint8_t, 96> in, conv_opt opt)
{
    // We decided to always validate the input here. Check flag will only affect on-curve checks.
    bool curve_check = opt.check_valid;
    opt.check_valid = true;
    optional<fp> x = fp::fromBytesLE(span<const uint8_t, 48>(&in[ 0], 48), opt);
    optional<fp> y = fp::fromBytesLE(span<const uint8_t, 48>(&in[48], 48), opt);
    if(!x || !y) return {};
    // check if given input points to infinity
    if(x->isZero() && y->isZero())
    {
        return zero();
    }
    fp z = fp::one();
    g1 p = g1({*x, *y, z});
    if(curve_check && !p.isOnCurve())
    {
        return {};
    }
    return p;
}

optional<g1> g1::fromCompressedBytesBE(const span<const uint8_t, 48> in)
{
    // check compression bit
    if(((in[0] >> 7) & 1) != 1)
    {
        return {};
    }
    // check if point is infinity
    if(((in[0] >> 6) & 1) == 1)
    {
        return zero();
    }
    // reconstruct point from x coordinate
    bool ysign = ((in[0] >> 5) & 1) == 1;
    g1 p;
    scalar::fromBytesBE(in, p.x.d);
    // erase 3 msbs from given input and perform validity check
    p.x.d[5] &= 0x1FFFFFFFFFFFFFFF;
    p.x = p.x.toMont();
    if(!p.x.isValid())
    {
        return {};
    }
    // BLS 12-381 curve equation:
    //      y^2 = x^3 + B
    //  =>  y   = +/- sqrt(x^3 + B)
    fp b = fp::B;
    _square(&p.y, &p.x);        // y = x^2
    _multiply(&p.y, &p.y, &p.x);     // y = x^2 * x = x^3
    _add(&p.y, &p.y, &b);       // y = x^3 + B
    if(!p.y.sqrt(p.y))
    {
        return {};
    }
    if(p.y.isLexicographicallyLargest() ^ ysign)
    {
        _negate(&p.y, &p.y);
    }
    p.z = fp::one();
    return p;
}

void g1::toJacobianBytesBE(const span<uint8_t, 144> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[ 0], &x.toBytesBE(fm)[0], 48);
    memcpy(&out[48], &y.toBytesBE(fm)[0], 48);
    memcpy(&out[96], &z.toBytesBE(fm)[0], 48);
}

void g1::toJacobianBytesLE(const span<uint8_t, 144> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[ 0], &x.toBytesLE(fm)[0], 48);
    memcpy(&out[48], &y.toBytesLE(fm)[0], 48);
    memcpy(&out[96], &z.toBytesLE(fm)[0], 48);
}

void g1::toAffineBytesBE(const span<uint8_t, 96> out, const from_mont fm /* = from_mont::yes */) const
{
    if(isZero())
    {
        memset(&out[0], 0, 96);
        return;
    }
    g1 r = affine();
    memcpy(&out[ 0], &r.x.toBytesBE(fm)[0], 48);
    memcpy(&out[48], &r.y.toBytesBE(fm)[0], 48);
}

void g1::toAffineBytesLE(const span<uint8_t, 96> out, const from_mont fm /* = from_mont::yes */) const
{
    if(isZero())
    {
        memset(&out[0], 0, 96);
        return;
    }
    g1 r = affine();
    memcpy(&out[ 0], &r.x.toBytesLE(fm)[0], 48);
    memcpy(&out[48], &r.y.toBytesLE(fm)[0], 48);
}

void g1::toCompressedBytesBE(const span<uint8_t, 48> out) const
{
    // check: https://github.com/zcash/librustzcash/blob/6e0364cd42a2b3d2b958a54771ef51a8db79dd29/pairing/src/bls12_381/README.md#serialization
    g1 p = affine();
    // check if point is at infinity
    if(p.x.isZero() && p.y.isZero())
    {
        memset(out.data(), 0, 48);
        out[0] |= 0xC0;
        return;
    }
    memcpy(out.data(), p.x.toBytesBE().data(), 48);
    // checks if y component is larger than its negation
    if(p.y.isLexicographicallyLargest())
    {
        out[0] |= 0x20;
    }
    // set compression bit
    out[0] |= 0x80;
}

array<uint8_t, 144> g1::toJacobianBytesBE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 144> out;
    toJacobianBytesBE(out, fm);
    return out;
}

array<uint8_t, 144> g1::toJacobianBytesLE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 144> out;
    toJacobianBytesLE(out, fm);
    return out;
}

array<uint8_t, 96> g1::toAffineBytesBE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 96> out;
    toAffineBytesBE(out, fm);
    return out;
}

array<uint8_t, 96> g1::toAffineBytesLE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 96> out;
    toAffineBytesLE(out, fm);
    return out;
}

array<uint8_t, 48> g1::toCompressedBytesBE() const
{
    array<uint8_t, 48> out;
    toCompressedBytesBE(out);
    return out;
}

g1 g1::zero()
{
    return g1({
        fp::zero(),
        fp::one(),
        fp::zero()
    });
}

g1 g1::one()
{
    return BASE;
}

bool g1::isZero() const
{
    return z.isZero();
}

bool g1::equal(const g1& e) const
{
    g1 b = e;
    if(isZero())
    {
        return b.isZero();
    }
    if(b.isZero())
    {
        return isZero();
    }
    fp t[4];
    _square(&t[0], &z);
    _square(&t[1], &b.z);
    _multiply(&t[2], &t[0], &b.x);
    _multiply(&t[3], &t[1], &x);
    _multiply(&t[0], &t[0], &z);
    _multiply(&t[1], &t[1], &b.z);
    _multiply(&t[1], &t[1], &y);
    _multiply(&t[0], &t[0], &b.y);
    return t[0].equal(t[1]) && t[2].equal(t[3]);
}

bool g1::inCorrectSubgroup() const
{
    // Faster Subgroup Checks for BLS12-381
    // S. Bowe (https://eprint.iacr.org/2019/814.pdf)
    // [(x^2 − 1)/3](2σ(P) − P − σ^2(P)) − σ^2(P) ?= O
    g1 t0 = this->glvEndomorphism();
    g1 t1 = t0;
    t0 = t0.glvEndomorphism();
    t1 = t1.dbl();
    t1 = t1.subtract(*this);
    t1 = t1.subtract(t0);
    t1 = t1.scale<2>({0x0000000055555555, 0x396c8c005555e156});
    t1 = t1.subtract(t0);
    return t1.isZero();
}

bool g1::isOnCurve() const
{
    if(isZero())
    {
        return true;
    }
    fp t[4], _b = fp::B;
    _square(&t[0], &y);
    _square(&t[1], &x);
    _multiply(&t[1], &t[1], &x);
    _square(&t[2], &z);
    _square(&t[3], &t[2]);
    _multiply(&t[2], &t[2], &t[3]);
    _multiply(&t[2], &_b, &t[2]);
    _add(&t[1], &t[1], &t[2]);
    return t[0].equal(t[1]);
}

bool g1::isAffine() const
{
    return z.isOne();
}

g1 g1::affine() const
{
    if(isZero() || isAffine())
    {
        return *this;
    }
    g1 r = *this;
    fp t[2];
    t[0] = r.z.inverse();
    _square(&t[1], &t[0]);
    _multiply(&r.x, &r.x, &t[1]);
    _multiply(&t[0], &t[0], &t[1]);
    _multiply(&r.y, &r.y, &t[0]);
    r.z = fp::one();
    return r;
}

g1 g1::add(const g1& e) const
{
    g1 r(*this);
    r.addAssign(e);
    return r;
}

void g1::addAssign(const g1& e) {
    // www.hyperelliptic.org/EFD/g1p/auto-shortw-jacobian-0.html#addition-add-2007-bl
    if(isZero())
    {
        *this = e;
        return;
    }
    if(e.isZero())
    {
        return;
    }
    fp t[9];
    _square(&t[7], &z);
    _multiply(&t[1], &e.x, &t[7]);
    _multiply(&t[2], &z, &t[7]);
    _multiply(&t[0], &e.y, &t[2]);
    _square(&t[8], &e.z);
    _multiply(&t[3], &x, &t[8]);
    _multiply(&t[4], &e.z, &t[8]);
    _multiply(&t[2], &y, &t[4]);
    if(t[1].equal(t[3]))
    {
        if(t[0].equal(t[2]))
        {
            doubleAssign();
            return;
        }
        *this = zero();
        return;
    }
    
    _subtract(&t[1], &t[1], &t[3]);
    _double(&t[4], &t[1]);
    _square(&t[4], &t[4]);
    _multiply(&t[5], &t[1], &t[4]);
    _subtract(&t[0], &t[0], &t[2]);
    _double(&t[0], &t[0]);
    _square(&t[6], &t[0]);
    _subtract(&t[6], &t[6], &t[5]);
    _multiply(&t[3], &t[3], &t[4]);
    _double(&t[4], &t[3]);
    _subtract(&x, &t[6], &t[4]);
    _subtract(&t[4], &t[3], &x);
    _multiply(&t[6], &t[2], &t[5]);
    _double(&t[6], &t[6]);
    _multiply(&t[0], &t[0], &t[4]);
    _subtract(&y, &t[0], &t[6]);
    _add(&t[0], &z, &e.z);
    _square(&t[0], &t[0]);
    _subtract(&t[0], &t[0], &t[7]);
    _subtract(&t[0], &t[0], &t[8]);
    _multiply(&z, &t[0], &t[1]);
}

g1 g1::dbl() const
{
    g1 r(*this);
    r.doubleAssign();
    return r;
}

void g1::doubleAssign() {
    // http://www.hyperelliptic.org/EFD/g1p/auto-shortw-jacobian-0.html#doubling-dbl-2009-l
    if(isZero())
    {
        return;
    }
    fp t[5];

    _square(&t[0], &x);
    _square(&t[1], &y);
    _square(&t[2], &t[1]);
    _add(&t[1], &x, &t[1]);
    _square(&t[1], &t[1]);
    _subtract(&t[1], &t[1], &t[0]);
    _subtract(&t[1], &t[1], &t[2]);
    _double(&t[1], &t[1]);
    _double(&t[3], &t[0]);
    _add(&t[0], &t[3], &t[0]);
    _square(&t[4], &t[0]);
    _double(&t[3], &t[1]);
    _subtract(&x, &t[4], &t[3]);
    _subtract(&t[1], &t[1], &x);
    _double(&t[2], &t[2]);
    _double(&t[2], &t[2]);
    _double(&t[2], &t[2]);
    _multiply(&t[0], &t[0], &t[1]);
    _subtract(&t[1], &t[0], &t[2]);
    _multiply(&t[0], &y, &z);
    y = t[1];
    _double(&z, &t[0]);
}

g1 g1::negate() const
{
    g1 r;
    r.x = x;
    r.z = z;
    _negate(&r.y, &y);
    return r;
}

g1 g1::subtract(const g1& e) const
{
    g1 c(*this);
    c.subtractAssign(e);
    return c;
}

void g1::subtractAssign(const g1& e) {
    addAssign(e.negate());
}

g1 g1::clearCofactor() const
{
    return this->scale(cofactorEFF);
}

g1 g1::glvEndomorphism() const
{
    if(this->isZero())
    {
        return zero();
    }
    g1 t = this->affine();
    t.x = t.x.phi();
    return t;
}

// Given pairs of G1 point and scalar values
// (P_0, e_0), (P_1, e_1), ... (P_n, e_n) calculates r = e_0 * P_0 + e_1 * P_1 + ... + e_n * P_n
// If length of points and scalars are not the same, then missing points will be treated as the zero point 
// and missing scalars will be treated as the zero scalar.
g1 g1::weightedSum(std::span<const g1> points, std::span<const std::array<uint64_t, 4>> scalars, const std::function<void()>& yield)
{
    const size_t effective_size = min(scalars.size(), points.size());
    uint64_t c = 3;
    if(effective_size >= 32)
    {
        c = (std::numeric_limits<size_t>::digits - std::countl_zero(effective_size))/3 + 2;
    }
    uint64_t bucketSize = (1<<c)-1;
    uint64_t windowsSize = 255/c+1;
    vector<g1> windows;
    windows.reserve(windowsSize);
    vector<g1> bucket;
    bucket.resize(bucketSize);
    for(uint64_t j = 0; j < windowsSize; j++)
    {
        for(uint64_t i = 0; i < bucketSize; i++)
        {
            if (yield && ((i & 255) == 0)) {
                yield();
            }
            bucket[i] = zero();
        }
        for(uint64_t i = 0; i < effective_size; i++)
        {
            if (yield && ((i & 255) == 0)) {
                yield();
            }
            array<uint64_t, 4> shifted;
            scalar::rsh(shifted, scalars[i], c*j);
            uint64_t index = bucketSize & shifted[0];
            if(index != 0)
            {
                bucket[index-1].addAssign(points[i]);
            }
        }
        g1 acc = zero();
        g1 sum = zero();
        for(int64_t i = bucketSize-1; i >= 0; i--)
        {
            if (yield && ((i & 255) == 0)) {
                yield();
            }
            sum.addAssign(bucket[i]);
            acc.addAssign(sum);
        }
        windows.push_back(acc);
    }

    g1 acc = zero();
    for(int64_t i = windows.size()-1; i >= 0; i--)
    {
        for(uint64_t j = 0; j < c; j++)
        {
            acc.doubleAssign();
        }
        acc.addAssign(windows[i]);
    }
    return acc;
}

// MapToCurve given a byte slice returns a valid G1 point.
// This mapping function implements the Simplified Shallue-van de Woestijne-Ulas method.
// https://tools.ietf.org/html/draft-irtf-cfrg-hash-to-curve-06
// Input byte slice should be a valid field element.
g1 g1::mapToCurve(const fp& e)
{
    fp x, y, z = fp::one();
    tie(x, y) = swuMapG1(e);
    isogenyMapG1(x, y);
    g1 p({x, y, z});
    p = p.clearCofactor();
    return p;
}

tuple<fp, fp> g1::swuMapG1(const fp& e)
{
    struct swuParamsForG1
    {
        fp z;
        fp zInv;
        fp a;
        fp b;
        fp minusBOverA;
    } params = {
        fp({0x886c00000023ffdc, 0x0f70008d3090001d, 0x77672417ed5828c3, 0x9dac23e943dc1740, 0x50553f1b9c131521, 0x078c712fbe0ab6e8}),
        fp({0x0e8a2e8ba2e83e10, 0x5b28ba2ca4d745d1, 0x678cd5473847377a, 0x4c506dd8a8076116, 0x9bcb227d79284139, 0x0e8d3154b0ba099a}),
        fp({0x2f65aa0e9af5aa51, 0x86464c2d1e8416c3, 0xb85ce591b7bd31e2, 0x27e11c91b5f24e7c, 0x28376eda6bfc1835, 0x155455c3e5071d85}),
        fp({0xfb996971fe22a1e0, 0x9aa93eb35b742d6f, 0x8c476013de99c5c4, 0x873e27c3a221e571, 0xca72b5e45a52d888, 0x06824061418a386b}),
        fp({0x052583c93555a7fe, 0x3b40d72430f93c82, 0x1b75faa0105ec983, 0x2527e7dc63851767, 0x99fffd1f34fc181d, 0x097cab54770ca0d3})
    };
    fp tv[4];
    _square(&tv[0], &e);
    _multiply(&tv[0], &tv[0], &params.z);
    _square(&tv[1], &tv[0]);
    fp x1;
    _add(&x1, &tv[0], &tv[1]);
    x1 = x1.inverse();
    bool e1 = x1.isZero();
    fp one = fp::one();
    _add(&x1, &x1, &one);
    if(e1)
    {
        x1 = params.zInv;
    }
    _multiply(&x1, &x1, &params.minusBOverA);
    fp gx1;
    _square(&gx1, &x1);
    _add(&gx1, &gx1, &params.a);
    _multiply(&gx1, &gx1, &x1);
    _add(&gx1, &gx1, &params.b);
    fp x2;
    _multiply(&x2, &tv[0], &x1);
    _multiply(&tv[1], &tv[0], &tv[1]);
    fp gx2;
    _multiply(&gx2, &gx1, &tv[1]);
    bool e2 = !gx1.isQuadraticNonResidue();
    fp x, y2;
    if(e2)
    {
        x = x1;
        y2 = gx1;
    }
    else
    {
        x = x2;
        y2 = gx2;
    }
    fp y;
    y2.sqrt(y);
    if(y.sign() != e.sign())
    {
        _negate(&y, &y);
    }
    return {x, y};
}

void g1::isogenyMapG1(fp& x, fp& y)
{
    // https://tools.ietf.org/html/draft-irtf-cfrg-hash-to-curve-06#appendix-C.2
    fp isogenyConstantsG1[4][16] = {
        {
            fp({0x4d18b6f3af00131c, 0x19fa219793fee28c, 0x3f2885f1467f19ae, 0x23dcea34f2ffb304, 0xd15b58d2ffc00054, 0x0913be200a20bef4}),
            fp({0x898985385cdbbd8b, 0x3c79e43cc7d966aa, 0x1597e193f4cd233a, 0x8637ef1e4d6623ad, 0x11b22deed20d827b, 0x07097bc5998784ad}),
            fp({0xa542583a480b664b, 0xfc7169c026e568c6, 0x5ba2ef314ed8b5a6, 0x5b5491c05102f0e7, 0xdf6e99707d2a0079, 0x0784151ed7605524}),
            fp({0x494e212870f72741, 0xab9be52fbda43021, 0x26f5577994e34c3d, 0x049dfee82aefbd60, 0x65dadd7828505289, 0x0e93d431ea011aeb}),
            fp({0x90ee774bd6a74d45, 0x7ada1c8a41bfb185, 0x0f1a8953b325f464, 0x104c24211be4805c, 0x169139d319ea7a8f, 0x09f20ead8e532bf6}),
            fp({0x6ddd93e2f43626b7, 0xa5482c9aa1ccd7bd, 0x143245631883f4bd, 0x2e0a94ccf77ec0db, 0xb0282d480e56489f, 0x18f4bfcbb4368929}),
            fp({0x23c5f0c953402dfd, 0x7a43ff6958ce4fe9, 0x2c390d3d2da5df63, 0xd0df5c98e1f9d70f, 0xffd89869a572b297, 0x1277ffc72f25e8fe}),
            fp({0x79f4f0490f06a8a6, 0x85f894a88030fd81, 0x12da3054b18b6410, 0xe2a57f6505880d65, 0xbba074f260e400f1, 0x08b76279f621d028}),
            fp({0xe67245ba78d5b00b, 0x8456ba9a1f186475, 0x7888bff6e6b33bb4, 0xe21585b9a30f86cb, 0x05a69cdcef55feee, 0x09e699dd9adfa5ac}),
            fp({0x0de5c357bff57107, 0x0a0db4ae6b1a10b2, 0xe256bb67b3b3cd8d, 0x8ad456574e9db24f, 0x0443915f50fd4179, 0x098c4bf7de8b6375}),
            fp({0xe6b0617e7dd929c7, 0xfe6e37d442537375, 0x1dafdeda137a489e, 0xe4efd1ad3f767ceb, 0x4a51d8667f0fe1cf, 0x054fdf4bbf1d821c}),
            fp({0x72db2a50658d767b, 0x8abf91faa257b3d5, 0xe969d6833764ab47, 0x464170142a1009eb, 0xb14f01aadb30be2f, 0x18ae6a856f40715d}),
            fp({0, 0, 0, 0, 0, 0}),
            fp({0, 0, 0, 0, 0, 0}),
            fp({0, 0, 0, 0, 0, 0}),
            fp({0, 0, 0, 0, 0, 0}),
        },
        {
            fp({0xb962a077fdb0f945, 0xa6a9740fefda13a0, 0xc14d568c3ed6c544, 0xb43fc37b908b133e, 0x9c0b3ac929599016, 0x0165aa6c93ad115f}),
            fp({0x23279a3ba506c1d9, 0x92cfca0a9465176a, 0x3b294ab13755f0ff, 0x116dda1c5070ae93, 0xed4530924cec2045, 0x083383d6ed81f1ce}),
            fp({0x9885c2a6449fecfc, 0x4a2b54ccd37733f0, 0x17da9ffd8738c142, 0xa0fba72732b3fafd, 0xff364f36e54b6812, 0x0f29c13c660523e2}),
            fp({0xe349cc118278f041, 0xd487228f2f3204fb, 0xc9d325849ade5150, 0x43a92bd69c15c2df, 0x1c2c7844bc417be4, 0x12025184f407440c}),
            fp({0x587f65ae6acb057b, 0x1444ef325140201f, 0xfbf995e71270da49, 0xccda066072436a42, 0x7408904f0f186bb2, 0x13b93c63edf6c015}),
            fp({0xfb918622cd141920, 0x4a4c64423ecaddb4, 0x0beb232927f7fb26, 0x30f94df6f83a3dc2, 0xaeedd424d780f388, 0x06cc402dd594bbeb}),
            fp({0xd41f761151b23f8f, 0x32a92465435719b3, 0x64f436e888c62cb9, 0xdf70a9a1f757c6e4, 0x6933a38d5b594c81, 0x0c6f7f7237b46606}),
            fp({0x693c08747876c8f7, 0x22c9850bf9cf80f0, 0x8e9071dab950c124, 0x89bc62d61c7baf23, 0xbc6be2d8dad57c23, 0x17916987aa14a122}),
            fp({0x1be3ff439c1316fd, 0x9965243a7571dfa7, 0xc7f7f62962f5cd81, 0x32c6aa9af394361c, 0xbbc2ee18e1c227f4, 0x0c102cbac531bb34}),
            fp({0x997614c97bacbf07, 0x61f86372b99192c0, 0x5b8c95fc14353fc3, 0xca2b066c2a87492f, 0x16178f5bbf698711, 0x12a6dcd7f0f4e0e8}),
            fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
            fp({0, 0, 0, 0, 0, 0}),
            fp({0, 0, 0, 0, 0, 0}),
            fp({0, 0, 0, 0, 0, 0}),
            fp({0, 0, 0, 0, 0, 0}),
            fp({0, 0, 0, 0, 0, 0}),
        },
        {
            fp({0x2b567ff3e2837267, 0x1d4d9e57b958a767, 0xce028fea04bd7373, 0xcc31a30a0b6cd3df, 0x7d7b18a682692693, 0x0d300744d42a0310}),
            fp({0x99c2555fa542493f, 0xfe7f53cc4874f878, 0x5df0608b8f97608a, 0x14e03832052b49c8, 0x706326a6957dd5a4, 0x0a8dadd9c2414555}),
            fp({0x13d942922a5cf63a, 0x357e33e36e261e7d, 0xcf05a27c8456088d, 0x0000bd1de7ba50f0, 0x83d0c7532f8c1fde, 0x13f70bf38bbf2905}),
            fp({0x5c57fd95bfafbdbb, 0x28a359a65e541707, 0x3983ceb4f6360b6d, 0xafe19ff6f97e6d53, 0xb3468f4550192bf7, 0x0bb6cde49d8ba257}),
            fp({0x590b62c7ff8a513f, 0x314b4ce372cacefd, 0x6bef32ce94b8a800, 0x6ddf84a095713d5f, 0x64eace4cb0982191, 0x0386213c651b888d}),
            fp({0xa5310a31111bbcdd, 0xa14ac0f5da148982, 0xf9ad9cc95423d2e9, 0xaa6ec095283ee4a7, 0xcf5b1f022e1c9107, 0x01fddf5aed881793}),
            fp({0x65a572b0d7a7d950, 0xe25c2d8183473a19, 0xc2fcebe7cb877dbd, 0x05b2d36c769a89b0, 0xba12961be86e9efb, 0x07eb1b29c1dfde1f}),
            fp({0x93e09572f7c4cd24, 0x364e929076795091, 0x8569467e68af51b5, 0xa47da89439f5340f, 0xf4fa918082e44d64, 0x0ad52ba3e6695a79}),
            fp({0x911429844e0d5f54, 0xd03f51a3516bb233, 0x3d587e5640536e66, 0xfa86d2a3a9a73482, 0xa90ed5adf1ed5537, 0x149c9c326a5e7393}),
            fp({0x462bbeb03c12921a, 0xdc9af5fa0a274a17, 0x9a558ebde836ebed, 0x649ef8f11a4fae46, 0x8100e1652b3cdc62, 0x1862bd62c291dacb}),
            fp({0x05c9b8ca89f12c26, 0x0194160fa9b9ac4f, 0x6a643d5a6879fa2c, 0x14665bdd8846e19d, 0xbb1d0d53af3ff6bf, 0x12c7e1c3b28962e5}),
            fp({0xb55ebf900b8a3e17, 0xfedc77ec1a9201c4, 0x1f07db10ea1a4df4, 0x0dfbd15dc41a594d, 0x389547f2334a5391, 0x02419f98165871a4}),
            fp({0xb416af000745fc20, 0x8e563e9d1ea6d0f5, 0x7c763e17763a0652, 0x01458ef0159ebbef, 0x8346fe421f96bb13, 0x0d2d7b829ce324d2}),
            fp({0x93096bb538d64615, 0x6f2a2619951d823a, 0x8f66b3ea59514fa4, 0xf563e63704f7092f, 0x724b136c4cf2d9fa, 0x046959cfcfd0bf49}),
            fp({0xea748d4b6e405346, 0x91e9079c2c02d58f, 0x41064965946d9b59, 0xa06731f1d2bbe1ee, 0x07f897e267a33f1b, 0x1017290919210e5f}),
            fp({0x872aa6c17d985097, 0xeecc53161264562a, 0x07afe37afff55002, 0x54759078e5be6838, 0xc4b92d15db8acca8, 0x106d87d1b51d13b9}),
        },
        {
            fp({0xeb6c359d47e52b1c, 0x18ef5f8a10634d60, 0xddfa71a0889d5b7e, 0x723e71dcc5fc1323, 0x52f45700b70d5c69, 0x0a8b981ee47691f1}),
            fp({0x616a3c4f5535b9fb, 0x6f5f037395dbd911, 0xf25f4cc5e35c65da, 0x3e50dffea3c62658, 0x6a33dca523560776, 0x0fadeff77b6bfe3e}),
            fp({0x2be9b66df470059c, 0x24a2c159a3d36742, 0x115dbe7ad10c2a37, 0xb6634a652ee5884d, 0x04fe8bb2b8d81af4, 0x01c2a7a256fe9c41}),
            fp({0xf27bf8ef3b75a386, 0x898b367476c9073f, 0x24482e6b8c2f4e5f, 0xc8e0bbd6fe110806, 0x59b0c17f7631448a, 0x11037cd58b3dbfbd}),
            fp({0x31c7912ea267eec6, 0x1dbf6f1c5fcdb700, 0xd30d4fe3ba86fdb1, 0x3cae528fbee9a2a4, 0xb1cce69b6aa9ad9a, 0x044393bb632d94fb}),
            fp({0xc66ef6efeeb5c7e8, 0x9824c289dd72bb55, 0x71b1a4d2f119981d, 0x104fc1aafb0919cc, 0x0e49df01d942a628, 0x096c3a09773272d4}),
            fp({0x9abc11eb5fadeff4, 0x32dca50a885728f0, 0xfb1fa3721569734c, 0xc4b76271ea6506b3, 0xd466a75599ce728e, 0x0c81d4645f4cb6ed}),
            fp({0x4199f10e5b8be45b, 0xda64e495b1e87930, 0xcb353efe9b33e4ff, 0x9e9efb24aa6424c6, 0xf08d33680a237465, 0x0d3378023e4c7406}),
            fp({0x7eb4ae92ec74d3a5, 0xc341b4aa9fac3497, 0x5be603899e907687, 0x03bfd9cca75cbdeb, 0x564c2935a96bfa93, 0x0ef3c33371e2fdb5}),
            fp({0x7ee91fd449f6ac2e, 0xe5d5bd5cb9357a30, 0x773a8ca5196b1380, 0xd0fda172174ed023, 0x6cb95e0fa776aead, 0x0d22d5a40cec7cff}),
            fp({0xf727e09285fd8519, 0xdc9d55a83017897b, 0x7549d8bd057894ae, 0x178419613d90d8f8, 0xfce95ebdeb5b490a, 0x0467ffaef23fc49e}),
            fp({0xc1769e6a7c385f1b, 0x79bc930deac01c03, 0x5461c75a23ede3b5, 0x6e20829e5c230c45, 0x828e0f1e772a53cd, 0x116aefa749127bff}),
            fp({0x101c10bf2744c10a, 0xbbf18d053a6a3154, 0xa0ecf39ef026f602, 0xfc009d4996dc5153, 0xb9000209d5bd08d3, 0x189e5fe4470cd73c}),
            fp({0x7ebd546ca1575ed2, 0xe47d5a981d081b55, 0x57b2b625b6d4ca21, 0xb0a1ba04228520cc, 0x98738983c2107ff3, 0x13dddbc4799d81d6}),
            fp({0x09319f2e39834935, 0x039e952cbdb05c21, 0x55ba77a9a2f76493, 0xfd04e3dfc6086467, 0xfb95832e7d78742e, 0x0ef9c24eccaf5e0e}),
            fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
        }
    };
    fp (*params)[16] = isogenyConstantsG1;
    int64_t degree = 15;
    fp xNum, xDen, yNum, yDen;
    xNum = params[0][degree];
    xDen = params[1][degree];
    yNum = params[2][degree];
    yDen = params[3][degree];
    for(int64_t i = degree - 1; i >= 0; i--)
    {
        _multiply(&xNum, &xNum, &x);
        _multiply(&xDen, &xDen, &x);
        _multiply(&yNum, &yNum, &x);
        _multiply(&yDen, &yDen, &x);
        _add(&xNum, &xNum, &params[0][i]);
        _add(&xDen, &xDen, &params[1][i]);
        _add(&yNum, &yNum, &params[2][i]);
        _add(&yDen, &yDen, &params[3][i]);
    }
    xDen = xDen.inverse();
    yDen = yDen.inverse();
    _multiply(&xNum, &xNum, &xDen);
    _multiply(&yNum, &yNum, &yDen);
    _multiply(&yNum, &yNum, &y);
    x = xNum;
    y = yNum;
}

const g1 g1::BASE = g1({
    fp({0x5cb38790fd530c16, 0x7817fc679976fff5, 0x154f95c7143ba1c1, 0xf0ae6acdf3d0e747, 0xedce6ecc21dbf440, 0x120177419e0bfb75}),
    fp({0xbaac93d50ce72271, 0x8c22631a7918fd8e, 0xdd595f13570725ce, 0x51ac582950405194, 0x0e1c8c3fad0059c0, 0x0bbc3efc5008a26a}),
    fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
});

const array<uint64_t, 1> g1::cofactorEFF = {0xd201000000010001};

g2::g2() : x(fp2()), y(fp2()), z(fp2())
{
}

g2::g2(const array<fp2, 3>& e3) : x(e3[0]), y(e3[1]), z(e3[2])
{
}

g2::g2(const g2& e) : x(e.x), y(e.y), z(e.z)
{
}

optional<g2> g2::fromJacobianBytesBE(const span<const uint8_t, 288> in, conv_opt opt)
{
    // We decided to always validate the input here. Check flag will only affect on-curve checks.
    bool curve_check = opt.check_valid;
    opt.check_valid = true;
    optional<fp2> x = fp2::fromBytesBE(span<const uint8_t, 96>(&in[  0], 96), opt);
    optional<fp2> y = fp2::fromBytesBE(span<const uint8_t, 96>(&in[ 96], 96), opt);
    optional<fp2> z = fp2::fromBytesBE(span<const uint8_t, 96>(&in[192], 96), opt);
    if(!x || !y || !z) return {};
    g2 p = g2({*x, *y, *z});
    if(curve_check && !p.isOnCurve())
    {
        return {};
    }
    return p;
}

optional<g2> g2::fromJacobianBytesLE(const span<const uint8_t, 288> in, conv_opt opt)
{
    // We decided to always validate the input here. Check flag will only affect on-curve checks.
    bool curve_check = opt.check_valid;
    opt.check_valid = true;
    optional<fp2> x = fp2::fromBytesLE(span<const uint8_t, 96>(&in[  0], 96), opt);
    optional<fp2> y = fp2::fromBytesLE(span<const uint8_t, 96>(&in[ 96], 96), opt);
    optional<fp2> z = fp2::fromBytesLE(span<const uint8_t, 96>(&in[192], 96), opt);
    if(!x || !y || !z) return {};
    g2 p = g2({*x, *y, *z});
    if(curve_check && !p.isOnCurve())
    {
        return {};
    }
    return p;
}

optional<g2> g2::fromAffineBytesBE(const span<const uint8_t, 192> in, conv_opt opt)
{
    // We decided to always validate the input here. Check flag will only affect on-curve checks.
    bool curve_check = opt.check_valid;
    opt.check_valid = true;
    optional<fp2> x = fp2::fromBytesBE(span<const uint8_t, 96>(&in[  0], 96), opt);
    optional<fp2> y = fp2::fromBytesBE(span<const uint8_t, 96>(&in[ 96], 96), opt);
    if(!x || !y) return {};
    // check if given input points to infinity
    if(x->isZero() && y->isZero())
    {
        return zero();
    }
    fp2 z = fp2::one();
    g2 p = g2({*x, *y, z});
    if(curve_check && !p.isOnCurve())
    {
        return {};
    }
    return p;
}

optional<g2> g2::fromAffineBytesLE(const span<const uint8_t, 192> in, conv_opt opt)
{
    // We decided to always validate the input here. Check flag will only affect on-curve checks.
    bool curve_check = opt.check_valid;
    opt.check_valid = true;
    optional<fp2> x = fp2::fromBytesLE(span<const uint8_t, 96>(&in[  0], 96), opt);
    optional<fp2> y = fp2::fromBytesLE(span<const uint8_t, 96>(&in[ 96], 96), opt);
    if(!x || !y) return {};
    // check if given input points to infinity
    if(x->isZero() && y->isZero())
    {
        return zero();
    }
    fp2 z = fp2::one();
    g2 p = g2({*x, *y, z});
    if(curve_check && !p.isOnCurve())
    {
        return {};
    }
    return p;
}

optional<g2> g2::fromCompressedBytesBE(const span<const uint8_t, 96> in)
{
    // check compression bit
    if(((in[0] >> 7) & 1) != 1)
    {
        return {};
    }
    // check if point is infinity
    if(((in[0] >> 6) & 1) == 1)
    {
        return zero();
    }
    // reconstruct point from x coordinate
    bool ysign = ((in[0] >> 5) & 1) == 1;
    g2 p;
    scalar::fromBytesBE(span<const uint8_t, 48>(&in[0], 48), p.x.c1.d);
    auto c0 = fp::fromBytesBE(span<const uint8_t, 48>(&in[48], 48));
    if (!c0) {
        return {};
    }
    p.x.c0 = *c0;
    // erase 3 msbs from given input and perform validity check
    p.x.c1.d[5] &= 0x1FFFFFFFFFFFFFFF;
    p.x.c1 = p.x.c1.toMont();
    if(!p.x.c1.isValid())
    {
        return {};
    }
    // BLS 12-381 curve equation:
    //      y^2 = x^3 + B
    //  =>  y   = +/- sqrt(x^3 + B)
    fp2 b = fp2::B;
    p.y = p.x.square();         // y = x^2
    p.y = p.y.multiply(p.x);         // y = x^2 * x = x^3
    p.y = p.y.add(b);           // y = x^3 + B
    if(!p.y.sqrt(p.y))
    {
        return {};
    }
    if(p.y.isLexicographicallyLargest() ^ ysign)
    {
        p.y = p.y.negate();
    }
    p.z = fp2::one();
    return p;
}

void g2::toJacobianBytesBE(const span<uint8_t, 288> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[  0], &x.toBytesBE(fm)[0], 96);
    memcpy(&out[ 96], &y.toBytesBE(fm)[0], 96);
    memcpy(&out[192], &z.toBytesBE(fm)[0], 96);
}

void g2::toJacobianBytesLE(const span<uint8_t, 288> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[  0], &x.toBytesLE(fm)[0], 96);
    memcpy(&out[ 96], &y.toBytesLE(fm)[0], 96);
    memcpy(&out[192], &z.toBytesLE(fm)[0], 96);
}

void g2::toAffineBytesBE(const span<uint8_t, 192> out, const from_mont fm /* = from_mont::yes */) const
{
    if(isZero())
    {
        memset(&out[0], 0, 192);
        return;
    }
    g2 r = affine();
    memcpy(&out[ 0], &r.x.toBytesBE(fm)[0], 96);
    memcpy(&out[96], &r.y.toBytesBE(fm)[0], 96);
}

void g2::toAffineBytesLE(const span<uint8_t, 192> out, const from_mont fm /* = from_mont::yes */) const
{
    if(isZero())
    {
        memset(&out[0], 0, 192);
        return;
    }
    g2 r = affine();
    memcpy(&out[ 0], &r.x.toBytesLE(fm)[0], 96);
    memcpy(&out[96], &r.y.toBytesLE(fm)[0], 96);
}

void g2::toCompressedBytesBE(const span<uint8_t, 96> out) const
{
    // check: https://github.com/zcash/librustzcash/blob/6e0364cd42a2b3d2b958a54771ef51a8db79dd29/pairing/src/bls12_381/README.md#serialization
    g2 p = affine();
    // check if point is at infinity
    if(p.x.isZero() && p.y.isZero())
    {
        memset(out.data(), 0, 96);
        out[0] |= 0xC0;
        return;
    }
    memcpy(out.data(), p.x.c1.toBytesBE().data(), 48);
    memcpy(out.data() + 48, p.x.c0.toBytesBE().data(), 48);
    // check y component
    if(p.y.isLexicographicallyLargest())
    {
        out[0] |= 0x20;
    }
    // set compression bit
    out[0] |= 0x80;
}

array<uint8_t, 288> g2::toJacobianBytesBE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 288> out;
    toJacobianBytesBE(out, fm);
    return out;
}

array<uint8_t, 288> g2::toJacobianBytesLE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 288> out;
    toJacobianBytesLE(out, fm);
    return out;
}

array<uint8_t, 192> g2::toAffineBytesBE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 192> out;
    toAffineBytesBE(out, fm);
    return out;
}

array<uint8_t, 192> g2::toAffineBytesLE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 192> out;
    toAffineBytesLE(out, fm);
    return out;
}

array<uint8_t, 96> g2::toCompressedBytesBE() const
{
    array<uint8_t, 96> out;
    toCompressedBytesBE(out);
    return out;
}

g2 g2::zero()
{
    return g2({
        fp2::zero(),
        fp2::one(),
        fp2::zero()
    });
}

g2 g2::one()
{
    return BASE;
}

bool g2::isZero() const
{
    return z.isZero();
}

bool g2::equal(const g2& e) const
{
    if(isZero())
    {
        return e.isZero();
    }
    if(e.isZero())
    {
        return isZero();
    }
    fp2 t[9];
    t[0] = z.square();
    t[1] = e.z.square();
    t[2] = t[0].multiply(e.x);
    t[3] = t[1].multiply(x);
    t[0] = t[0].multiply(z);
    t[1] = t[1].multiply(e.z);
    t[1] = t[1].multiply(y);
    t[0] = t[0].multiply(e.y);
    return t[0].equal(t[1]) && t[2].equal(t[3]);
}

bool g2::inCorrectSubgroup() const
{
    // Faster Subgroup Checks for BLS12-381
    // S. Bowe (https://eprint.iacr.org/2019/814.pdf)
    // [z]ψ^3(P) − ψ^2(P) + P = O
    g2 t0, t1;
    t0 = this->psi();
    t0 = t0.psi();
    t1 = t0.negate();               // - ψ^2(P)
    t0 = t0.psi();                  // ψ^3(P)
    t0 = t0.scale(cofactorEFF);     // - x ψ^3(P)
    t0 = t0.negate();

    t0 = t0.add(t1);
    t0 = t0.add(*this);

    return t0.isZero();
}

bool g2::isOnCurve() const
{
    if(isZero())
    {
        return true;
    }
    fp2 t[9];
    t[0] = y.square();
    t[1] = x.square();
    t[1] = t[1].multiply(x);
    t[2] = z.square();
    t[3] = t[2].square();
    t[2] = t[2].multiply(t[3]);
    t[2] = fp2::B.multiply(t[2]);
    t[1] = t[1].add(t[2]);
    return t[0].equal(t[1]);
}

bool g2::isAffine() const
{
    return z.isOne();
}

g2 g2::affine() const
{
    if(isZero() || isAffine())
    {
        return *this;
    }
    g2 r = *this;
    fp2 t[9];
    t[0] = r.z.inverse();
    t[1] = t[0].square();
    r.x = r.x.multiply(t[1]);
    t[0] = t[0].multiply(t[1]);
    r.y = r.y.multiply(t[0]);
    r.z = fp2::one();
    return r;
}

g2 g2::add(const g2& e) const
{
    g2 r(*this);
    r.addAssign(e);
    return r;
}

void g2::addAssign(const g2& e) {
    // http://www.hyperelliptic.org/EFD/g1p/auto-shortw-jacobian-0.html#addition-add-2007-bl
    if(isZero())
    {
        *this = e;
        return;
    }
    if(e.isZero())
    {
        return;
    }
    fp2 t[9];
    t[7] = z.square();
    t[1] = e.x.multiply(t[7]);
    t[2] = z.multiply(t[7]);
    t[0] = e.y.multiply(t[2]);
    t[8] = e.z.square();
    t[3] = x.multiply(t[8]);
    t[4] = e.z.multiply(t[8]);
    t[2] = y.multiply(t[4]);
    if(t[1].equal(t[3]))
    {
        if(t[0].equal(t[2]))
        {
            doubleAssign();
            return;
        }
        *this = zero();
        return;
    }

    t[1] = t[1].subtract(t[3]);
    t[4] = t[1].dbl();
    t[4] = t[4].square();
    t[5] = t[1].multiply(t[4]);
    t[0] = t[0].subtract(t[2]);
    t[0] = t[0].dbl();
    t[6] = t[0].square();
    t[6] = t[6].subtract(t[5]);
    t[3] = t[3].multiply(t[4]);
    t[4] = t[3].dbl();
    x = t[6].subtract(t[4]);
    t[4] = t[3].subtract(x);
    t[6] = t[2].multiply(t[5]);
    t[6] = t[6].dbl();
    t[0] = t[0].multiply(t[4]);
    y = t[0].subtract(t[6]);
    t[0] = z.add(e.z);
    t[0] = t[0].square();
    t[0] = t[0].subtract(t[7]);
    t[0] = t[0].subtract(t[8]);
    z = t[0].multiply(t[1]);
}

g2 g2::dbl() const
{
    g2 r(*this);
    r.doubleAssign();
    return r;
}

void g2::doubleAssign() {
    // http://www.hyperelliptic.org/EFD/g1p/auto-shortw-jacobian-0.html#doubling-dbl-2009-l
    if(isZero())
    {
        return;
    }
    fp2 t[9];

    t[0] = x.square();
    t[1] = y.square();
    t[2] = t[1].square();
    t[1] = x.add(t[1]);
    t[1] = t[1].square();
    t[1] = t[1].subtract(t[0]);
    t[1] = t[1].subtract(t[2]);
    t[1] = t[1].dbl();
    t[3] = t[0].dbl();
    t[0] = t[3].add(t[0]);
    t[4] = t[0].square();
    t[3] = t[1].dbl();
    x = t[4].subtract(t[3]);
    t[1] = t[1].subtract(x);
    t[2] = t[2].dbl();
    t[2] = t[2].dbl();
    t[2] = t[2].dbl();
    t[0] = t[0].multiply(t[1]);
    t[1] = t[0].subtract(t[2]);
    t[0] = y.multiply(z);
    y = t[1];
    z = t[0].dbl();
}

g2 g2::negate() const
{
    g2 r;
    r.x = x;
    r.y = y.negate();
    r.z = z;
    return r;
}

g2 g2::subtract(const g2& e) const
{
    g2 c(*this);
    c.subtractAssign(e);
    return c;
}

void g2::subtractAssign(const g2& e) {
    addAssign(e.negate());
}

g2 g2::psi() const
{
    g2 p;
    p.x = this->x.conjugate();
    p.y = this->y.conjugate();
    p.z = this->z.conjugate();
    p.x = p.x.multiply(fp2::psiX);
    p.y = p.y.multiply(fp2::psiY);
    return p;
}

g2 g2::clearCofactor() const
{
    g2 t0, t1, t2, t3;
    // Compute t0 = xP
    t0 = scale(cofactorEFF);
    // cofactorEFF has the MSB set, so relic has the sign bit set and negates the y coordinate
    t0 = t0.negate();
    // Compute t1 = [x^2]P
    t1 = t0.scale(cofactorEFF);
    // cofactorEFF has the MSB set, so relic has the sign bit set and negates the y coordinate
    t1 = t1.negate();

    // t2 = (x^2 - x - 1)P = x^2P - x*P - P
    t2 = t1.subtract(t0);
    t2 = t2.subtract(*this);
    // t3 = \psi(x - 1)P
    t3 = t0.subtract(*this);
    t3 = t3.frobeniusMap(1);
    t2 = t2.add(t3);
    // t3 = \psi^2(2P)
    t3 = this->dbl();
    t3 = t3.frobeniusMap(2);
    t2 = t2.add(t3);
    return t2;
}

g2 g2::frobeniusMap(int64_t power) const
{
    fp2 frb0 = fp2({
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
        fp({0x890dc9e4867545c3, 0x2af322533285a5d5, 0x50880866309b7e2c, 0xa20d1b8c7e881024, 0x14e4f04fe2db9068, 0x14e56d3f1564853a}),
    });
    fp2 frb1 = fp2({
        fp({0x3e2f585da55c9ad1, 0x4294213d86c18183, 0x382844c88b623732, 0x92ad2afd19103e18, 0x1d794e4fac7cf0b9, 0x0bd592fc7d825ec8}),
        fp({0x7bcfa7a25aa30fda, 0xdc17dec12a927e7c, 0x2f088dd86b4ebef1, 0xd1ca2087da74d4a7, 0x2da2596696cebc1d, 0x0e2b7eedbbfd87d2}),
    });
    g2 r = *this;
    for(; power > 0; power--)
    {
        r.x.frobeniusMapAssign(1);
        r.y.frobeniusMapAssign(1);
        r.z.frobeniusMapAssign(1);
        r.x = r.x.multiply(frb0);
        r.y = r.y.multiply(frb1);
    }
    return r;
}

// Given pairs of G2 point and scalar values
// (P_0, e_0), (P_1, e_1), ... (P_n, e_n) calculates r = e_0 * P_0 + e_1 * P_1 + ... + e_n * P_n
// If length of points and scalars are not the same, then missing points will be treated as the zero point 
// and missing scalars will be treated as the zero scalar.
g2 g2::weightedSum(std::span<const g2> points, std::span<const std::array<uint64_t, 4>> scalars, const std::function<void()>& yield)
{
    const size_t effective_size = min(scalars.size(), points.size());
    uint64_t c = 3;
    if(effective_size >= 32)
    {
        c = (std::numeric_limits<size_t>::digits - std::countl_zero(effective_size))/3 + 2;
    }
    uint64_t bucketSize = (1<<c)-1;
    uint64_t windowsSize = 255/c+1;
    vector<g2> windows;
    windows.reserve(windowsSize);
    vector<g2> bucket;
    bucket.resize(bucketSize);
    for(uint64_t j = 0; j < windowsSize; j++)
    {
        for(uint64_t i = 0; i < bucketSize; i++)
        {
            if (yield && ((i & 255) == 0)) {
                yield();
            }
            bucket[i] = zero();
        }
        for(uint64_t i = 0; i < effective_size; i++)
        {
            if (yield && ((i & 255) == 0)) {
                yield();
            }
            array<uint64_t, 4> shifted;
            scalar::rsh(shifted, scalars[i], c*j);
            uint64_t index = bucketSize & shifted[0];
            if(index != 0)
            {
                bucket[index-1].addAssign(points[i]);
            }
        }
        g2 acc = zero();
        g2 sum = zero();
        for(int64_t i = bucketSize-1; i >= 0; i--)
        {
            if (yield && ((i & 255) == 0)) {
                yield();
            }
            sum.addAssign(bucket[i]);
            acc.addAssign(sum);
        }
        windows.push_back(acc);
    }

    g2 acc = zero();
    for(int64_t i = windows.size()-1; i >= 0; i--)
    {
        for(uint64_t j = 0; j < c; j++)
        {
            acc.doubleAssign();
        }
        acc.addAssign(windows[i]);
    }
    return acc;
}

// MapToCurve given a byte slice returns a valid G2 point.
// This mapping function implements the Simplified Shallue-van de Woestijne-Ulas method.
// https://tools.ietf.org/html/draft-irtf-cfrg-hash-to-curve-05#section-6.6.2
// Input byte slice should be a valid field element.
g2 g2::mapToCurve(const fp2& e)
{
    fp2 x, y, z = fp2::one();
    tie(x, y) = swuMapG2(e);
    g2 p({x, y, z});
    p = p.isogenyMap();
    p = p.clearCofactor();
    return p;
}

tuple<fp2, fp2> g2::swuMapG2(const fp2& e)
{
    struct swuParamsForG2
    {
        fp2 z;
        fp2 zInv;
        fp2 a;
        fp2 b;
        fp2 minusBOverA;
    } params = {
        fp2({
            fp({0x87ebfffffff9555c, 0x656fffe5da8ffffa, 0x0fd0749345d33ad2, 0xd951e663066576f4, 0xde291a3d41e980d3, 0x0815664c7dfe040d}),
            fp({0x43f5fffffffcaaae, 0x32b7fff2ed47fffd, 0x07e83a49a2e99d69, 0xeca8f3318332bb7a, 0xef148d1ea0f4c069, 0x040ab3263eff0206}),
        }),
        fp2({
            fp({0xacd0000000011110, 0x9dd9999dc88ccccd, 0xb5ca2ac9b76352bf, 0xf1b574bcf4bc90ce, 0x42dab41f28a77081, 0x132fc6ac14cd1e12}),
            fp({0xe396ffffffff2223, 0x4fbf332fcd0d9998, 0x0c4bbd3c1aff4cc4, 0x6b9c91267926ca58, 0x29ae4da6aef7f496, 0x10692e942f195791}),
        }),
        fp2({
            fp({0, 0, 0, 0, 0, 0}),
            fp({0xe53a000003135242, 0x01080c0fdef80285, 0xe7889edbe340f6bd, 0x0b51375126310601, 0x02d6985717c744ab, 0x1220b4e979ea5467}),
        }),
        fp2({
            fp({0x22ea00000cf89db2, 0x6ec832df71380aa4, 0x6e1b94403db5a66e, 0x75bf3c53a79473ba, 0x3dd3a569412c0a34, 0x125cdb5e74dc4fd1}),
            fp({0x22ea00000cf89db2, 0x6ec832df71380aa4, 0x6e1b94403db5a66e, 0x75bf3c53a79473ba, 0x3dd3a569412c0a34, 0x125cdb5e74dc4fd1}),
        }),
        fp2({
            fp({0x903c555555474fb3, 0x5f98cc95ce451105, 0x9f8e582eefe0fade, 0xc68946b6aebbd062, 0x467a4ad10ee6de53, 0x0e7146f483e23a05}),
            fp({0x29c2aaaaaab85af8, 0xbf133368e30eeefa, 0xc7a27a7206cffb45, 0x9dee04ce44c9425c, 0x04a15ce53464ce83, 0x0b8fcaf5b59dac95}),
        }),
    };
    fp2 tv[4];
    tv[0] = e.square();
    tv[0] = tv[0].multiply(params.z);
    tv[1] = tv[0].square();
    fp2 x1;
    x1 = tv[0].add(tv[1]);
    x1 = x1.inverse();
    bool e1 = x1.isZero();
    x1 = x1.add(fp2::one());
    if(e1)
    {
        x1 = params.zInv;
    }
    x1 = x1.multiply(params.minusBOverA);
    fp2 gx1;
    gx1 = x1.square();
    gx1 = gx1.add(params.a);
    gx1 = gx1.multiply(x1);
    gx1 = gx1.add(params.b);
    fp2 x2;
    x2 = tv[0].multiply(x1);
    tv[1] = tv[0].multiply(tv[1]);
    fp2 gx2;
    gx2 = gx1.multiply(tv[1]);
    bool e2 = !gx1.isQuadraticNonResidue();
    fp2 x, y2;
    if(e2)
    {
        x = x1;
        y2 = gx1;
    }
    else
    {
        x = x2;
        y2 = gx2;
    }
    fp2 y;
    y2.sqrt(y);
    if(y.sign() != e.sign())
    {
        y = y.negate();
    }
    return {x, y};
}
/*
void g2::isogenyMapG2(fp2& x, fp2& y)
{
    // https://tools.ietf.org/html/draft-irtf-cfrg-hash-to-curve-06#appendix-C.2
    fp2 isogenyConstantsG2[4][4] = {
        {
            fp2({
                fp({0x47f671c71ce05e62, 0x06dd57071206393e, 0x7c80cd2af3fd71a2, 0x048103ea9e6cd062, 0xc54516acc8d037f6, 0x13808f550920ea41}),
                fp({0x47f671c71ce05e62, 0x06dd57071206393e, 0x7c80cd2af3fd71a2, 0x048103ea9e6cd062, 0xc54516acc8d037f6, 0x13808f550920ea41}),
            }),
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0x5fe55555554c71d0, 0x873fffdd236aaaa3, 0x6a6b4619b26ef918, 0x21c2888408874945, 0x2836cda7028cabc5, 0x0ac73310a7fd5abd}),
            }),
            fp2({
                fp({0x0a0c5555555971c3, 0xdb0c00101f9eaaae, 0xb1fb2f941d797997, 0xd3960742ef416e1c, 0xb70040e2c20556f4, 0x149d7861e581393b}),
                fp({0xaff2aaaaaaa638e8, 0x439fffee91b55551, 0xb535a30cd9377c8c, 0x90e144420443a4a2, 0x941b66d3814655e2, 0x0563998853fead5e}),
            }),
            fp2({
                fp({0x40aac71c71c725ed, 0x190955557a84e38e, 0xd817050a8f41abc3, 0xd86485d4c87f6fb1, 0x696eb479f885d059, 0x198e1a74328002d2}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
        },
        {
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0x1f3affffff13ab97, 0xf25bfc611da3ff3e, 0xca3757cb3819b208, 0x3e6427366f8cec18, 0x03977bc86095b089, 0x04f69db13f39a952}),
            }),
            fp2({
                fp({0x447600000027552e, 0xdcb8009a43480020, 0x6f7ee9ce4a6e8b59, 0xb10330b7c0a95bc6, 0x6140b1fcfb1e54b7, 0x0381be097f0bb4e1}),
                fp({0x7588ffffffd8557d, 0x41f3ff646e0bffdf, 0xf7b1e8d2ac426aca, 0xb3741acd32dbb6f8, 0xe9daf5b9482d581f, 0x167f53e0ba7431b8}),
            }),
            fp2({
                fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
        },
        {
            fp2({
                fp({0x96d8f684bdfc77be, 0xb530e4f43b66d0e2, 0x184a88ff379652fd, 0x57cb23ecfae804e1, 0x0fd2e39eada3eba9, 0x08c8055e31c5d5c3}),
                fp({0x96d8f684bdfc77be, 0xb530e4f43b66d0e2, 0x184a88ff379652fd, 0x57cb23ecfae804e1, 0x0fd2e39eada3eba9, 0x08c8055e31c5d5c3}),
            }),
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0xbf0a71c71c91b406, 0x4d6d55d28b7638fd, 0x9d82f98e5f205aee, 0xa27aa27b1d1a18d5, 0x02c3b2b2d2938e86, 0x0c7d13420b09807f}),
            }),
            fp2({
                fp({0xd7f9555555531c74, 0x21cffff748daaaa8, 0x5a9ad1866c9bbe46, 0x4870a2210221d251, 0x4a0db369c0a32af1, 0x02b1ccc429ff56af}),
                fp({0xe205aaaaaaac8e37, 0xfcdc000768795556, 0x0c96011a8a1537dd, 0x1c06a963f163406e, 0x010df44c82a881e6, 0x174f45260f808feb}),
            }),
            fp2({
                fp({0xa470bda12f67f35c, 0xc0fe38e23327b425, 0xc9d3d0f2c6f0678d, 0x1c55c9935b5a982e, 0x27f6c0e2f0746764, 0x117c5e6e28aa9054}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
        },
        {
            fp2({
                fp({0x0162fffffa765adf, 0x8f7bea480083fb75, 0x561b3c2259e93611, 0x11e19fc1a9c875d5, 0xca713efc00367660, 0x03c6a03d41da1151}),
                fp({0x0162fffffa765adf, 0x8f7bea480083fb75, 0x561b3c2259e93611, 0x11e19fc1a9c875d5, 0xca713efc00367660, 0x03c6a03d41da1151}),
            }),
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0x5db0fffffd3b02c5, 0xd713f52358ebfdba, 0x5ea60761a84d161a, 0xbb2c75a34ea6c44a, 0x0ac6735921c1119b, 0x0ee3d913bdacfbf6}),
            }),
            fp2({
                fp({0x66b10000003affc5, 0xcb1400e764ec0030, 0xa73e5eb56fa5d106, 0x8984c913a0fe09a9, 0x11e10afb78ad7f13, 0x05429d0e3e918f52}),
                fp({0x534dffffffc4aae6, 0x5397ff174c67ffcf, 0xbff273eb870b251d, 0xdaf2827152870915, 0x393a9cbaca9e2dc3, 0x14be74dbfaee5748}),
            }),
            fp2({
                fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
        }
    };
    fp2 (*params)[4] = isogenyConstantsG2;
    uint64_t degree = 3;
    fp2 xNum = params[0][degree];
    fp2 xDen = params[1][degree];
    fp2 yNum = params[2][degree];
    fp2 yDen = params[3][degree];
    for(int64_t i = degree - 1; i >= 0; i--)
    {
        xNum = xNum.multiply(x);
        xDen = xDen.multiply(x);
        yNum = yNum.multiply(x);
        yDen = yDen.multiply(x);
        xNum = xNum.add(params[0][i]);
        xDen = xDen.add(params[1][i]);
        yNum = yNum.add(params[2][i]);
        yDen = yDen.add(params[3][i]);
    }
    xDen = xDen.inverse();
    yDen = yDen.inverse();
    xNum = xNum.multiply(xDen);
    yNum = yNum.multiply(yDen);
    yNum = yNum.multiply(y);
    x = xNum;
    y = yNum;
}
*/
g2 g2::isogenyMap() const
{
    fp2 isogenyConstantsG2[4][4] = {
        {
            fp2({
                fp({0x47f671c71ce05e62, 0x06dd57071206393e, 0x7c80cd2af3fd71a2, 0x048103ea9e6cd062, 0xc54516acc8d037f6, 0x13808f550920ea41}),
                fp({0x47f671c71ce05e62, 0x06dd57071206393e, 0x7c80cd2af3fd71a2, 0x048103ea9e6cd062, 0xc54516acc8d037f6, 0x13808f550920ea41}),
            }),
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0x5fe55555554c71d0, 0x873fffdd236aaaa3, 0x6a6b4619b26ef918, 0x21c2888408874945, 0x2836cda7028cabc5, 0x0ac73310a7fd5abd}),
            }),
            fp2({
                fp({0x0a0c5555555971c3, 0xdb0c00101f9eaaae, 0xb1fb2f941d797997, 0xd3960742ef416e1c, 0xb70040e2c20556f4, 0x149d7861e581393b}),
                fp({0xaff2aaaaaaa638e8, 0x439fffee91b55551, 0xb535a30cd9377c8c, 0x90e144420443a4a2, 0x941b66d3814655e2, 0x0563998853fead5e}),
            }),
            fp2({
                fp({0x40aac71c71c725ed, 0x190955557a84e38e, 0xd817050a8f41abc3, 0xd86485d4c87f6fb1, 0x696eb479f885d059, 0x198e1a74328002d2}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
        },
        {
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0x1f3affffff13ab97, 0xf25bfc611da3ff3e, 0xca3757cb3819b208, 0x3e6427366f8cec18, 0x03977bc86095b089, 0x04f69db13f39a952}),
            }),
            fp2({
                fp({0x447600000027552e, 0xdcb8009a43480020, 0x6f7ee9ce4a6e8b59, 0xb10330b7c0a95bc6, 0x6140b1fcfb1e54b7, 0x0381be097f0bb4e1}),
                fp({0x7588ffffffd8557d, 0x41f3ff646e0bffdf, 0xf7b1e8d2ac426aca, 0xb3741acd32dbb6f8, 0xe9daf5b9482d581f, 0x167f53e0ba7431b8}),
            }),
            fp2({
                fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
        },
        {
            fp2({
                fp({0x96d8f684bdfc77be, 0xb530e4f43b66d0e2, 0x184a88ff379652fd, 0x57cb23ecfae804e1, 0x0fd2e39eada3eba9, 0x08c8055e31c5d5c3}),
                fp({0x96d8f684bdfc77be, 0xb530e4f43b66d0e2, 0x184a88ff379652fd, 0x57cb23ecfae804e1, 0x0fd2e39eada3eba9, 0x08c8055e31c5d5c3}),
            }),
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0xbf0a71c71c91b406, 0x4d6d55d28b7638fd, 0x9d82f98e5f205aee, 0xa27aa27b1d1a18d5, 0x02c3b2b2d2938e86, 0x0c7d13420b09807f}),
            }),
            fp2({
                fp({0xd7f9555555531c74, 0x21cffff748daaaa8, 0x5a9ad1866c9bbe46, 0x4870a2210221d251, 0x4a0db369c0a32af1, 0x02b1ccc429ff56af}),
                fp({0xe205aaaaaaac8e37, 0xfcdc000768795556, 0x0c96011a8a1537dd, 0x1c06a963f163406e, 0x010df44c82a881e6, 0x174f45260f808feb}),
            }),
            fp2({
                fp({0xa470bda12f67f35c, 0xc0fe38e23327b425, 0xc9d3d0f2c6f0678d, 0x1c55c9935b5a982e, 0x27f6c0e2f0746764, 0x117c5e6e28aa9054}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
        },
        {
            fp2({
                fp({0x0162fffffa765adf, 0x8f7bea480083fb75, 0x561b3c2259e93611, 0x11e19fc1a9c875d5, 0xca713efc00367660, 0x03c6a03d41da1151}),
                fp({0x0162fffffa765adf, 0x8f7bea480083fb75, 0x561b3c2259e93611, 0x11e19fc1a9c875d5, 0xca713efc00367660, 0x03c6a03d41da1151}),
            }),
            fp2({
                fp({0, 0, 0, 0, 0, 0}),
                fp({0x5db0fffffd3b02c5, 0xd713f52358ebfdba, 0x5ea60761a84d161a, 0xbb2c75a34ea6c44a, 0x0ac6735921c1119b, 0x0ee3d913bdacfbf6}),
            }),
            fp2({
                fp({0x66b10000003affc5, 0xcb1400e764ec0030, 0xa73e5eb56fa5d106, 0x8984c913a0fe09a9, 0x11e10afb78ad7f13, 0x05429d0e3e918f52}),
                fp({0x534dffffffc4aae6, 0x5397ff174c67ffcf, 0xbff273eb870b251d, 0xdaf2827152870915, 0x393a9cbaca9e2dc3, 0x14be74dbfaee5748}),
            }),
            fp2({
                fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
                fp({0, 0, 0, 0, 0, 0}),
            }),
        }
    };
    
    fp2 t0, t1, t2, t3;
    g2 q;
    fp2 (*params)[4] = isogenyConstantsG2;

    t0 = params[0][3];
    for(int i = 3; i > 0; --i)
    {
        t0 = t0.multiply(x);
        t0 = t0.add(params[0][i - 1]);
    }
    t1 = params[2][3];
    for(int i = 3; i > 0; --i)
    {
        t1 = t1.multiply(x);
        t1 = t1.add(params[2][i - 1]);
    }
    t2 = params[3][3];
    for(int i = 3; i > 0; --i)
    {
        t2 = t2.multiply(x);
        t2 = t2.add(params[3][i - 1]);
    }
    t3 = params[1][2];
    for(int i = 2; i > 0; --i)
    {
        t3 = t3.multiply(x);
        t3 = t3.add(params[1][i - 1]);
    }

    // Y = Ny * Dx * Z^2.
    q.y = y.multiply(t1);
    q.y = q.y.multiply(t3);
    // Z = Dx * Dy, t1 = Z^2.
    q.z = t2.multiply(t3);
    t1 = q.z.square();
    q.y = q.y.multiply(t1);
    // X = Nx * Dy * Z.
    q.x = t0.multiply(t2);
    q.x = q.x.multiply(q.z);

    return q;
}

const g2 g2::BASE = g2({
    fp2({
        fp({0xf5f28fa202940a10, 0xb3f5fb2687b4961a, 0xa1a893b53e2ae580, 0x9894999d1a3caee9, 0x6f67b7631863366b, 0x058191924350bcd7}),
        fp({0xa5a9c0759e23f606, 0xaaa0c59dbccd60c3, 0x3bb17e18e2867806, 0x1b1ab6cc8541b367, 0xc2b6ed0ef2158547, 0x11922a097360edf3}),
    }),
    fp2({
        fp({0x4c730af860494c4a, 0x597cfa1f5e369c5a, 0xe7e6856caa0a635a, 0xbbefb5e96e0d495f, 0x07d3a975f0ef25a2, 0x083fd8e7e80dae5}),
        fp({0xadc0fc92df64b05d, 0x18aa270a2b1461dc, 0x86adac6a3be4eba0, 0x79495c4ec93da33a, 0xe7175850a43ccaed, 0xb2bc2a163de1bf2}),
    }),
    fp2({
        fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
});

const array<uint64_t, 1> g2::cofactorEFF = {0xd201000000010000};

} // namespace bls12_381
