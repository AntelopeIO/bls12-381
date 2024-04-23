#include <bls12-381/bls12-381.hpp>

using namespace std;

namespace bls12_381
{

fp::fp() : d{0, 0, 0, 0, 0, 0}
{
}

fp::fp(const array<uint64_t, 6>& d) : d{d[0], d[1], d[2], d[3], d[4], d[5]}
{
}

fp::fp(const fp& e) : d{e.d[0], e.d[1], e.d[2], e.d[3], e.d[4], e.d[5]}
{
}

optional<fp> fp::fromBytesBE(const span<const uint8_t, 48> in, const conv_opt opt)
{
    // We decided to always validate the input here. But we reserve the flag.
    fp e = fp(scalar::fromBytesBE<6>(in));
    if(opt.check_valid && !e.isValid()) return {};
    if(opt.to_mont) return e.toMont();
    else            return e;
}

optional<fp> fp::fromBytesLE(const span<const uint8_t, 48> in, const conv_opt opt)
{
    // We decided to always validate the input here. But we reserve the flag.
    fp e = fp(scalar::fromBytesLE<6>(in));
    if(opt.check_valid && !e.isValid()) return {};
    if(opt.to_mont) return e.toMont();
    else            return e;
}

void fp::toBytesBE(const span<uint8_t, 48> out, const from_mont fm /* = from_mont::yes */) const
{
    if(fm == from_mont::yes) scalar::toBytesBE<6>(fromMont().d, out);
    else                     scalar::toBytesBE<6>(d, out);
}

void fp::toBytesLE(const span<uint8_t, 48> out, const from_mont fm /* = from_mont::yes */) const
{
    if(fm == from_mont::yes) scalar::toBytesLE<6>(fromMont().d, out);
    else                     scalar::toBytesLE<6>(d, out);
}

array<uint8_t, 48> fp::toBytesBE(const from_mont fm /* = from_mont::yes */) const
{
    if(fm == from_mont::yes) return scalar::toBytesBE<6>(fromMont().d);
    else                     return scalar::toBytesBE<6>(d);
}

array<uint8_t, 48> fp::toBytesLE(const from_mont fm /* = from_mont::yes */) const
{
    if(fm == from_mont::yes) return scalar::toBytesLE<6>(fromMont().d);
    else                     return scalar::toBytesLE<6>(d);
}

fp fp::zero()
{
    return fp({0, 0, 0, 0, 0, 0});
}

fp fp::one()
{
    return R1;
}

bool fp::isValid() const
{
    return cmp(MODULUS) < 0;
}

bool fp::isOdd() const
{
    return (d[0] & 1) != 0;
}

bool fp::isEven() const
{
    return (d[0] & 1) == 0;
}

bool fp::isZero() const
{
    return (d[5] | d[4] | d[3] | d[2] | d[1] | d[0]) == 0;
}

bool fp::isOne() const
{
    return equal(R1);
}

bool fp::equal(const fp& e) const
{
    return d[0] == e.d[0] && d[1] == e.d[1] && d[2] == e.d[2] && d[3] == e.d[3] && d[4] == e.d[4] && d[5] == e.d[5];
}

bool fp::sign() const
{
    return (fromMont().d[0] & 1) == 0;
}

fp fp::add(const fp& e) const
{
    fp c;
    _add(&c, this, &e);
    return c;
}

void fp::addAssign(const fp& e)
{
    _add(this, this, &e);
}

fp fp::dbl() const
{
    fp c;
    _double(&c, this);
    return c;
}

void fp::doubleAssign()
{
    _double(this, this);
}

fp fp::subtract(const fp& e) const
{
    fp c;
    _subtract(&c, this, &e);
    return c;
}

void fp::subtractAssign(const fp& e)
{
    _subtract(this, this, &e);
}

fp fp::negate() const
{
    fp c;
    _negate(&c, this);
    return c;
}

fp fp::multiply(const fp& e) const
{
    fp c;
    _multiply(&c, this, &e);
    return c;
}

void fp::multiplyAssign(const fp& e)
{
    _multiply(this, this, &e);
}

fp fp::square() const
{
    fp c;
    _square(&c, this);
    return c;
}

void fp::squareAssign()
{
    _square(this, this);
}

void fp::div2(const uint64_t& e)
{
    d[0] = d[0]>>1 | d[1]<<63;
    d[1] = d[1]>>1 | d[2]<<63;
    d[2] = d[2]>>1 | d[3]<<63;
    d[3] = d[3]>>1 | d[4]<<63;
    d[4] = d[4]>>1 | d[5]<<63;
    d[5] = d[5]>>1 | e<<63;
}

uint64_t fp::mul2()
{
    uint64_t e = d[5] >> 63;
    d[5] = d[5]<<1 | d[4]>>63;
    d[4] = d[4]<<1 | d[3]>>63;
    d[3] = d[3]<<1 | d[2]>>63;
    d[2] = d[2]<<1 | d[1]>>63;
    d[1] = d[1]<<1 | d[0]>>63;
    d[0] = d[0] << 1;
    return e;
}

fp fp::toMont() const
{
    fp c;
    _multiply(&c, this, &R2);
    return c;
}

fp fp::fromMont() const
{
    fp c, b = fp({1, 0, 0, 0, 0, 0});
    _multiply(&c, this, &b);
    return c;
}

fp fp::phi() const
{
    fp c;
    _multiply(&c, this, &glvPhi1);
    return c;
}

// Origin algorithm comes from:
// [1] B.S.Kaliski Jr. The Montgomery inverse and its applications. IEEE Transactions on Computers, 44(8):1064–1065, August 1995.
// Modified according to:
// [2] Savas, Erkay, and Cetin Kaya Koç. "The Montgomery modular inverse-revisited." IEEE transactions on computers 49, no. 7 (2000): 763-766.
fp fp::inverse() const
{
    if(isZero())
    {
        return zero();
    }
    fp u = MODULUS;
    fp v = *this;
    fp s({1, 0, 0, 0, 0, 0});
    fp r({0, 0, 0, 0, 0, 0});
    int64_t k = 0;
    bool found = false;

    // Phase 1
    // Input: a*2^384 mod p < p, p.
    // Output: a^{-1} * 2^{k-384} mod p, where 381 <= k <= 2 * 381.
    // 1. Input is in Montgomery form, so the input is a*2^384 mod p.
    // 2. All Phase 1 operations will treat fp as integer in Z instead of elements in GF(p).
    // Therefore, NO modular reduction shall happen in Phase 1.
    // 3. As proved in [1], s,r,u,v < 2p. 
    // Therefore, there shall be NO overflow in the process as p has 381 bits and we can hold 384 bits in fp.
    for(uint64_t i = 0; i < 768; i++)
    {
        if(v.isZero())
        {
            found = true;
            break;
        }
        if(u.isEven())
        {
            u.div2(0);
            s.mul2();
        }
        else if(v.isEven())
        {
            v.div2(0);
            r.mul2();
        }
        else if(u.cmp(v) > 0)
        {
            _lsubtract(&u, &u, &v);
            u.div2(0);
            _ladd(&r, &r, &s);
            s.mul2();
        }
        else
        {
            _lsubtract(&v, &v, &u);
            v.div2(0);
            _ladd(&s, &s, &r);
            r.mul2();
        }
        k += 1;
    }

    if(!found)
    {
        return zero();
    }

    if(r.cmp(MODULUS) >= 0)
    {
        _lsubtract(&r, &r, &MODULUS);
    }
    u = MODULUS;
    _lsubtract(&u, &u, &r);

    // Phase 2
    // Input: u = a^{-1} * 2^{k-384} mod p, k where 381 <= k <= 2*381
    // Output: a^{-1} * 2^384 mod p

    // Process for 381 <= k <= 384
    // Generate new u, k pair that
    // k' = k + 384
    // u' = u * 2^{2*384} * 2^{-384} mod p
    // u' = a^{-1} * 2^{k'-384} mod p will still hold.
    // Will hit this case if and only if input is fp::one() for the current p.
    if (k <= 384) {
        u.multiplyAssign(fp::R2);
        k += 384;
    }

    // u = u * 2^{2*384} * 2^{-384} mod p = a^{-1} * 2^k mod p
    u.multiplyAssign(fp::R2);

    // This case should not happen mathmatically.
    // Leave it here as sanity check to guard against access vialotion later.
    if(k > 2 * 384)
    {
        return zero();
    }

    // 384 < k <= 2 * 384
    // 0 <= power < 384
    uint64_t power = 2*384 - k;
    fp fpPower;
    fpPower.d[power/64] = ((uint64_t)1) << (power%64);
   
    // result = u * 2^(2*384-k) * 2^{-384} = a^{-1} * 2^k * 2^{384-k} mod p = a^{-1} * 2^384 mod p
    u.multiplyAssign(fpPower);

    return u;
}

bool fp::sqrt(fp& c) const
{
    fp u = *this;
    fp v;
    c = exp(pPlus1Over4);
    _square(&v, &c);
    return u.equal(v);
}

bool fp::isQuadraticNonResidue() const
{
    fp result = exp(pMinus1Over2);
    return !result.isOne();
}

// Returns whether or not this element is strictly lexicographically larger than its negation.
bool fp::isLexicographicallyLargest() const
{
    array<uint64_t, 6> halfQ = {
        0x0D00'88F5'1CBF'F34D,
        0x258D'D3DB'21A5'D66B,
        0xB23B'A5C2'79C2'895F,
        0xB398'6950'7B58'7B12,
        0x0F55'FFFF'58A9'FFFF,
        0xDCFF'7FFF'FFFF'D556
    };
    array<uint64_t, 6> yValue = fromMont().d;
    
    uint64_t _, borrow;
    tie(_, borrow) = Sub64(yValue[0], halfQ[5], 0);         // If the element was smaller, the subtraction will underflow
    tie(_, borrow) = Sub64(yValue[1], halfQ[4], borrow);    // producing a borrow value of 0xffff...ffff, otherwise it will
    tie(_, borrow) = Sub64(yValue[2], halfQ[3], borrow);    // be zero. We create a Choice representing true if there was
    tie(_, borrow) = Sub64(yValue[3], halfQ[2], borrow);    // overflow (and so this element is not lexicographically larger
    tie(_, borrow) = Sub64(yValue[4], halfQ[1], borrow);    // than its negation) and then negate it.
    tie(_, borrow) = Sub64(yValue[5], halfQ[0], borrow);
    
    // if underflow bit is NOT set this field element is larger than Q/2
    return borrow == 0;
}

const fp fp::MODULUS = fp(
{
    0xb9fe'ffff'ffff'aaab,
    0x1eab'fffe'b153'ffff,
    0x6730'd2a0'f6b0'f624,
    0x6477'4b84'f385'12bf,
    0x4b1b'a7b6'434b'acd7,
    0x1a01'11ea'397f'e69a,
});

const uint64_t fp::INP = 0x89f3'fffc'fffc'fffd;

const fp fp::R1 = fp({
    0x7609'0000'0002'fffd,
    0xebf4'000b'c40c'0002,
    0x5f48'9857'53c7'58ba,
    0x77ce'5853'7052'5745,
    0x5c07'1a97'a256'ec6d,
    0x15f6'5ec3'fa80'e493,
});

const fp fp::R2 = fp({
    0xf4df'1f34'1c34'1746,
    0x0a76'e6a6'09d1'04f1,
    0x8de5'476c'4c95'b6d5,
    0x67eb'88a9'939d'83c0,
    0x9a79'3e85'b519'952d,
    0x1198'8fe5'92ca'e3aa,
});

const fp fp::B = fp({
    0xaa27'0000'000c'fff3,
    0x53cc'0032'fc34'000a,
    0x478f'e97a'6b0a'807f,
    0xb1d3'7ebe'e6ba'24d7,
    0x8ec9'733b'bf78'ab2f,
    0x09d6'4551'3d83'de7e,
});

const fp fp::twoInv = fp({
    0x1804'0000'0001'5554,
    0x8550'0005'3ab0'0001,
    0x633c'b57c'253c'276f,
    0x6e22'd1ec'31eb'b502,
    0xd391'6126'f2d1'4ca2,
    0x17fb'b857'1a00'6596
});

const fp fp::glvPhi1 = fp({
    0xcd03'c9e4'8671'f071,
    0x5dab'2246'1fcd'a5d2,
    0x5870'42af'd385'1b95,
    0x8eb6'0ebe'01ba'cb9e,
    0x03f9'7d6e'83d0'50d2,
    0x18f0'2065'5463'8741
});

const array<uint64_t, 4> fp::Q = {
    0xffff'ffff'0000'0001,
    0x53bd'a402'fffe'5bfe,
    0x3339'd808'09a1'd805,
    0x73ed'a753'299d'7d48
};

const array<uint64_t, 6> fp::pPlus1Over4 = {
    0xee7f'bfff'ffff'eaab,
    0x07aa'ffff'ac54'ffff,
    0xd9cc'34a8'3dac'3d89,
    0xd91d'd2e1'3ce1'44af,
    0x92c6'e9ed'90d2'eb35,
    0x0680'447a'8e5f'f9a6
};

const array<uint64_t, 6> fp::pMinus1Over2 = {
    0xdcff'7fff'ffff'd555,
    0x0f55'ffff'58a9'ffff,
    0xb398'6950'7b58'7b12,
    0xb23b'a5c2'79c2'895f,
    0x258d'd3db'21a5'd66b,
    0x0d00'88f5'1cbf'f34d
};

const array<uint64_t, 6> fp::pMinus3Over4 = {
    0xee7f'bfff'ffff'eaaa,
    0x07aa'ffff'ac54'ffff,
    0xd9cc'34a8'3dac'3d89,
    0xd91d'd2e1'3ce1'44af,
    0x92c6'e9ed'90d2'eb35,
    0x0680'447a'8e5f'f9a6
};

fp2::fp2() : c0(fp()), c1(fp())
{
}

fp2::fp2(const array<fp, 2>& e2) : c0(e2[0]), c1(e2[1])
{
}

fp2::fp2(const fp2& e) : c0(e.c0), c1(e.c1)
{
}

optional<fp2> fp2::fromBytesBE(const span<const uint8_t, 96> in, const conv_opt opt)
{
    optional<fp> c1 = fp::fromBytesBE(span<const uint8_t, 48>(&in[ 0], 48), opt);
    optional<fp> c0 = fp::fromBytesBE(span<const uint8_t, 48>(&in[48], 48), opt);
    if(!c1 || !c0) return {};
    return fp2({*c0, *c1});
}

optional<fp2> fp2::fromBytesLE(const span<const uint8_t, 96> in, const conv_opt opt)
{
    optional<fp> c0 = fp::fromBytesLE(span<const uint8_t, 48>(&in[ 0], 48), opt);
    optional<fp> c1 = fp::fromBytesLE(span<const uint8_t, 48>(&in[48], 48), opt);
    if(!c1 || !c0) return {};
    return fp2({*c0, *c1});
}

void fp2::toBytesBE(const span<uint8_t, 96> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[ 0], &c1.toBytesBE(fm)[0], 48);
    memcpy(&out[48], &c0.toBytesBE(fm)[0], 48);
}

void fp2::toBytesLE(const span<uint8_t, 96> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[ 0], &c0.toBytesLE(fm)[0], 48);
    memcpy(&out[48], &c1.toBytesLE(fm)[0], 48);
}

array<uint8_t, 96> fp2::toBytesBE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 96> out;
    toBytesBE(out, fm);
    return out;
}

array<uint8_t, 96> fp2::toBytesLE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 96> out;
    toBytesLE(out, fm);
    return out;
}

fp2 fp2::zero()
{
    return fp2({fp::zero(), fp::zero()});
}

fp2 fp2::one()
{
    return fp2({fp::one(), fp::zero()});
}

bool fp2::isOne() const
{
    return c0.isOne() && c1.isZero();
}

bool fp2::isZero() const
{
    return c0.isZero() && c1.isZero();
}

bool fp2::equal(const fp2& e) const
{
    return c0.equal(e.c0) && c1.equal(e.c1);
}

bool fp2::sign() const
{
    fp r;
    if(!c0.isZero())
    {
        return (c0.fromMont().d[0] & 1) == 0;
    }
    return (c1.fromMont().d[0] & 1) == 0;
}

fp2 fp2::add(const fp2& e) const
{
    fp2 c(*this);
    c.addAssign(e);
    return c;
}

void fp2::addAssign(const fp2& e)
{
    _add(&c0, &c0, &e.c0);
    _add(&c1, &c1, &e.c1);
}

fp2 fp2::dbl() const
{
    fp2 c(*this);
    c.doubleAssign();
    return c;
}

void fp2::doubleAssign()
{
    _double(&c0, &c0);
    _double(&c1, &c1);
}

fp2 fp2::subtract(const fp2& e) const
{
    fp2 c(*this);
    c.subtractAssign(e);
    return c;
}

void fp2::subtractAssign(const fp2& e)
{
    _subtract(&c0, &c0, &e.c0);
    _subtract(&c1, &c1, &e.c1);
}

fp2 fp2::negate() const
{
    fp2 c;
    _negate(&c.c0, &c0);
    _negate(&c.c1, &c1);
    return c;
}

fp2 fp2::conjugate() const
{
    fp2 c;
    c.c0 = c0;
    _negate(&c.c1, &c1);
    return c;
}

fp2 fp2::multiply(const fp2& e) const
{
    fp2 c(*this);
    c.multiplyAssign(e);
    return c;
}

void fp2::multiplyAssign(const fp2& e)
{
    fp t[4];
    _multiply(&t[1], &c0, &e.c0);
    _multiply(&t[2], &c1, &e.c1);
    _add(&t[0], &c0, &c1);
    _add(&t[3], &e.c0, &e.c1);
    _subtract(&c0, &t[1], &t[2]);
    _add(&t[1], &t[1], &t[2]);
    _multiply(&t[0], &t[0], &t[3]);
    _subtract(&c1, &t[0], &t[1]);
}

fp2 fp2::square() const
{
    fp2 c(*this);
    c.squareAssign();
    return c;
}

void fp2::squareAssign()
{
    fp t[3];
    _ladd(&t[0], &c0, &c1);
    _subtract(&t[1], &c0, &c1);
    _ldouble(&t[2], &c0);
    _multiply(&c0, &t[0], &t[1]);
    _multiply(&c1, &t[2], &c1);
}

fp2 fp2::mulByNonResidue() const
{
    fp t;
    fp2 c;
    _subtract(&t, &c0, &c1);
    _add(&c.c1, &c0, &c1);
    c.c0 = t;
    return c;
}

fp2 fp2::mulByB() const
{
    fp t[2];
    fp2 c;
    _double(&t[0], &c0);
    _double(&t[1], &c1);
    _double(&t[0], &t[0]);
    _double(&t[1], &t[1]);
    _subtract(&c.c0, &t[0], &t[1]);
    _add(&c.c1, &t[0], &t[1]);
    return c;
}

fp2 fp2::inverse() const
{
    fp t[2];
    fp2 c;
    _square(&t[0], &c0);
    _square(&t[1], &c1);
    _add(&t[0], &t[0], &t[1]);
    t[0] = t[0].inverse();
    _multiply(&c.c0, &c0, &t[0]);
    _multiply(&t[0], &t[0], &c1);
    _negate(&c.c1, &t[0]);
    return c;
}

fp2 fp2::mulByFq(const fp& e) const
{
    fp2 c;
    _multiply(&c.c0, &c0, &e);
    _multiply(&c.c1, &c1, &e);
    return c;
}

fp2 fp2::frobeniusMap(const uint64_t& power) const
{
    fp2 c;
    c.c0 = c0;
    if(power%2 == 1)
    {
        _negate(&c.c1, &c1);
        return c;
    }
    c.c1 = c1;
    return c;
}

void fp2::frobeniusMapAssign(const uint64_t& power)
{
    if(power%2 == 1)
    {
        _negate(&c1, &c1);
    }
}

bool fp2::sqrt(fp2& c) const
{
    fp2 u, x0, a1, alpha;
    u = *this;
    a1 = this->exp(fp::pMinus3Over4);
    alpha = a1.square();
    alpha = alpha.multiply(*this);
    x0 = a1.multiply(*this);
    if(alpha.equal(fp2::negativeOne2))
    {
        _negate(&c.c0, &x0.c1);
        c.c1 = x0.c0;
        return true;
    }
    alpha = alpha.add(fp2::one());
    alpha = alpha.exp(fp::pMinus1Over2);
    c = alpha.multiply(x0);
    alpha = c.square();
    return alpha.equal(u);
}

bool fp2::isQuadraticNonResidue() const
{
    // https://github.com/leovt/constructible/wiki/Taking-Square-Roots-in-quadratic-extension-Fields
    fp _c0, _c1;
    _square(&_c0, &c0);
    _square(&_c1, &c1);
    _add(&_c1, &_c1, &_c0);
    return _c1.isQuadraticNonResidue();
}

// Returns whether or not this element is strictly lexicographically larger than its negation.
bool fp2::isLexicographicallyLargest() const
{
    return c1.isLexicographicallyLargest() || (c1.isZero() && c0.isLexicographicallyLargest());
}

const fp2 fp2::negativeOne2 = fp2({
    fp({
        0x43f5'ffff'fffc'aaae,
        0x32b7'fff2'ed47'fffd,
        0x07e8'3a49'a2e9'9d69,
        0xeca8'f331'8332'bb7a,
        0xef14'8d1e'a0f4'c069,
        0x040a'b326'3eff'0206
    }),
    fp({
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000
    }),
});

const fp2 fp2::B = fp2({
    fp({
        0xaa27'0000'000c'fff3,
        0x53cc'0032'fc34'000a,
        0x478f'e97a'6b0a'807f,
        0xb1d3'7ebe'e6ba'24d7,
        0x8ec9'733b'bf78'ab2f,
        0x09d6'4551'3d83'de7e,
    }),
    fp({
        0xaa27'0000'000c'fff3,
        0x53cc'0032'fc34'000a,
        0x478f'e97a'6b0a'807f,
        0xb1d3'7ebe'e6ba'24d7,
        0x8ec9'733b'bf78'ab2f,
        0x09d6'4551'3d83'de7e,
    }),
});

const fp2 fp2::psiX = fp2({
    fp({
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000,
        0x0000'0000'0000'0000,
    }),
    fp({
        0x890d'c9e4'8675'45c3,
        0x2af3'2253'3285'a5d5,
        0x5088'0866'309b'7e2c,
        0xa20d'1b8c'7e88'1024,
        0x14e4'f04f'e2db'9068,
        0x14e5'6d3f'1564'853a,
    }),
});

const fp2 fp2::psiY = fp2({
    fp({
        0x3e2f'585d'a55c'9ad1,
        0x4294'213d'86c1'8183,
        0x3828'44c8'8b62'3732,
        0x92ad'2afd'1910'3e18,
        0x1d79'4e4f'ac7c'f0b9,
        0x0bd5'92fc'7d82'5ec8,
    }),
    fp({
        0x7bcf'a7a2'5aa3'0fda,
        0xdc17'dec1'2a92'7e7c,
        0x2f08'8dd8'6b4e'bef1,
        0xd1ca'2087'da74'd4a7,
        0x2da2'5966'96ce'bc1d,
        0x0e2b'7eed'bbfd'87d2,
    }),
});

fp6::fp6() : c0(fp2()), c1(fp2()), c2(fp2())
{
}

fp6::fp6(const array<fp2, 3>& e3) : c0(e3[0]), c1(e3[1]), c2(e3[2])
{
}

fp6::fp6(const fp6& e) : c0(e.c0), c1(e.c1), c2(e.c2)
{
}

optional<fp6> fp6::fromBytesBE(const span<const uint8_t, 288> in, const conv_opt opt)
{
    optional<fp2> c2 = fp2::fromBytesBE(span<const uint8_t, 96>(&in[  0], 96), opt);
    optional<fp2> c1 = fp2::fromBytesBE(span<const uint8_t, 96>(&in[ 96], 96), opt);
    optional<fp2> c0 = fp2::fromBytesBE(span<const uint8_t, 96>(&in[192], 96), opt);
    if(!c2 || !c1 || !c0) return {};
    return fp6({*c0, *c1, *c2});
}

optional<fp6> fp6::fromBytesLE(const span<const uint8_t, 288> in, const conv_opt opt)
{
    optional<fp2> c0 = fp2::fromBytesLE(span<const uint8_t, 96>(&in[  0], 96), opt);
    optional<fp2> c1 = fp2::fromBytesLE(span<const uint8_t, 96>(&in[ 96], 96), opt);
    optional<fp2> c2 = fp2::fromBytesLE(span<const uint8_t, 96>(&in[192], 96), opt);
    if(!c2 || !c1 || !c0) return {};
    return fp6({*c0, *c1, *c2});
}

void fp6::toBytesBE(const span<uint8_t, 288> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[  0], &c2.toBytesBE(fm)[0], 96);
    memcpy(&out[ 96], &c1.toBytesBE(fm)[0], 96);
    memcpy(&out[192], &c0.toBytesBE(fm)[0], 96);
}

void fp6::toBytesLE(const span<uint8_t, 288> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[  0], &c0.toBytesLE(fm)[0], 96);
    memcpy(&out[ 96], &c1.toBytesLE(fm)[0], 96);
    memcpy(&out[192], &c2.toBytesLE(fm)[0], 96);
}

array<uint8_t, 288> fp6::toBytesBE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 288> out;
    toBytesBE(out, fm);
    return out;
}

array<uint8_t, 288> fp6::toBytesLE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 288> out;
    toBytesLE(out, fm);
    return out;
}

fp6 fp6::zero()
{
    return fp6({fp2::zero(), fp2::zero(), fp2::zero()});
}

fp6 fp6::one()
{
    return fp6({fp2::one(), fp2::zero(), fp2::zero()});
}

bool fp6::isOne() const
{
    return c0.isOne() && c1.isZero() && c2.isZero();
}

bool fp6::isZero() const
{
    return c0.isZero() && c1.isZero() && c2.isZero();
}

bool fp6::equal(const fp6& e) const
{
    return c0.equal(e.c0) && c1.equal(e.c1) && c2.equal(e.c2);
}

fp6 fp6::add(const fp6& e) const
{
    fp6 c(*this);
    c.addAssign(e);
    return c;
}

void fp6::addAssign(const fp6& e)
{
    c0.addAssign(e.c0);
    c1.addAssign(e.c1);
    c2.addAssign(e.c2);
}

fp6 fp6::dbl() const
{
    fp6 c(*this);
    c.doubleAssign();
    return c;
}

void fp6::doubleAssign()
{
    c0.doubleAssign();
    c1.doubleAssign();
    c2.doubleAssign();
}

fp6 fp6::subtract(const fp6& e) const
{
    fp6 c(*this);
    c.subtractAssign(e);
    return c;
}

void fp6::subtractAssign(const fp6& e)
{
    c0.subtractAssign(e.c0);
    c1.subtractAssign(e.c1);
    c2.subtractAssign(e.c2);
}

fp6 fp6::negate() const
{
    fp6 c;
    c.c0 = c0.negate();
    c.c1 = c1.negate();
    c.c2 = c2.negate();
    return c;
}

fp6 fp6::multiply(const fp6& e) const
{
    fp6 c(*this);
    c.multiplyAssign(e);
    return c;
}

void fp6::multiplyAssign(const fp6& e)
{
    fp2 t[6];
    t[0] = c0.multiply(e.c0);
    t[1] = c1.multiply(e.c1);
    t[2] = c2.multiply(e.c2);
    t[3] = c1.add(c2);
    t[4] = e.c1.add(e.c2);
    t[3].multiplyAssign(t[4]);
    t[4] = t[1].add(t[2]);
    t[3].subtractAssign(t[4]);
    t[3] = t[3].mulByNonResidue();
    t[5] = t[0].add(t[3]);
    t[3] = c0.add(c1);
    t[4] = e.c0.add(e.c1);
    t[3].multiplyAssign(t[4]);
    t[4] = t[0].add(t[1]);
    t[3].subtractAssign(t[4]);
    t[4] = t[2].mulByNonResidue();
    c1 = t[3].add(t[4]);
    t[3] = c0.add(c2);
    t[4] = e.c0.add(e.c2);
    t[3].multiplyAssign(t[4]);
    t[4] = t[0].add(t[2]);
    t[3].subtractAssign(t[4]);
    c2 = t[1].add(t[3]);
    c0 = t[5];
}

fp6 fp6::square() const
{
    fp6 c(*this);
    c.squareAssign();
    return c;
}

void fp6::squareAssign()
{
    fp2 t[6];
    fp6 c;
    t[0] = c0.square();
    t[1] = c0.multiply(c1);
    t[1].doubleAssign();
    t[2] = c0.subtract(c1);
    t[2].addAssign(c2);
    t[2].squareAssign();
    t[3] = c1.multiply(c2);
    t[3].doubleAssign();
    t[4] = c2.square();
    t[5] = t[3].mulByNonResidue();
    c0 = t[0].add(t[5]);
    t[5] = t[4].mulByNonResidue();
    c1 = t[1].add(t[5]);
    t[1].addAssign(t[2]);
    t[1].addAssign(t[3]);
    t[0].addAssign(t[4]);
    c2 = t[1].subtract(t[0]);
}

void fp6::mulBy01Assign(const fp2& e0, const fp2& e1)
{
    fp2 t[6];
    t[0] = c0.multiply(e0);
    t[1] = c1.multiply(e1);
    t[5] = c1.add(c2);
    t[2] = e1.multiply(t[5]);
    t[2].subtractAssign(t[1]);
    t[2] = t[2].mulByNonResidue();
    t[5] = c0.add(c2);
    t[3] = e0.multiply(t[5]);
    t[3].subtractAssign(t[0]);
    c2 = t[3].add(t[1]);
    t[4] = e0.add(e1);
    t[5] = c0.add(c1);
    t[4].multiplyAssign(t[5]);
    t[4].subtractAssign(t[0]);
    c1 = t[4].subtract(t[1]);
    c0 = t[2].add(t[0]);
}

fp6 fp6::mulBy01(const fp2& e0, const fp2& e1) const
{
    fp2 t[5];
    fp6 c;
    t[0] = c0.multiply(e0);
    t[1] = c1.multiply(e1);
    t[2] = c1.add(c2);
    t[2].multiplyAssign(e1);
    t[2].subtractAssign(t[1]);
    t[2] = t[2].mulByNonResidue();
    t[3] = c0.add(c2);
    t[3].multiplyAssign(e0);
    t[3].subtractAssign(t[0]);
    c.c2 = t[3].add(t[1]);
    t[4] = e0.add(e1);
    t[3] = c0.add(c1);
    t[4].multiplyAssign(t[3]);
    t[4].subtractAssign(t[0]);
    c.c1 = t[4].subtract(t[1]);
    c.c0 = t[2].add(t[0]);
    return c;
}

fp6 fp6::mulBy1(const fp2& e1) const
{
    fp2 t;
    fp6 c;
    t = c2.multiply(e1);
    c.c2 = c1.multiply(e1);
    c.c1 = c0.multiply(e1);
    c.c0 = t.mulByNonResidue();
    return c;
}

fp6 fp6::mulByNonResidue() const
{
    fp6 c;
    c.c0 = c2.mulByNonResidue();
    c.c2 = c1;
    c.c1 = c0;
    return c;
}

fp6 fp6::mulByBaseField(const fp2& e) const
{
    fp6 c;
    c.c0 = c0.multiply(e);
    c.c1 = c1.multiply(e);
    c.c2 = c2.multiply(e);
    return c;
}

fp6 fp6::inverse() const
{
    fp2 t[5];
    fp6 c;
    t[0] = c0.square();
    t[1] = c1.multiply(c2);
    t[1] = t[1].mulByNonResidue();
    t[0].subtractAssign(t[1]);
    t[1] = c1.square();
    t[2] = c0.multiply(c2);
    t[1].subtractAssign(t[2]);
    t[2] = c2.square();
    t[2] = t[2].mulByNonResidue();
    t[3] = c0.multiply(c1);
    t[2].subtractAssign(t[3]);
    t[3] = c2.multiply(t[2]);
    t[4] = c1.multiply(t[1]);
    t[3].addAssign(t[4]);
    t[3] = t[3].mulByNonResidue();
    t[4] = c0.multiply(t[0]);
    t[3].addAssign(t[4]);
    t[3] = t[3].inverse();
    c.c0 = t[0].multiply(t[3]);
    c.c1 = t[2].multiply(t[3]);
    c.c2 = t[1].multiply(t[3]);
    return c;
}

fp6 fp6::frobeniusMap(const uint64_t& power) const
{
    fp6 c;
    c.c0 = c0.frobeniusMap(power);
    c.c1 = c1.frobeniusMap(power);
    c.c2 = c2.frobeniusMap(power);
    switch(power % 6)
    {
        case 0:
        {
            return c;
        }
        case 3:
        {
            _negate(&c.c0.c0, &c1.c1);
            c.c1.c1 = c1.c0;
            c.c2 = c2.negate();
            break;
        }
        default:
        {
            c.c1 = c.c1.multiply(frobeniusCoeffs61[power%6]);
            c.c2 = c.c2.multiply(frobeniusCoeffs62[power%6]);
            break;
        }
    }
    return c;
}

void fp6::frobeniusMapAssign(const uint64_t& power)
{
    c0.frobeniusMapAssign(power);
    c1.frobeniusMapAssign(power);
    c2.frobeniusMapAssign(power);
    fp2 t;
    switch(power % 6)
    {
        case 0:
        {
            return;
        }
        case 3:
        {
            _negate(&t.c0, &c1.c1);
            c1.c1 = c1.c0;
            c1.c0 = t.c0;
            c2 = c2.negate();
            break;
        }
        default:
        {
            c1.multiplyAssign(frobeniusCoeffs61[power%6]);
            c2.multiplyAssign(frobeniusCoeffs62[power%6]);
            break;
        }
    }
}

const array<fp2, 6> fp6::frobeniusCoeffs61 = array<fp2, 6>({
    fp2({
        fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
        fp({0xcd03c9e48671f071, 0x5dab22461fcda5d2, 0x587042afd3851b95, 0x8eb60ebe01bacb9e, 0x03f97d6e83d050d2, 0x18f0206554638741}),
    }),
    fp2({
        fp({0x30f1361b798a64e8, 0xf3b8ddab7ece5a2a, 0x16a8ca3ac61577f7, 0xc26a2ff874fd029b, 0x3636b76660701c6e, 0x051ba4ab241b6160}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
        fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
    }),
    fp2({
        fp({0xcd03c9e48671f071, 0x5dab22461fcda5d2, 0x587042afd3851b95, 0x8eb60ebe01bacb9e, 0x03f97d6e83d050d2, 0x18f0206554638741}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
        fp({0x30f1361b798a64e8, 0xf3b8ddab7ece5a2a, 0x16a8ca3ac61577f7, 0xc26a2ff874fd029b, 0x3636b76660701c6e, 0x051ba4ab241b6160}),
    }),
});

const array<fp2, 6> fp6::frobeniusCoeffs62 = array<fp2, 6>({
    fp2({
        fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x890dc9e4867545c3, 0x2af322533285a5d5, 0x50880866309b7e2c, 0xa20d1b8c7e881024, 0x14e4f04fe2db9068, 0x14e56d3f1564853a}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0xcd03c9e48671f071, 0x5dab22461fcda5d2, 0x587042afd3851b95, 0x8eb60ebe01bacb9e, 0x03f97d6e83d050d2, 0x18f0206554638741}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x43f5fffffffcaaae, 0x32b7fff2ed47fffd, 0x07e83a49a2e99d69, 0xeca8f3318332bb7a, 0xef148d1ea0f4c069, 0x040ab3263eff0206}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x30f1361b798a64e8, 0xf3b8ddab7ece5a2a, 0x16a8ca3ac61577f7, 0xc26a2ff874fd029b, 0x3636b76660701c6e, 0x051ba4ab241b6160}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0xecfb361b798dba3a, 0xc100ddb891865a2c, 0x0ec08ff1232bda8e, 0xd5c13cc6f1ca4721, 0x47222a47bf7b5c04, 0x0110f184e51c5f59}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
});

fp12::fp12() : c0(fp6()), c1(fp6())
{
}

fp12::fp12(const array<fp6, 2>& e2) : c0(e2[0]), c1(e2[1])
{
}

fp12::fp12(const fp12& e) : c0(e.c0), c1(e.c1)
{
}

optional<fp12> fp12::fromBytesBE(const span<const uint8_t, 576> in, const conv_opt opt)
{
    optional<fp6> c1 = fp6::fromBytesBE(span<const uint8_t, 288>(&in[  0], 288), opt);
    optional<fp6> c0 = fp6::fromBytesBE(span<const uint8_t, 288>(&in[288], 288), opt);
    if(!c1 || !c0) return {};
    return fp12({*c0, *c1});
}

optional<fp12> fp12::fromBytesLE(const span<const uint8_t, 576> in, const conv_opt opt)
{
    optional<fp6> c0 = fp6::fromBytesLE(span<const uint8_t, 288>(&in[  0], 288), opt);
    optional<fp6> c1 = fp6::fromBytesLE(span<const uint8_t, 288>(&in[288], 288), opt);
    if(!c1 || !c0) return {};
    return fp12({*c0, *c1});
}

void fp12::toBytesBE(const span<uint8_t, 576> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[  0], &c1.toBytesBE(fm)[0], 288);
    memcpy(&out[288], &c0.toBytesBE(fm)[0], 288);
}

void fp12::toBytesLE(const span<uint8_t, 576> out, const from_mont fm /* = from_mont::yes */) const
{
    memcpy(&out[  0], &c0.toBytesLE(fm)[0], 288);
    memcpy(&out[288], &c1.toBytesLE(fm)[0], 288);
}

array<uint8_t, 576> fp12::toBytesBE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 576> out;
    toBytesBE(out, fm);
    return out;
}

array<uint8_t, 576> fp12::toBytesLE(const from_mont fm /* = from_mont::yes */) const
{
    array<uint8_t, 576> out;
    toBytesLE(out, fm);
    return out;
}

fp12 fp12::zero()
{
    return fp12({fp6::zero(), fp6::zero()});
}

fp12 fp12::one()
{
    return fp12({fp6::one(), fp6::zero()});
}

bool fp12::isOne() const
{
    return c0.isOne() && c1.isZero();
}

bool fp12::isZero() const
{
    return c0.isZero() && c1.isZero();
}

// IsValid checks whether given target group element is in correct subgroup.
bool fp12::isGtValid() const
{
    return this->exp(fp::Q).isOne();
}

bool fp12::equal(const fp12& e) const
{
    return c0.equal(e.c0) && c1.equal(e.c1);
}

fp12 fp12::add(const fp12& e) const
{
    fp12 c(*this);
    c.addAssign(e);
    return c;
}

void fp12::addAssign(const fp12& e) {
    c0.addAssign(e.c0);
    c1.addAssign(e.c1);
}

fp12 fp12::dbl() const
{
    fp12 c(*this);
    c.doubleAssign();
    return c;
}

void fp12::doubleAssign() {
    c0.doubleAssign();
    c1.doubleAssign();
}

fp12 fp12::subtract(const fp12& e) const
{
    fp12 c(*this);
    c.subtractAssign(e);
    return c;
}

void fp12::subtractAssign(const fp12& e) {
    c0.subtractAssign(e.c0);
    c1.subtractAssign(e.c1);
}

fp12 fp12::negate() const
{
    fp12 c;
    c.c0 = c0.negate();
    c.c1 = c1.negate();
    return c;
}

fp12 fp12::conjugate() const
{
    fp12 c;
    c.c0 = c0;
    c.c1 = c1.negate();
    return c;
}

fp12 fp12::square() const
{
    fp12 c(*this);
    c.squareAssign();
    return c;
}

void fp12::squareAssign() {
    fp6 t[4];

    t[0] = c0.add(c1);
    t[2] = c0.multiply(c1);
    t[1] = c1.mulByNonResidue();
    t[1].addAssign(c0);
    t[3] = t[2].mulByNonResidue();
    t[0].multiplyAssign(t[1]);
    t[0].subtractAssign(t[2]);
    c0 = t[0].subtract(t[3]);
    c1 = t[2].dbl();
}

fp12 fp12::cyclotomicSquare() const
{
    fp12 c(*this);
    c.cyclotomicSquareAssign();
    return c;
}

void fp12::cyclotomicSquareAssign() {
    fp2 t[7];
    tie(t[3], t[4]) = fp4Square(c0.c0, c1.c1);
    t[2] = t[3].subtract(c0.c0);
    t[2].doubleAssign();
    c0.c0 = t[2].add(t[3]);
    t[2] = t[4].add(c1.c1);
    t[2].doubleAssign();
    c1.c1 = t[2].add(t[4]);
    tie(t[3], t[4]) = fp4Square(c1.c0, c0.c2);
    tie(t[5], t[6]) = fp4Square(c0.c1, c1.c2);
    t[2] = t[3].subtract(c0.c1);
    t[2].doubleAssign();
    c0.c1 = t[2].add(t[3]);
    t[2] = t[4].add(c1.c2);
    t[2].doubleAssign();
    c1.c2 = t[2].add(t[4]);
    t[3] = t[6].mulByNonResidue();
    t[2] = t[3].add(c1.c0);
    t[2].doubleAssign();
    c1.c0 = t[2].add(t[3]);
    t[2] = t[5].subtract(c0.c2);
    t[2].doubleAssign();
    c0.c2 = t[2].add(t[5]);
}

fp12 fp12::multiply(const fp12& e) const
{
    fp12 c(*this);
    c.multiplyAssign(e);
    return c;
}

void fp12::multiplyAssign(const fp12& e)
{
    fp6 t[4];
    t[1] = c0.multiply(e.c0);
    t[2] = c1.multiply(e.c1);
    t[0] = t[1].add(t[2]);
    t[2] = t[2].mulByNonResidue();
    t[3] = t[1].add(t[2]);
    t[1] = c0.add(c1);
    t[2] = e.c0.add(e.c1);
    t[1].multiplyAssign(t[2]);
    c0 = t[3];
    c1 = t[1].subtract(t[0]);
}

tuple<fp2, fp2> fp12::fp4Square(const fp2& e0, const fp2& e1)
{
    fp2 t[3];
    fp2 c0, c1;
    t[0] = e0.square();
    t[1] = e1.square();
    t[2] = t[1].mulByNonResidue();
    c0 = t[2].add(t[0]);
    t[2] = e0.add(e1);
    t[2].squareAssign();
    t[2].subtractAssign(t[0]);
    c1 = t[2].subtract(t[1]);
    return {c0, c1};
}

fp12 fp12::inverse() const
{
    fp6 t[2];
    fp12 c;
    t[0] = c0.square();
    t[1] = c1.square();
    t[1] = t[1].mulByNonResidue();
    t[1] = t[0].subtract(t[1]);
    t[0] = t[1].inverse();
    c.c0 = c0.multiply(t[0]);
    t[0].multiplyAssign(c1);
    c.c1 = t[0].negate();
    return c;
}

void fp12::mulBy014Assign(const fp2& e0, const fp2& e1, const fp2& e4)
{
    fp6 t[3];
    fp2 t2;
    t[0] = c0.mulBy01(e0, e1);
    t[1] = c1.mulBy1(e4);
    t2 = e1.add(e4);
    t[2] = c1.add(c0);
    t[2].mulBy01Assign(e0, t2);
    t[2].subtractAssign(t[0]);
    c1 = t[2].subtract(t[1]);
    t[1] = t[1].mulByNonResidue();
    c0 = t[1].add(t[0]);
}

fp12 fp12::frobeniusMap(const uint64_t& power) const
{
    fp12 c;
    c.c0 = c0.frobeniusMap(power);
    c.c1 = c1.frobeniusMap(power);
    switch(power)
    {
        case 0:
        {
            return c;
        }
        case 6:
        {
            c.c1 = c.c1.negate();
            break;
        }
        default:
        {
            c.c1 = c.c1.mulByBaseField(frobeniusCoeffs12[power]);
            break;
        }
    }
    return c;
}

void fp12::frobeniusMapAssign(const uint64_t& power)
{
    c0.frobeniusMapAssign(power);
    c1.frobeniusMapAssign(power);
    switch(power)
    {
        case 0:
        {
            return;
        }
        case 6:
        {
            c1 = c1.negate();
            break;
        }
        default:
        {
            c1 = c1.mulByBaseField(frobeniusCoeffs12[power]);
            break;
        }
    }
}

const array<fp2, 12> fp12::frobeniusCoeffs12 = array<fp2, 12>({
    fp2({
        fp({0x760900000002fffd, 0xebf4000bc40c0002, 0x5f48985753c758ba, 0x77ce585370525745, 0x5c071a97a256ec6d, 0x15f65ec3fa80e493}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x07089552b319d465, 0xc6695f92b50a8313, 0x97e83cccd117228f, 0xa35baecab2dc29ee, 0x1ce393ea5daace4d, 0x08f2220fb0fb66eb}),
        fp({0xb2f66aad4ce5d646, 0x5842a06bfc497cec, 0xcf4895d42599d394, 0xc11b9cba40a8e8d0, 0x2e3813cbe5a0de89, 0x110eefda88847faf}),
    }),
    fp2({
        fp({0xecfb361b798dba3a, 0xc100ddb891865a2c, 0x0ec08ff1232bda8e, 0xd5c13cc6f1ca4721, 0x47222a47bf7b5c04, 0x0110f184e51c5f59}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x3e2f585da55c9ad1, 0x4294213d86c18183, 0x382844c88b623732, 0x92ad2afd19103e18, 0x1d794e4fac7cf0b9, 0x0bd592fc7d825ec8}),
        fp({0x7bcfa7a25aa30fda, 0xdc17dec12a927e7c, 0x2f088dd86b4ebef1, 0xd1ca2087da74d4a7, 0x2da2596696cebc1d, 0x0e2b7eedbbfd87d2}),
    }),
    fp2({
        fp({0x30f1361b798a64e8, 0xf3b8ddab7ece5a2a, 0x16a8ca3ac61577f7, 0xc26a2ff874fd029b, 0x3636b76660701c6e, 0x051ba4ab241b6160}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x3726c30af242c66c, 0x7c2ac1aad1b6fe70, 0xa04007fbba4b14a2, 0xef517c3266341429, 0x0095ba654ed2226b, 0x02e370eccc86f7dd}),
        fp({0x82d83cf50dbce43f, 0xa2813e53df9d018f, 0xc6f0caa53c65e181, 0x7525cf528d50fe95, 0x4a85ed50f4798a6b, 0x171da0fd6cf8eebd}),
    }),
    fp2({
        fp({0x43f5fffffffcaaae, 0x32b7fff2ed47fffd, 0x07e83a49a2e99d69, 0xeca8f3318332bb7a, 0xef148d1ea0f4c069, 0x040ab3263eff0206}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0xb2f66aad4ce5d646, 0x5842a06bfc497cec, 0xcf4895d42599d394, 0xc11b9cba40a8e8d0, 0x2e3813cbe5a0de89, 0x110eefda88847faf}),
        fp({0x07089552b319d465, 0xc6695f92b50a8313, 0x97e83cccd117228f, 0xa35baecab2dc29ee, 0x1ce393ea5daace4d, 0x08f2220fb0fb66eb}),
    }),
    fp2({
        fp({0xcd03c9e48671f071, 0x5dab22461fcda5d2, 0x587042afd3851b95, 0x8eb60ebe01bacb9e, 0x03f97d6e83d050d2, 0x18f0206554638741}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x7bcfa7a25aa30fda, 0xdc17dec12a927e7c, 0x2f088dd86b4ebef1, 0xd1ca2087da74d4a7, 0x2da2596696cebc1d, 0x0e2b7eedbbfd87d2}),
        fp({0x3e2f585da55c9ad1, 0x4294213d86c18183, 0x382844c88b623732, 0x92ad2afd19103e18, 0x1d794e4fac7cf0b9, 0x0bd592fc7d825ec8}),
    }),
    fp2({
        fp({0x890dc9e4867545c3, 0x2af322533285a5d5, 0x50880866309b7e2c, 0xa20d1b8c7e881024, 0x14e4f04fe2db9068, 0x14e56d3f1564853a}),
        fp({0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}),
    }),
    fp2({
        fp({0x82d83cf50dbce43f, 0xa2813e53df9d018f, 0xc6f0caa53c65e181, 0x7525cf528d50fe95, 0x4a85ed50f4798a6b, 0x171da0fd6cf8eebd}),
        fp({0x3726c30af242c66c, 0x7c2ac1aad1b6fe70, 0xa04007fbba4b14a2, 0xef517c3266341429, 0x0095ba654ed2226b, 0x02e370eccc86f7dd}),
    }),
});

} // namespace bls12_381
