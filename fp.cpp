#include "bls12_381.hpp"

fp::fp() : d{0, 0, 0, 0, 0, 0}
{
}

fp::fp(const array<uint64_t, 6>& d) : d{d[0], d[1], d[2], d[3], d[4], d[5]}
{
}

fp::fp(const fp& e) : d{e.d[0], e.d[1], e.d[2], e.d[3], e.d[4], e.d[5]}
{
}

fp fp::fromBytesBE(const span<const uint8_t, 48> in)
{
    fp e = fp(scalar::fromBytesBE<6>(in));
    if(!e.isValid()) throw invalid_argument("field element invalid!");
    return e.toMont();
}

fp fp::fromBytesLE(const span<const uint8_t, 48> in)
{
    fp e = fp(scalar::fromBytesLE<6>(in));
    if(!e.isValid()) throw invalid_argument("field element invalid!");
    return e.toMont();
}

void fp::toBytesBE(const span<uint8_t, 48> out) const
{
    scalar::toBytesBE<6>(fromMont().d, out);
}

void fp::toBytesLE(const span<uint8_t, 48> out) const
{
    scalar::toBytesLE<6>(fromMont().d, out);
}

array<uint8_t, 48> fp::toBytesBE() const
{
    return scalar::toBytesBE<6>(fromMont().d);
}

array<uint8_t, 48> fp::toBytesLE() const
{
    return scalar::toBytesLE<6>(fromMont().d);
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

int64_t fp::cmp(const fp& e) const
{
    return scalar::cmp<6>(d, e.d);
}

bool fp::equal(const fp& e) const
{
    return d[0] == e.d[0] && d[1] == e.d[1] && d[2] == e.d[2] && d[3] == e.d[3] && d[4] == e.d[4] && d[5] == e.d[5];
}

bool fp::sign() const
{
    return (fromMont().d[0] & 1) == 0;
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
    fp r2 = R2;
    _mul(&c, this, &r2);
    return c;
}

fp fp::fromMont() const
{
    fp c, b = fp({1, 0, 0, 0, 0, 0});
    _mul(&c, this, &b);
    return c;
}

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
    uint64_t z = 0;
    bool found = false;
    // Phase 1
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
            z += r.mul2();
        }
        else if(u.cmp(v) == 1)
        {
            _lsubAssign(&u, &v);
            u.div2(0);
            _laddAssign(&r, &s);
            s.mul2();
        }
        else
        {
            _lsubAssign(&v, &u);
            v.div2(0);
            _laddAssign(&s, &r);
            z += r.mul2();
        }
        k += 1;
    }

    if(!found)
    {
        return zero();
    }

    if(k < 381 || k > 381+384)
    {
        return zero();
    }

    if(r.cmp(MODULUS) != -1 || z > 0)
    {
        fp modulus = MODULUS;
        _lsubAssign(&r, &modulus);
    }
    u = MODULUS;
    _lsubAssign(&u, &r);

    // Phase 2
    for(uint64_t i = k; i < 384*2; i++)
    {
        _double(&u, &u);
    }
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
        0x0D0088F51CBFF34D,
        0x258DD3DB21A5D66B,
        0xB23BA5C279C2895F,
        0xB39869507B587B12,
        0x0F55FFFF58A9FFFF,
        0xDCFF7FFFFFFFD556
    };
    array<uint64_t, 6> yValue = toMont().d;
    
    uint64_t _, borrow;
    tie(_, borrow) = Sub64(yValue[0], halfQ[5], 0);     // If the element was smaller, the subtraction will underflow
    tie(_, borrow) = Sub64(yValue[1], halfQ[4], 0);     // producing a borrow value of 0xffff...ffff, otherwise it will
    tie(_, borrow) = Sub64(yValue[2], halfQ[3], 0);     // be zero. We create a Choice representing true if there was
    tie(_, borrow) = Sub64(yValue[3], halfQ[2], 0);     // overflow (and so this element is not lexicographically larger
    tie(_, borrow) = Sub64(yValue[4], halfQ[1], 0);     // than its negation) and then negate it.
    tie(_, borrow) = Sub64(yValue[5], halfQ[0], 0);
    
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
    0x1804000000015554,
    0x855000053ab00001,
    0x633cb57c253c276f,
    0x6e22d1ec31ebb502,
    0xd3916126f2d14ca2,
    0x17fbb8571a006596
});

const array<uint64_t, 4> fp::Q = {
    0xffffffff00000001,
    0x53bda402fffe5bfe,
    0x3339d80809a1d805,
    0x73eda753299d7d48
};

const array<uint64_t, 6> fp::pPlus1Over4 = {
    0xee7fbfffffffeaab,
    0x07aaffffac54ffff,
    0xd9cc34a83dac3d89,
    0xd91dd2e13ce144af,
    0x92c6e9ed90d2eb35,
    0x0680447a8e5ff9a6
};

const array<uint64_t, 6> fp::pMinus1Over2 = {
    0xdcff7fffffffd555,
    0x0f55ffff58a9ffff,
    0xb39869507b587b12,
    0xb23ba5c279c2895f,
    0x258dd3db21a5d66b,
    0x0d0088f51cbff34d
};

const array<uint64_t, 6> fp::pMinus3Over4 = {
    0xee7fbfffffffeaaa,
    0x07aaffffac54ffff,
    0xd9cc34a83dac3d89,
    0xd91dd2e13ce144af,
    0x92c6e9ed90d2eb35,
    0x0680447a8e5ff9a6
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

fp2 fp2::fromBytesBE(const span<const uint8_t, 96> in)
{
    fp c1 = fp::fromBytesBE(span<const uint8_t, 48>(&in[ 0], &in[48]));
    fp c0 = fp::fromBytesBE(span<const uint8_t, 48>(&in[48], &in[96]));
    return fp2({c0, c1});
}

fp2 fp2::fromBytesLE(const span<const uint8_t, 96> in)
{
    fp c0 = fp::fromBytesLE(span<const uint8_t, 48>(&in[ 0], &in[48]));
    fp c1 = fp::fromBytesLE(span<const uint8_t, 48>(&in[48], &in[96]));
    return fp2({c0, c1});
}

void fp2::toBytesBE(const span<uint8_t, 96> out) const
{
    memcpy(&out[ 0], &c1.toBytesBE()[0], 48);
    memcpy(&out[48], &c0.toBytesBE()[0], 48);
}

void fp2::toBytesLE(const span<uint8_t, 96> out) const
{
    memcpy(&out[ 0], &c0.toBytesLE()[0], 48);
    memcpy(&out[48], &c1.toBytesLE()[0], 48);
}

array<uint8_t, 96> fp2::toBytesBE() const
{
    array<uint8_t, 96> out;
    memcpy(&out[ 0], &c1.toBytesBE()[0], 48);
    memcpy(&out[48], &c0.toBytesBE()[0], 48);
    return out;
}

array<uint8_t, 96> fp2::toBytesLE() const
{
    array<uint8_t, 96> out;
    memcpy(&out[ 0], &c0.toBytesLE()[0], 48);
    memcpy(&out[48], &c1.toBytesLE()[0], 48);
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
    fp2 c, b = e;
    _add(&c.c0, &c0, &b.c0);
    _add(&c.c1, &c1, &b.c1);
    return c;
}

void fp2::addAssign(const fp2& e)
{
    fp2 b = e;
    _addAssign(&c0, &b.c0);
    _addAssign(&c1, &b.c1);
}

fp2 fp2::ladd(const fp2& e) const
{
    fp2 c, b = e;
    _ladd(&c.c0, &c0, &b.c0);
    _ladd(&c.c1, &c1, &b.c1);
    return c;
}

fp2 fp2::dbl() const
{
    fp2 c;
    _double(&c.c0, &c0);
    _double(&c.c1, &c1);
    return c;
}

void fp2::doubleAssign()
{
    _doubleAssign(&c0);
    _doubleAssign(&c1);
}

fp2 fp2::ldouble() const
{
    fp2 c;
    _ldouble(&c.c0, &c0);
    _ldouble(&c.c1, &c1);
    return c;
}

fp2 fp2::sub(const fp2& e) const
{
    fp2 c, b = e;
    _sub(&c.c0, &c0, &b.c0);
    _sub(&c.c1, &c1, &b.c1);
    return c;
}

void fp2::subAssign(const fp2& e)
{
    fp2 b = e;
    _subAssign(&c0, &b.c0);
    _subAssign(&c1, &b.c1);
}

fp2 fp2::neg() const
{
    fp2 c;
    _neg(&c.c0, &c0);
    _neg(&c.c1, &c1);
    return c;
}

fp2 fp2::mul(const fp2& e) const
{
    fp t[4];
    fp2 c, b = e;
    _mul(&t[1], &c0, &b.c0);
    _mul(&t[2], &c1, &b.c1);
    _add(&t[0], &c0, &c1);
    _add(&t[3], &b.c0, &b.c1);
    _sub(&c.c0, &t[1], &t[2]);
    _addAssign(&t[1], &t[2]);
    _mul(&t[0], &t[0], &t[3]);
    _sub(&c.c1, &t[0], &t[1]);
    return c;
}

void fp2::mulAssign(const fp2& e)
{
    fp t[4];
    fp2 b = e;
    _mul(&t[1], &c0, &b.c0);
    _mul(&t[2], &c1, &b.c1);
    _add(&t[0], &c0, &c1);
    _add(&t[3], &b.c0, &b.c1);
    _sub(&c0, &t[1], &t[2]);
    _addAssign(&t[1], &t[2]);
    _mul(&t[0], &t[0], &t[3]);
    _sub(&c1, &t[0], &t[1]);
}

fp2 fp2::square() const
{
    fp t[4];
    fp2 c;
    _ladd(&t[0], &c0, &c1);
    _sub(&t[1], &c0, &c1);
    _ldouble(&t[2], &c0);
    _mul(&c.c0, &t[0], &t[1]);
    _mul(&c.c1, &t[2], &c1);
    return c;
}

void fp2::squareAssign()
{
    fp t[4];
    _ladd(&t[0], &c0, &c1);
    _sub(&t[1], &c0, &c1);
    _ldouble(&t[2], &c0);
    _mul(&c0, &t[0], &t[1]);
    _mul(&c1, &t[2], &c1);
}

fp2 fp2::mulByNonResidue() const
{
    fp t[4];
    fp2 c;
    _sub(&t[0], &c0, &c1);
    _add(&c.c1, &c0, &c1);
    c.c0 = t[0];
    return c;
}

fp2 fp2::mulByB() const
{
    fp t[4];
    fp2 c;
    _double(&t[0], &c0);
    _double(&t[1], &c1);
    _doubleAssign(&t[0]);
    _doubleAssign(&t[1]);
    _sub(&c.c0, &t[0], &t[1]);
    _add(&c.c1, &t[0], &t[1]);
    return c;
}

fp2 fp2::inverse() const
{
    fp t[4];
    fp2 c;
    _square(&t[0], &c0);
    _square(&t[1], &c1);
    _addAssign(&t[0], &t[1]);
    t[0] = t[0].inverse();
    _mul(&c.c0, &c0, &t[0]);
    _mul(&t[0], &t[0], &c1);
    _neg(&c.c1, &t[0]);
    return c;
}

fp2 fp2::mulByFq(const fp& e) const
{
    fp2 c;
    fp b = e;
    _mul(&c.c0, &c0, &b);
    _mul(&c.c1, &c1, &b);
    return c;
}

fp2 fp2::frobeniusMap(const uint64_t& power) const
{
    fp2 c;
    c.c0 = c0;
    if(power%2 == 1)
    {
        _neg(&c.c1, &c1);
        return c;
    }
    c.c1 = c1;
    return c;
}

void fp2::frobeniusMapAssign(const uint64_t& power)
{
    if(power%2 == 1)
    {
        _neg(&c1, &c1);
    }
}

bool fp2::sqrt(fp2& c) const
{
    fp2 u, x0, a1, alpha;
    u = *this;
    a1 = this->exp(fp::pMinus3Over4);
    alpha = a1.square();
    alpha = alpha.mul(*this);
    x0 = a1.mul(*this);
    if(alpha.equal(fp2::negativeOne2))
    {
        _neg(&c.c0, &x0.c1);
        c.c1 = x0.c0;
        return true;
    }
    alpha = alpha.add(fp2::one());
    alpha = alpha.exp(fp::pMinus1Over2);
    c = alpha.mul(x0);
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
        0x43f5fffffffcaaae,
        0x32b7fff2ed47fffd,
        0x07e83a49a2e99d69,
        0xeca8f3318332bb7a,
        0xef148d1ea0f4c069,
        0x040ab3263eff0206
    }),
    fp({
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000,
        0x0000000000000000
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

fp6::fp6() : c0(fp2()), c1(fp2()), c2(fp2())
{
}

fp6::fp6(const array<fp2, 3>& e3) : c0(e3[0]), c1(e3[1]), c2(e3[2])
{
}

fp6::fp6(const fp6& e) : c0(e.c0), c1(e.c1), c2(e.c2)
{
}

fp6 fp6::fromBytesBE(const span<const uint8_t, 288> in)
{
    fp2 c2 = fp2::fromBytesBE(span<const uint8_t, 96>(&in[  0], &in[ 96]));
    fp2 c1 = fp2::fromBytesBE(span<const uint8_t, 96>(&in[ 96], &in[192]));
    fp2 c0 = fp2::fromBytesBE(span<const uint8_t, 96>(&in[192], &in[288]));
    return fp6({c0, c1, c2});
}

fp6 fp6::fromBytesLE(const span<const uint8_t, 288> in)
{
    fp2 c0 = fp2::fromBytesBE(span<const uint8_t, 96>(&in[  0], &in[ 96]));
    fp2 c1 = fp2::fromBytesBE(span<const uint8_t, 96>(&in[ 96], &in[192]));
    fp2 c2 = fp2::fromBytesBE(span<const uint8_t, 96>(&in[192], &in[288]));
    return fp6({c0, c1, c2});
}

void fp6::toBytesBE(const span<uint8_t, 288> out) const
{
    memcpy(&out[  0], &c2.toBytesBE()[0], 96);
    memcpy(&out[ 96], &c1.toBytesBE()[0], 96);
    memcpy(&out[192], &c0.toBytesBE()[0], 96);
}

void fp6::toBytesLE(const span<uint8_t, 288> out) const
{
    memcpy(&out[  0], &c0.toBytesBE()[0], 96);
    memcpy(&out[ 96], &c1.toBytesBE()[0], 96);
    memcpy(&out[192], &c2.toBytesBE()[0], 96);
}

array<uint8_t, 288> fp6::toBytesBE() const
{
    array<uint8_t, 288> out;
    memcpy(&out[  0], &c2.toBytesBE()[0], 96);
    memcpy(&out[ 96], &c1.toBytesBE()[0], 96);
    memcpy(&out[192], &c0.toBytesBE()[0], 96);
    return out;
}

array<uint8_t, 288> fp6::toBytesLE() const
{
    array<uint8_t, 288> out;
    memcpy(&out[  0], &c0.toBytesBE()[0], 96);
    memcpy(&out[ 96], &c1.toBytesBE()[0], 96);
    memcpy(&out[192], &c2.toBytesBE()[0], 96);
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
    fp6 c;
    c.c0 = c0.add(e.c0);
    c.c1 = c1.add(e.c1);
    c.c2 = c2.add(e.c2);
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
    fp6 c;
    c.c0 = c0.dbl();
    c.c1 = c1.dbl();
    c.c2 = c2.dbl();
    return c;
}

void fp6::doubleAssign()
{
    c0.doubleAssign();
    c1.doubleAssign();
    c2.doubleAssign();
}

fp6 fp6::sub(const fp6& e) const
{
    fp6 c;
    c.c0 = c0.sub(e.c0);
    c.c1 = c1.sub(e.c1);
    c.c2 = c2.sub(e.c2);
    return c;
}

void fp6::subAssign(const fp6& e)
{
    c0.subAssign(e.c0);
    c1.subAssign(e.c1);
    c2.subAssign(e.c2);
}

fp6 fp6::neg() const
{
    fp6 c;
    c.c0 = c0.neg();
    c.c1 = c1.neg();
    c.c2 = c2.neg();
    return c;
}

fp6 fp6::mul(const fp6& e) const
{
    fp2 t[6];
    fp6 c, b = e;
    t[0] = c0.mul(b.c0);
    t[1] = c1.mul(b.c1);
    t[2] = c2.mul(b.c2);
    t[3] = c1.add(c2);
    t[4] = b.c1.add(b.c2);
    t[3].mulAssign(t[4]);
    t[4] = t[1].add(t[2]);
    t[3].subAssign(t[4]);
    t[3] = t[3].mulByNonResidue();
    t[5] = t[0].add(t[3]);
    t[3] = c0.add(c1);
    t[4] = b.c0.add(b.c1);
    t[3].mulAssign(t[4]);
    t[4] = t[0].add(t[1]);
    t[3].subAssign(t[4]);
    t[4] = t[2].mulByNonResidue();
    c.c1 = t[3].add(t[4]);
    t[3] = c0.add(c2);
    t[4] = b.c0.add(b.c2);
    t[3].mulAssign(t[4]);
    t[4] = t[0].add(t[2]);
    t[3].subAssign(t[4]);
    c.c2 = t[1].add(t[3]);
    c.c0 = t[5];
    return c;
}

void fp6::mulAssign(const fp6& e)
{
    fp2 t[6];
    fp6 b = e;
    t[0] = c0.mul(b.c0);
    t[1] = c1.mul(b.c1);
    t[2] = c2.mul(b.c2);
    t[3] = c1.add(c2);
    t[4] = b.c1.add(b.c2);
    t[3].mulAssign(t[4]);
    t[4] = t[1].add(t[2]);
    t[3].subAssign(t[4]);
    t[3] = t[3].mulByNonResidue();
    t[5] = t[0].add(t[3]);
    t[3] = c0.add(c1);
    t[4] = b.c0.add(b.c1);
    t[3].mulAssign(t[4]);
    t[4] = t[0].add(t[1]);
    t[3].subAssign(t[4]);
    t[4] = t[2].mulByNonResidue();
    c1 = t[3].add(t[4]);
    t[3] = c0.add(c2);
    t[4] = b.c0.add(b.c2);
    t[3].mulAssign(t[4]);
    t[4] = t[0].add(t[2]);
    t[3].subAssign(t[4]);
    c2 = t[1].add(t[3]);
    c0 = t[5];
}

fp6 fp6::square() const
{
    fp2 t[6];
    fp6 c;
    t[0] = c0.square();
    t[1] = c0.mul(c1);
    t[1].doubleAssign();
    t[2] = c0.sub(c1);
    t[2].addAssign(c2);
    t[2].squareAssign();
    t[3] = c1.mul(c2);
    t[3].doubleAssign();
    t[4] = c2.square();
    t[5] = t[3].mulByNonResidue();
    c.c0 = t[0].add(t[5]);
    t[5] = t[4].mulByNonResidue();
    c.c1 = t[1].add(t[5]);
    t[1].addAssign(t[2]);
    t[1].addAssign(t[3]);
    t[0].addAssign(t[4]);
    c.c2 = t[1].sub(t[0]);
    return c;
}

void fp6::mulBy01Assign(const fp2& e0, const fp2& e1)
{
    fp2 t[6], b0 = e0, b1 = e1;
    t[0] = c0.mul(b0);
    t[1] = c1.mul(b1);
    t[5] = c1.add(c2);
    t[2] = b1.mul(t[5]);
    t[2].subAssign(t[1]);
    t[2] = t[2].mulByNonResidue();
    t[5] = c0.add(c2);
    t[3] = b0.mul(t[5]);
    t[3].subAssign(t[0]);
    c2 = t[3].add(t[1]);
    t[4] = b0.add(b1);
    t[5] = c0.add(c1);
    t[4].mulAssign(t[5]);
    t[4].subAssign(t[0]);
    c1 = t[4].sub(t[1]);
    c0 = t[2].add(t[0]);
}

fp6 fp6::mulBy01(const fp2& e0, const fp2& e1) const
{
    fp2 t[6], b0 = e0, b1 = e1;
    fp6 c;
    t[0] = c0.mul(b0);
    t[1] = c1.mul(b1);
    t[2] = c1.add(c2);
    t[2].mulAssign(b1);
    t[2].subAssign(t[1]);
    t[2] = t[2].mulByNonResidue();
    t[3] = c0.add(c2);
    t[3].mulAssign(b0);
    t[3].subAssign(t[0]);
    c.c2 = t[3].add(t[1]);
    t[4] = b0.add(b1);
    t[3] = c0.add(c1);
    t[4].mulAssign(t[3]);
    t[4].subAssign(t[0]);
    c.c1 = t[4].sub(t[1]);
    c.c0 = t[2].add(t[0]);
    return c;
}

fp6 fp6::mulBy1(const fp2& e1) const
{
    fp2 t[6], b1 = e1;
    fp6 c;
    t[0] = c2.mul(b1);
    c.c2 = c1.mul(b1);
    c.c1 = c0.mul(b1);
    c.c0 = t[0].mulByNonResidue();
    return c;
}

fp6 fp6::mulByNonResidue() const
{
    fp2 t[6];
    fp6 c;
    t[0] = c0;
    c.c0 = c2.mulByNonResidue();
    c.c2 = c1;
    c.c1 = t[0];
    return c;
}

fp6 fp6::mulByBaseField(const fp2& e) const
{
    fp2 b = e;
    fp6 c;
    c.c0 = c0.mul(b);
    c.c1 = c1.mul(b);
    c.c2 = c2.mul(b);
    return c;
}

fp6 fp6::inverse() const
{
    fp2 t[6];
    fp6 c;
    t[0] = c0.square();
    t[1] = c1.mul(c2);
    t[1] = t[1].mulByNonResidue();
    t[0].subAssign(t[1]);
    t[1] = c1.square();
    t[2] = c0.mul(c2);
    t[1].subAssign(t[2]);
    t[2] = c2.square();
    t[2] = t[2].mulByNonResidue();
    t[3] = c0.mul(c1);
    t[2].subAssign(t[3]);
    t[3] = c2.mul(t[2]);
    t[4] = c1.mul(t[1]);
    t[3].addAssign(t[4]);
    t[3] = t[3].mulByNonResidue();
    t[4] = c0.mul(t[0]);
    t[3].addAssign(t[4]);
    t[3] = t[3].inverse();
    c.c0 = t[0].mul(t[3]);
    c.c1 = t[2].mul(t[3]);
    c.c2 = t[1].mul(t[3]);
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
            _neg(&c.c0.c0, &c1.c1);
            c.c1.c1 = c1.c0;
            c.c2 = c2.neg();
            break;
        }
        default:
        {
            c.c1 = c.c1.mul(frobeniusCoeffs61[power%6]);
            c.c2 = c.c2.mul(frobeniusCoeffs62[power%6]);
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
    fp2 t[6];
    switch(power % 6)
    {
        case 0:
        {
            return;
        }
        case 3:
        {
            _neg(&t[0].c0, &c1.c1);
            c1.c1 = c1.c0;
            c1.c0 = t[0].c0;
            c2 = c2.neg();
            break;
        }
        default:
        {
            c1.mulAssign(frobeniusCoeffs61[power%6]);
            c2.mulAssign(frobeniusCoeffs62[power%6]);
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

fp12 fp12::fromBytesBE(const span<const uint8_t, 576> in)
{
    fp6 c1 = fp6::fromBytesBE(span<const uint8_t, 288>(&in[  0], &in[288]));
    fp6 c0 = fp6::fromBytesBE(span<const uint8_t, 288>(&in[288], &in[576]));
    return fp12({c0, c1});
}

fp12 fp12::fromBytesLE(const span<const uint8_t, 576> in)
{
    fp6 c0 = fp6::fromBytesBE(span<const uint8_t, 288>(&in[  0], &in[288]));
    fp6 c1 = fp6::fromBytesBE(span<const uint8_t, 288>(&in[288], &in[576]));
    return fp12({c0, c1});
}

void fp12::toBytesBE(const span<uint8_t, 576> out) const
{
    memcpy(&out[  0], &c1.toBytesBE()[0], 288);
    memcpy(&out[288], &c0.toBytesBE()[0], 288);
}

void fp12::toBytesLE(const span<uint8_t, 576> out) const
{
    memcpy(&out[  0], &c0.toBytesBE()[0], 288);
    memcpy(&out[288], &c1.toBytesBE()[0], 288);
}

array<uint8_t, 576> fp12::toBytesBE() const
{
    array<uint8_t, 576> out;
    memcpy(&out[  0], &c1.toBytesBE()[0], 288);
    memcpy(&out[288], &c0.toBytesBE()[0], 288);
    return out;
}

array<uint8_t, 576> fp12::toBytesLE() const
{
    array<uint8_t, 576> out;
    memcpy(&out[  0], &c0.toBytesBE()[0], 288);
    memcpy(&out[288], &c1.toBytesBE()[0], 288);
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
    fp12 c;
    c.c0 = c0.add(e.c0);
    c.c1 = c1.add(e.c1);
    return c;
}

fp12 fp12::dbl() const
{
    fp12 c;
    c.c0 = c0.dbl();
    c.c1 = c1.dbl();
    return c;
}

fp12 fp12::sub(const fp12& e) const
{
    fp12 c;
    c.c0 = c0.sub(e.c0);
    c.c1 = c1.sub(e.c1);
    return c;
}

fp12 fp12::neg() const
{
    fp12 c;
    c.c0 = c0.neg();
    c.c1 = c1.neg();
    return c;
}

fp12 fp12::conjugate() const
{
    fp12 c;
    c.c0 = c0;
    c.c1 = c1.neg();
    return c;
}

fp12 fp12::square() const
{
    fp6 t[5];
    fp12 c;
    t[0] = c0.add(c1);
    t[2] = c0.mul(c1);
    t[1] = c1.mulByNonResidue();
    t[1].addAssign(c0);
    t[3] = t[2].mulByNonResidue();
    t[0].mulAssign(t[1]);
    t[0].subAssign(t[2]);
    c.c0 = t[0].sub(t[3]);
    c.c1 = t[2].dbl();
    return c;
}

fp12 fp12::cyclotomicSquare() const
{
    fp2 t[9];
    fp12 c;
    tie(t[3], t[4]) = fp4Square(c0.c0, c1.c1);
    t[2] = t[3].sub(c0.c0);
    t[2].doubleAssign();
    c.c0.c0 = t[2].add(t[3]);
    t[2] = t[4].add(c1.c1);
    t[2].doubleAssign();
    c.c1.c1 = t[2].add(t[4]);
    tie(t[3], t[4]) = fp4Square(c1.c0, c0.c2);
    tie(t[5], t[6]) = fp4Square(c0.c1, c1.c2);
    t[2] = t[3].sub(c0.c1);
    t[2].doubleAssign();
    c.c0.c1 = t[2].add(t[3]);
    t[2] = t[4].add(c1.c2);
    t[2].doubleAssign();
    c.c1.c2 = t[2].add(t[4]);
    t[3] = t[6].mulByNonResidue();
    t[2] = t[3].add(c1.c0);
    t[2].doubleAssign();
    c.c1.c0 = t[2].add(t[3]);
    t[2] = t[5].sub(c0.c2);
    t[2].doubleAssign();
    c.c0.c2 = t[2].add(t[5]);
    return c;
}

fp12 fp12::mul(const fp12& e) const
{
    fp6 t[5];
    fp12 c, b = e;
    t[1] = c0.mul(b.c0);
    t[2] = c1.mul(b.c1);
    t[0] = t[1].add(t[2]);
    t[2] = t[2].mulByNonResidue();
    t[3] = t[1].add(t[2]);
    t[1] = c0.add(c1);
    t[2] = b.c0.add(b.c1);
    t[1].mulAssign(t[2]);
    c.c0 = t[3];
    c.c1 = t[1].sub(t[0]);
    return c;
}

void fp12::mulAssign(const fp12& e)
{
    fp6 t[5];
    fp12 b = e;
    t[1] = c0.mul(b.c0);
    t[2] = c1.mul(b.c1);
    t[0] = t[1].add(t[2]);
    t[2] = t[2].mulByNonResidue();
    t[3] = t[1].add(t[2]);
    t[1] = c0.add(c1);
    t[2] = b.c0.add(b.c1);
    t[1].mulAssign(t[2]);
    c0 = t[3];
    c1 = t[1].sub(t[0]);
}

tuple<fp2, fp2> fp12::fp4Square(const fp2& e0, const fp2& e1)
{
    fp2 t[9], a0 = e0, a1 = e1;
    fp2 c0, c1;
    t[0] = a0.square();
    t[1] = a1.square();
    t[2] = t[1].mulByNonResidue();
    c0 = t[2].add(t[0]);
    t[2] = a0.add(a1);
    t[2].squareAssign();
    t[2].subAssign(t[0]);
    c1 = t[2].sub(t[1]);
    return {c0, c1};
}

fp12 fp12::inverse() const
{
    fp6 t[5];
    fp12 c;
    t[0] = c0.square();
    t[1] = c1.square();
    t[1] = t[1].mulByNonResidue();
    t[1] = t[0].sub(t[1]);
    t[0] = t[1].inverse();
    c.c0 = c0.mul(t[0]);
    t[0].mulAssign(c1);
    c.c1 = t[0].neg();
    return c;
}

void fp12::mulBy014Assign(const fp2& e0, const fp2& e1, const fp2& e4)
{
    fp6 t[5];
    fp2 t2, a0 = e0, a1 = e1, a4 = e4;
    t[0] = c0.mulBy01(a0, a1);
    t[1] = c1.mulBy1(a4);
    t2 = a1.add(a4);
    t[2] = c1.add(c0);
    t[2].mulBy01Assign(a0, t2);
    t[2].subAssign(t[0]);
    c1 = t[2].sub(t[1]);
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
            c.c1 = c.c1.neg();
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
            c1 = c1.neg();
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
