#include <array>
#include <vector>
#include <random>
#include <iostream>

#include <bls12-381/bls12-381.hpp>

using namespace std;
using namespace bls12_381;

array<uint64_t, 4> random_scalar()
{
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dis;

    return {
        dis(gen) % fp::Q[0],
        dis(gen) % fp::Q[1],
        dis(gen) % fp::Q[2],
        dis(gen) % fp::Q[3]
    };
}

fp random_fe()
{
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dis;

    return fp({
        dis(gen) % 0xb9feffffffffaaab,
        dis(gen) % 0x1eabfffeb153ffff,
        dis(gen) % 0x6730d2a0f6b0f624,
        dis(gen) % 0x64774b84f38512bf,
        dis(gen) % 0x4b1ba7b6434bacd7,
        dis(gen) % 0x1a0111ea397fe69a
    });
}

fp2 random_fe2()
{
    return fp2({
        random_fe(),
        random_fe()
    });
}

fp6 random_fe6()
{
    return fp6({
        random_fe2(),
        random_fe2(),
        random_fe2()
    });
}

fp12 random_fe12()
{
    return fp12({
        random_fe6(),
        random_fe6()
    });
}

g1 random_g1()
{
    array<uint64_t, 4> k = random_scalar();
    return g1::one().scale(k);
}

g2 random_g2()
{
    array<uint64_t, 4> k = random_scalar();
    return g2::one().scale(k);
}

void TestScalar()
{
    for(int i = 0; i < 100; i++)
    {
        array<uint64_t, 4> s = random_scalar();
        array<uint64_t, 4> r = scalar::fromBytesLE<4>(scalar::toBytesLE(s));
        if(s != r) throw invalid_argument("LE: r != s");

        r = scalar::fromBytesBE<4>(scalar::toBytesBE(s));
        if(s != r) throw invalid_argument("BE: r != s");
    }
}

void TestFieldElementValidation()
{
    fp zero = fp::zero();
    if(!zero.isValid())
    {
        throw invalid_argument("zero must be valid");
    }
    fp one = fp::one();
    if(!one.isValid())
    {
        throw invalid_argument("one must be valid");
    }
    if(fp(fp::MODULUS).isValid())
    {
        throw invalid_argument("modulus must be invalid");
    }
    fp n = fp::MODULUS;
    n.d[0] += 1;
    if(n.isValid())
    {
        throw invalid_argument("number greater than modulus must be invalid");
    }
}

void TestFieldElementEquality()
{
    // fe
    fp zero = fp::zero();
    if(!zero.equal(zero))
    {
        throw invalid_argument("0 == 0");
    }
    fp one = fp::one();
    if(!one.equal(one))
    {
        throw invalid_argument("1 == 1");
    }
    fp a = random_fe();
    if(!a.equal(a))
    {
        throw invalid_argument("a == a");
    }
    fp b;
    _add(&b, &a, &one);
    if(a.equal(b))
    {
        throw invalid_argument("a != a + 1");
    }
    // fp2
    fp2 zero2 = fp2::zero();
    if(!zero2.equal(zero2))
    {
        throw invalid_argument("0 == 0");
    }
    fp2 one2 = fp2::one();
    if(!one2.equal(one2))
    {
        throw invalid_argument("1 == 1");
    }
    fp2 a2 = random_fe2();
    if(!a2.equal(a2))
    {
        throw invalid_argument("a == a");
    }
    fp2 b2;
    b2 = a2.add(one2);
    if(a2.equal(b2))
    {
        throw invalid_argument("a != a + 1");
    }
    // fp6
    fp6 zero6 = fp6::zero();
    if(!zero6.equal(zero6))
    {
        throw invalid_argument("0 == 0");
    }
    fp6 one6 = fp6::one();
    if(!one6.equal(one6))
    {
        throw invalid_argument("1 == 1");
    }
    fp6 a6 = random_fe6();
    if(!a6.equal(a6))
    {
        throw invalid_argument("a == a");
    }
    fp6 b6;
    b6 = a6.add(one6);
    if(a6.equal(b6))
    {
        throw invalid_argument("a != a + 1");
    }
    // fp12
    fp12 zero12 = fp12::zero();
    if(!zero12.equal(zero12))
    {
        throw invalid_argument("0 == 0");
    }
    fp12 one12 = fp12::one();
    if(!one12.equal(one12))
    {
        throw invalid_argument("1 == 1");
    }
    fp12 a12 = random_fe12();
    if(!a12.equal(a12))
    {
        throw invalid_argument("a == a");
    }
    fp12 b12;
    b12 = a12.add(one12);
    if(a12.equal(b12))
    {
        throw invalid_argument("a != a + 1");
    }
}

void TestFieldElementArithmeticCornerCases() {
    const char* testVectorInput[] = {
        "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
        "1A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAA", // p-1
        "1A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAB", // p
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    };

    const char* testVectorExpectedSquare[] = {
        "NA",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "NA",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    };

    const char* testVectorExpectedAdd[] = {
        "NA",
        "1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffaaa9",
        "NA",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    };

    auto testSqureMul = [](const char* in, const char* expectedSquare, const char* expectedAdd) {
        // Input should be convert to Montgomery form, so "raw" = false
       auto input = fp::fromBytesBE(hexToBytes<48>(in), { .check_valid = true, .to_mont = true });

        if (0 == strcmp("NA", expectedSquare)) {
            if (input) {
                throw invalid_argument("input should be invalid but not");
            }
            return;
        }

        // Expected result will be compared against numbers converted back from Montgomery form, so "raw" = true
        auto fpExpectedSquare = fp::fromBytesBE(hexToBytes<48>(expectedSquare), { .check_valid=false, .to_mont=false });
        auto fpExpectedAdd = fp::fromBytesBE(hexToBytes<48>(expectedAdd), { .check_valid = false, .to_mont = false });

        fp s,m,a,d;
        fp s1,m1,a1,d1;

        _square(&s, &*input);
        _multiply(&m, &*input, &*input);
        _add(&a, &*input, &*input);
        _double(&d, &*input);

        s1 = input->square();
        m1 = input->multiply(*input);
        a1 = input->add(*input);
        d1 = input->dbl();

        if(!s.equal(s1)) {
            throw invalid_argument("_square != fp::square");
        }
        if(!m.equal(m1)) {
            throw invalid_argument("_multiply != fp::multiply");
        }
        if(!a.equal(a1)) {
            throw invalid_argument("_add != fp::add");
        }
        if(!d.equal(d1)) {
            throw invalid_argument("_double != fp::dbl");
        }

        s = s.fromMont();
        m = m.fromMont();
        a = a.fromMont();
        d = d.fromMont();

        if(!s.equal(m))
        {
            throw invalid_argument("square != mul self");
        }

        if(!s.equal(*fpExpectedSquare))
        {
            throw invalid_argument("square != expected");
        }

        if(!a.equal(d))
        {
            throw invalid_argument("double != add self");
        }

        if(!a.equal(*fpExpectedAdd))
        {
            throw invalid_argument("add != expected");
        }

    };

    for (size_t i = 0; i < sizeof(testVectorInput) / sizeof(const char*); ++i) {
        testSqureMul(testVectorInput[i], testVectorExpectedSquare[i], testVectorExpectedAdd[i]);
    }
}

template<class T> void TestHelperMultiplySquare(const T input) {
    T s,m;
    s = input.square();
    m = input.multiply(input);

    T sa,ma;
    sa = input;
    sa.squareAssign();
    ma = input;
    ma.multiplyAssign(ma);

    if(!s.equal(sa))
    {
        throw invalid_argument("square != squareAssign");
    }

    if(!m.equal(ma))
    {
        throw invalid_argument("multiply != multiplyAssign");
    }

    if(!s.equal(m))
    {
        throw invalid_argument("square != mul self");
    }
}

template<class T> void TestHelperAddSubtractDouble(const T input) {
    T a,d;
    a = input.add(input);
    d = input.dbl();

    T aa,da;
    aa = input;
    aa.addAssign(aa);
    da = input;
    da.doubleAssign();

    if(!a.equal(aa))
    {
        throw invalid_argument("add != addAssign");
    }

    if(!d.equal(da))
    {
        throw invalid_argument("dbl != doubleAssign");
    }

    if(!a.equal(d))
    {
        throw invalid_argument("double != add self");
    }

    T s = input.subtract(input);
    T sa = input;
    sa.subtractAssign(sa);

    if(!s.equal(sa))
    {
        throw invalid_argument("subtract != subtractAssign");
    }

    if(!s.isZero())
    {
        throw invalid_argument("zero != sub self");
    }
}

void TestArithmeticOpraters() {
    for(int i = 0; i < 100; i++)
    {
        TestHelperMultiplySquare(random_fe());
        TestHelperMultiplySquare(random_fe2());
        TestHelperMultiplySquare(random_fe6());
        TestHelperMultiplySquare(random_fe12());

        TestHelperAddSubtractDouble(random_fe());
        TestHelperAddSubtractDouble(random_fe2());
        TestHelperAddSubtractDouble(random_fe6());
        TestHelperAddSubtractDouble(random_fe12());

        TestHelperAddSubtractDouble(random_g1());
        TestHelperAddSubtractDouble(random_g2());
    }
}

void TestSqrt() {
    for (int i = 0; i < 100; ++ i) {
        fp a = random_fe();
        fp as = a.square();
        fp asqrt;
        if (as.sqrt(asqrt)) {
            if(!as.equal(asqrt.square()))
            {
                throw invalid_argument("sqrt(fp).square != fp");
            }
            if(!a.equal(asqrt) && !a.equal(asqrt.negate()))
            {
                throw invalid_argument("fp!= sqrt(fp.square)");
            }
        } else {
            throw invalid_argument("failed to find sqrt for fp");
        }
    }
    
    for (int i = 0; i < 100; ++ i) {
        fp2 a = random_fe2();
        fp2 as = a.square();
        fp2 asqrt;
        if (as.sqrt(asqrt)) {
            if(!as.equal(asqrt.square()))
            {
                throw invalid_argument("sqrt(fp2).square != fp2");
            }
            if(!a.equal(asqrt) && !a.equal(asqrt.negate()))
            {
                throw invalid_argument("fp2 != sqrt(fp2.square)");
            }
        } else {
            throw invalid_argument("failed to find sqrt for fp2");
        }
    }

}

void TestInverse() {
    if (!fp::one().inverse().equal(fp::one())) {
        throw invalid_argument("1^-1 != 1");
    }

    auto two = fp::one().dbl();
    if (!two.multiply(two.inverse()).equal(fp::one())) {
        throw invalid_argument("2 * 2^-1 != 1");
    }

    auto pminus1 = *fp::fromBytesBE(hexToBytes<48>("1A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAA"), { .check_valid = false, .to_mont = false });
    if (!pminus1.multiply(pminus1.inverse()).equal(fp::one())) {
        throw invalid_argument("(p-1) * (p-1)^-1 != 1");
    }

    for (int i = 0; i < 100; ++ i) {
        fp a = random_fe();
        auto b = a.inverse();
        if (!a.multiply(b).equal(fp::one())) {
            throw invalid_argument("fp * fp^-1 != 1");
        }
    }

    for (int i = 0; i < 100; ++ i) {
        fp2 a = random_fe2();
        auto b = a.inverse();
        if (!a.multiply(b).equal(fp2::one())) {
            throw invalid_argument("fp2 * fp2^-1 != 1");
        }
    }

    for (int i = 0; i < 100; ++ i) {
        fp6 a = random_fe6();
        auto b = a.inverse();
        if (!a.multiply(b).equal(fp6::one())) {
            throw invalid_argument("fp * fp^-1 != 1");
        }
    }

    for (int i = 0; i < 100; ++ i) {
        fp12 a = random_fe12();
        auto b = a.inverse();
        if (!a.multiply(b).equal(fp12::one())) {
            throw invalid_argument("fp * fp^-1 != 1");
        }
    }
}

void TestMod() {

    const char* testVectorInput[] = {
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "000000000000000000000000000000001A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAA", // p-1
        "000000000000000000000000000000001A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAB", // p
        "000000000000000000000000000000001A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAC", // p+1
    };

    const char* testVectorExpected[] = {
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "1A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAA",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
    };

    for (size_t i = 0; i < sizeof(testVectorInput)/sizeof(const char*); ++ i) {
        auto s = hexToBytes<64>(testVectorInput[i]);
        auto k = scalar::fromBytesBE<8>(s);
        fp r = fp::modPrime<8>(k);
        auto fpExpected = fp::fromBytesBE(hexToBytes<48>(testVectorExpected[i]), { .check_valid = false, .to_mont = true });
        if(!fpExpected->equal(r))
        {
            throw invalid_argument("r != expected for Mod");
        }
        
    }
}

void TestExp() {

    const char* testVectorInput[] = {
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "000000000000000000000000000000001A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAA", // p-1
    };

    const char* testVectorInput2[] = {
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "1A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAA", // p-1
    };

    const char* testVectorExpected[] = {
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001",
    };

    for (size_t i = 0; i < sizeof(testVectorInput)/sizeof(const char*); ++ i) {
        auto s = hexToBytes<64>(testVectorInput[i]);
        auto b = fp::fromBytesBE(hexToBytes<48>(testVectorInput2[i]), { .check_valid = false, .to_mont = true });
        auto k = scalar::fromBytesBE<8>(s);
        fp r = b->exp(k);
        auto fpExpected = fp::fromBytesBE(hexToBytes<48>(testVectorExpected[i]), { .check_valid = false, .to_mont = true });
        if(!fpExpected->equal(r))
        {
            throw invalid_argument("r != expected for Exp");
        }
        
    }
}

void TestFieldElementHelpers()
{
    // fe
    fp zero = fp::zero();
    if(!zero.isZero())
    {
        throw invalid_argument("'zero' is not zero");
    }
    fp one = fp::one();
    if(!one.isOne())
    {
        throw invalid_argument("'one' is not one");
    }
    fp odd = fp({1, 0, 0, 0, 0, 0});
    if(!odd.isOdd())
    {
        throw invalid_argument("1 must be odd");
    }
    if(odd.isEven())
    {
        throw invalid_argument("1 must not be even");
    }
    fp even = fp({2, 0, 0, 0, 0, 0});
    if(!even.isEven())
    {
        throw invalid_argument("2 must be even");
    }
    if(even.isOdd())
    {
        throw invalid_argument("2 must not be odd");
    }
    // fp2
    fp2 zero2 = fp2::zero();
    if(!zero2.isZero())
    {
        throw invalid_argument("'zero' is not zero, 2");
    }
    fp2 one2 = fp2::one();
    if(!one2.isOne())
    {
        throw invalid_argument("'one' is not one, 2");
    }
    // fp6
    fp6 zero6 = fp6::zero();
    if(!zero6.isZero())
    {
        throw invalid_argument("'zero' is not zero, 6");
    }
    fp6 one6 = fp6::one();
    if(!one6.isOne())
    {
        throw invalid_argument("'one' is not one, 6");
    }
    // fp12
    fp12 zero12 = fp12::zero();
    if(!zero12.isZero())
    {
        throw invalid_argument("'zero' is not zero, 12");
    }
    fp12 one12 = fp12::one();
    if(!one12.isOne())
    {
        throw invalid_argument("'one' is not one, 12");
    }
}

const size_t fuz = 10;

void TestFieldElementSerialization()
{
    {
        array<uint8_t, 48> in({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
        fp e = fp::fromBytesBE(in).value();
        if(!e.isZero())
        {
            throw invalid_argument("bad serialization");
        }
        if(in != e.toBytesBE())
        {
            throw invalid_argument("bad serialization");
        }
    }
    {
        for(size_t i = 0; i < fuz; i++)
        {
            fp a = random_fe();
            fp b = fp::fromBytesBE(a.toBytesBE()).value();
            if(!a.equal(b))
            {
                throw invalid_argument("BE: bad serialization");
            }
            if( a != b )
            {
               throw invalid_argument("BE: bad serialization");
            }
            if(!(a == b))
            {
               throw invalid_argument("BE: bad serialization");
            }
        }
        for(size_t i = 0; i < fuz; i++)
        {
            fp a = random_fe();
            fp b = fp::fromBytesLE(a.toBytesLE()).value();
            if(!a.equal(b))
            {
                throw invalid_argument("LE: bad serialization");
            }
        }
    }
    {
        for(size_t i = 0; i < fuz; i++)
        {
            fp a = random_fe();
            fp b = fp::fromBytesBE(hexToBytes<48>(bytesToHex<48>(a.toBytesBE()))).value();
            if(!a.equal(b))
            {
                throw invalid_argument("string, BE: bad encoding or decoding");
            }
            b = fp::fromBytesLE(hexToBytes<48>(bytesToHex<48>(a.toBytesLE()))).value();
            if(!a.equal(b))
            {
                throw invalid_argument("string, LE: bad encoding or decoding");
            }
        }
    }
}

void TestFieldElementByteInputs()
{
    fp zero = fp::zero();
    array<uint8_t, 48> in = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    fp a = fp::fromBytesBE(in).value();
    if(!a.equal(zero))
    {
        throw invalid_argument("bad serialization");
    }
}

///////////////////////////////////////////////////////////

void TestG1Serialization()
{
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        array<uint8_t, 144> buf = a.toJacobianBytesBE();
        g1 b = g1::fromJacobianBytesBE(buf).value();
        if(a != b)
        {
            throw invalid_argument("g1, jacobian: bad serialization from/to");
        }
        if(!(a == b))
        {
            throw invalid_argument("g1, jacobian: bad serialization from/to");
        }
        if(!a.equal(b))
        {
            throw invalid_argument("g1, jacobian: bad serialization from/to");
        }
    }
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        array<uint8_t, 96> buf = a.toAffineBytesBE();
        g1 b = g1::fromAffineBytesBE(buf).value();
        if(!a.equal(b))
        {
            throw invalid_argument("g1, affine: bad serialization from/to");
        }
    }
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        array<uint8_t, 48> buf = a.toCompressedBytesBE();
        g1 b = g1::fromCompressedBytesBE(buf).value();
        if(!a.equal(b))
        {
            throw invalid_argument("g1, compressed: bad serialization from/to");
        }
    }
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        array<uint8_t, 144> buf = a.toJacobianBytesLE();
        g1 b = g1::fromJacobianBytesLE(buf).value();
        if(!a.equal(b))
        {
            throw invalid_argument("g1, jacobian, LE: bad serialization from/to");
        }
    }
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        array<uint8_t, 96> buf = a.toAffineBytesLE();
        g1 b = g1::fromAffineBytesLE(buf).value();
        if(!a.equal(b))
        {
            throw invalid_argument("g1, affine, LE: bad serialization from/to");
        }
    }
}

void TestG1SerializationGarbage() {
    array<uint8_t, 144> buf;
    buf.fill(0xff);
    for (int i = 0 ; i < 4; ++i ) {
        auto a = g1::fromJacobianBytesBE(buf, { .check_valid = i<2, .to_mont = !(i%2) });
        if(a)
        {
            throw invalid_argument("g1, jacobianBE: serialization not catching invalid input");
        }
        auto b = g1::fromJacobianBytesLE(buf, { .check_valid = i<2, .to_mont = !(i%2) });
        if(b)
        {
            throw invalid_argument("g1, jacobianLE: serialization not catching invalid input");
        }
    }
       
    for (int i = 0 ; i < 4; ++i ) {
        auto a = g1::fromAffineBytesBE(std::span<const uint8_t, 96>{buf.begin(),96}, { .check_valid = i<2, .to_mont = !(i%2) });
        if(a)
        {
            throw invalid_argument("g1, affineBE: serialization not catching invalid input");
        }
        auto b = g1::fromAffineBytesLE(std::span<const uint8_t, 96>{buf.begin(),96}, { .check_valid = i<2, .to_mont = !(i%2) });
        if(b)
        {
            throw invalid_argument("g1, affineLE: serialization not catching invalid input");
        }
    }
}

void TestG1IsOnCurve()
{
    g1 zero = g1::zero();
    if(!zero.isOnCurve())
    {
        throw invalid_argument("zero must be on curve");
    }
    fp one = fp::one();
    g1 p = g1({one, one, one});
    if(p.isOnCurve())
    {
        throw invalid_argument("(1, 1) is not on curve");
    }
}

void TestG1AdditiveProperties()
{
    g1 t0, t1;
    g1 zero = g1::zero();
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        g1 b = random_g1();
        t0 = a.add(zero);
        if(!t0.equal(a))
        {
            throw invalid_argument("a + 0 == a");
        }
        t0 = zero.add(zero);
        if(!t0.equal(zero))
        {
            throw invalid_argument("0 + 0 == 0");
        }
        t0 = a.subtract(zero);
        if(!t0.equal(a))
        {
            throw invalid_argument("a - 0 == a");
        }
        t0 = zero.subtract(zero);
        if(!t0.equal(zero))
        {
            throw invalid_argument("0 - 0 == 0");
        }
        t0 = zero.negate();
        if(!t0.equal(zero))
        {
            throw invalid_argument("- 0 == 0");
        }
        t0 = zero.subtract(a);
        t0 = t0.negate();
        if(!t0.equal(a))
        {
            throw invalid_argument(" - (0 - a) == a");
        }
        t0 = zero.dbl();
        if(!t0.equal(zero))
        {
            throw invalid_argument("2 * 0 == 0");
        }
        t0 = a.dbl();
        t0 = t0.subtract(a);
        if(!t0.equal(a) || !t0.isOnCurve())
        {
            throw invalid_argument(" (2 * a) - a == a");
        }
        t0 = a.add(b);
        t1 = b.add(a);
        if(!t0.equal(t1))
        {
            throw invalid_argument("a + b == b + a");
        }
        t0 = a.subtract(b);
        t1 = b.subtract(a);
        t1 = t1.negate();
        if(!t0.equal(t1))
        {
            throw invalid_argument("a - b == - ( b - a )");
        }
        g1 c = random_g1();
        t0 = a.add(b);
        t0 = t0.add(c);
        t1 = a.add(c);
        t1 = t1.add(b);
        if(!t0.equal(t1))
        {
            throw invalid_argument("(a + b) + c == (a + c ) + b");
        }
        t0 = a.subtract(b);
        t0 = t0.subtract(c);
        t1 = a.subtract(c);
        t1 = t1.subtract(b);
        if(!t0.equal(t1))
        {
            throw invalid_argument("(a - b) - c == (a - c) -b");
        }
    }
}

void TestG1MultiplicativePropertiesExpected()
{
    struct testdata {
        g1 g;
        array<uint64_t, 4> s1;
        array<uint64_t, 4> s2;
    };
    array<testdata, 1> tv = {
        testdata {
            g1({
                fp({3417891934972824866UL, 15033698923095482284UL, 4308006749155089476UL, 12106291570202121725UL, 2599277836029828640UL, 312782107632090658UL}),
                fp({5205587738984789115UL, 5067874624690748468UL, 13190631992216725609UL, 11345201024832610071UL, 14051629697167006557UL, 393909533259831005UL}),
                fp({3721446142051153446UL, 12426141085104671205UL, 9131026636729359854UL, 3777282351912617648UL, 11898443449281733652UL, 518350083385915957UL})
            }),
            {709522381180683898UL, 6180605729922878966UL, 17818642988822031089UL, 2436448532036587648UL},
            {13638846313248189432UL, 13997190380171701873UL, 13650465545368734818UL, 6581793109286319586UL}
        }
    };

    g1 t0, t1;
    g1 zero = g1::zero();
    for(uint64_t i = 0; i < 1; i++)
    {
        g1 a = tv[i].g;
        array<uint64_t, 4> s1 = tv[i].s1;
        array<uint64_t, 4> s2 = tv[i].s2;
        array<uint64_t, 10> s3;
        array<uint64_t, 4> sone = {1, 0, 0, 0};
        t0 = zero.scale(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == 0");
        }
        t0 = a.scale(sone);
        if(!t0.equal(a))
        {
            throw invalid_argument(" a ^ 1 == a");
        }
        t0 = zero.scale(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == a");
        }
        t0 = a.scale(s1);
        t0 = t0.scale(s2);
        s3 = scalar::multiply<10, 4, 4>(s1, s2);
        t1 = a.scale(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument("G1: (a ^ s1) ^ s2 == a ^ (s1 * s2)");
        }
        t0 = a.scale(s1);
        t1 = a.scale(s2);
        t0 = t0.add(t1);
        s3 = scalar::add<10, 4, 4>(s1, s2);
        t1 = a.scale(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument(" (a ^ s1) + (a ^ s2) == a ^ (s1 + s2)");
        }
    }
}

void TestG1MultiplicativeProperties()
{
    g1 t0, t1;
    g1 zero = g1::zero();
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        array<uint64_t, 4> s1 = random_scalar();
        array<uint64_t, 4> s2 = random_scalar(); 
        array<uint64_t, 10> s3;
        array<uint64_t, 4> sone = {1, 0, 0, 0};
        t0 = zero.scale(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == 0");
        }
        t0 = a.scale(sone);
        if(!t0.equal(a))
        {
            throw invalid_argument(" a ^ 1 == a");
        }
        t0 = zero.scale(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == a");
        }
        t0 = a.scale(s1);
        t0 = t0.scale(s2);
        s3 = scalar::multiply<10, 4, 4>(s1, s2);
        t1 = a.scale(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument("G1: (a ^ s1) ^ s2 == a ^ (s1 * s2)");
        }
        t0 = a.scale(s1);
        t1 = a.scale(s2);
        t0 = t0.add(t1);
        s3 = scalar::add<10, 4, 4>(s1, s2);
        t1 = a.scale(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument(" (a ^ s1) + (a ^ s2) == a ^ (s1 + s2)");
        }
    }
}

void TestG1WeightedSumExpected()
{
    g1 one = g1::one();
    vector<array<uint64_t, 4>> scalars = {
        {2, 0, 0, 0},
        {3, 0, 0, 0}
    };
    vector<g1> bases = {one, one};
    g1 expected, result;
    expected = one.scale<1>({5});
    result = g1::weightedSum(bases, scalars);
    if(!expected.equal(result))
    {
        throw invalid_argument("TestG1WeightedSumExpected: bad multi-exponentiation");
    }
}

void TestG1WeightedSumBatch()
{
    const auto doTest = [](int64_t n) {
        vector<array<uint64_t, 4>> scalars;
        vector<g1> bases;

        for(int64_t i = 0; i < n; i++)
        {
            scalars.push_back(random_scalar());
            bases.push_back(random_g1());
        }

        g1 expected, tmp;
        for(int64_t i = 0; i < n; i++)
        {
            tmp = bases[i].scale(scalars[i]);
            expected = expected.add(tmp);
        }
        g1 result = g1::weightedSum(bases, scalars);
        if(!expected.equal(result))
        {
            throw invalid_argument("bad G1 weighted sum");
        }
    };

    doTest(0);
    doTest(1);
    doTest(2);
    doTest(31);
    doTest(32);
    doTest(33);
    doTest(63);
    doTest(64);
    doTest(65);
    doTest(511);
    doTest(512);
    doTest(513);
}

void TestG1MapToCurve()
{
    struct pair
    {
        array<uint8_t, 48> u;
        g1 expected;
    };
    array<pair, 5> pairs = {
        pair{
            array<uint8_t, 48>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            g1({fp::fromBytesBE(hexToBytes<48>("11a9a0372b8f332d5c30de9ad14e50372a73fa4c45d5f2fa5097f2d6fb93bcac592f2e1711ac43db0519870c7d0ea415")).value(), fp::fromBytesBE(hexToBytes<48>("092c0f994164a0719f51c24ba3788de240ff926b55f58c445116e8bc6a47cd63392fd4e8e22bdf9feaa96ee773222133")).value(), fp::one()})
        },
        pair{
            array<uint8_t, 48>{0x07, 0xfd, 0xf4, 0x9e, 0xa5, 0x8e, 0x96, 0x01, 0x5d, 0x61, 0xf6, 0xb5, 0xc9, 0xd1, 0xc8, 0xf2, 0x77, 0x14, 0x6a, 0x53, 0x3a, 0xe7, 0xfb, 0xca, 0x2a, 0x8e, 0xf4, 0xc4, 0x10, 0x55, 0xcd, 0x96, 0x1f, 0xbc, 0x6e, 0x26, 0x97, 0x9b, 0x55, 0x54, 0xe4, 0xb4, 0xf2, 0x23, 0x30, 0xc0, 0xe1, 0x6d},
            g1({fp::fromBytesBE(hexToBytes<48>("1223effdbb2d38152495a864d78eee14cb0992d89a241707abb03819a91a6d2fd65854ab9a69e9aacb0cbebfd490732c")).value(), fp::fromBytesBE(hexToBytes<48>("0f925d61e0b235ecd945cbf0309291878df0d06e5d80d6b84aa4ff3e00633b26f9a7cb3523ef737d90e6d71e8b98b2d5")).value(), fp::one()})
        },
        pair{
            array<uint8_t, 48>{0x12, 0x75, 0xab, 0x3a, 0xdb, 0xf8, 0x24, 0xa1, 0x69, 0xed, 0x4b, 0x1f, 0xd6, 0x69, 0xb4, 0x9c, 0xf4, 0x06, 0xd8, 0x22, 0xf7, 0xfe, 0x90, 0xd6, 0xb2, 0xf8, 0xc6, 0x01, 0xb5, 0x34, 0x84, 0x36, 0xf8, 0x97, 0x61, 0xbb, 0x1a, 0xd8, 0x9a, 0x6f, 0xb1, 0x13, 0x7c, 0xd9, 0x18, 0x10, 0xe5, 0xd2},
            g1({fp::fromBytesBE(hexToBytes<48>("179d3fd0b4fb1da43aad06cea1fb3f828806ddb1b1fa9424b1e3944dfdbab6e763c42636404017da03099af0dcca0fd6")).value(), fp::fromBytesBE(hexToBytes<48>("0d037cb1c6d495c0f5f22b061d23f1be3d7fe64d3c6820cfcd99b6b36fa69f7b4c1f4addba2ae7aa46fb25901ab483e4")).value(), fp::one()})
        },
        pair{
            array<uint8_t, 48>{0x0e, 0x93, 0xd1, 0x1d, 0x30, 0xde, 0x6d, 0x84, 0xb8, 0x57, 0x88, 0x27, 0x85, 0x6f, 0x5c, 0x05, 0xfe, 0xef, 0x36, 0x08, 0x3e, 0xef, 0x0b, 0x7b, 0x26, 0x3e, 0x35, 0xec, 0xb9, 0xb5, 0x6e, 0x86, 0x29, 0x96, 0x14, 0xa0, 0x42, 0xe5, 0x7d, 0x46, 0x7f, 0xa2, 0x09, 0x48, 0xe8, 0x56, 0x49, 0x09},
            g1({fp::fromBytesBE(hexToBytes<48>("15aa66c77eded1209db694e8b1ba49daf8b686733afaa7b68c683d0b01788dfb0617a2e2d04c0856db4981921d3004af")).value(), fp::fromBytesBE(hexToBytes<48>("0952bb2f61739dd1d201dd0a79d74cda3285403d47655ee886afe860593a8a4e51c5b77a22d2133e3a4280eaaaa8b788")).value(), fp::one()})
        },
        pair{
            array<uint8_t, 48>{0x01, 0x5a, 0x41, 0x48, 0x11, 0x55, 0xd1, 0x70, 0x74, 0xd2, 0x0b, 0xe6, 0xd8, 0xec, 0x4d, 0x46, 0x63, 0x2a, 0x51, 0x52, 0x1c, 0xd9, 0xc9, 0x16, 0xe2, 0x65, 0xbd, 0x9b, 0x47, 0x34, 0x3b, 0x36, 0x89, 0x97, 0x9b, 0x50, 0x70, 0x8c, 0x85, 0x46, 0xcb, 0xc2, 0x91, 0x6b, 0x86, 0xcb, 0x1a, 0x3a},
            g1({fp::fromBytesBE(hexToBytes<48>("06328ce5106e837935e8da84bd9af473422e62492930aa5f460369baad9545defa468d9399854c23a75495d2a80487ee")).value(), fp::fromBytesBE(hexToBytes<48>("094bfdfe3e552447433b5a00967498a3f1314b86ce7a7164c8a8f4131f99333b30a574607e301d5f774172c627fd0bca")).value(), fp::one()})
        },
    };
    for(uint64_t i = 0; i < 5; i++)
    {
        g1 p0 = g1::mapToCurve(fp::fromBytesBE(pairs[i].u).value());
        if(!p0.equal(pairs[i].expected))
        {
            throw invalid_argument("G1: map to curve fails");
        }
    }
}

///////////////////////////////////////////////////////////

void TestG2Serialization()
{
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        array<uint8_t, 288> buf = a.toJacobianBytesBE();
        g2 b = g2::fromJacobianBytesBE(buf).value();
        if(!a.equal(b))
        {
            throw invalid_argument("g1, jacobian: bad serialization from/to");
        }
    }
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        array<uint8_t, 192> buf = a.toAffineBytesBE();
        g2 b = g2::fromAffineBytesBE(buf).value();
        if(!a.equal(b))
        {
            throw invalid_argument("g1, affine: bad serialization from/to");
        }
    }
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        array<uint8_t, 96> buf = a.toCompressedBytesBE();
        g2 b = g2::fromCompressedBytesBE(buf).value();
        if(!a.equal(b))
        {
            throw invalid_argument("g1, compressed: bad serialization from/to");
        }
    }
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        array<uint8_t, 288> buf = a.toJacobianBytesLE();
        g2 b = g2::fromJacobianBytesLE(buf).value();
        if(!a.equal(b))
        {
            throw invalid_argument("g1, jacobian, LE: bad serialization from/to");
        }
    }
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        array<uint8_t, 192> buf = a.toAffineBytesLE();
        g2 b = g2::fromAffineBytesLE(buf).value();
        if(!a.equal(b))
        {
            throw invalid_argument("g1, affine, LE: bad serialization from/to");
        }
    }
}

void TestG2SerializationGarbage() {
    array<uint8_t, 288> buf;
    buf.fill(0xff);
    for (int i = 0 ; i < 4; ++i ) {
        auto a = g2::fromJacobianBytesBE(buf, { .check_valid = i<2, .to_mont = !(i%2) });
        if(a)
        {
            throw invalid_argument("g2, jacobianBE: serialization not catching invalid input");
        }
        auto b = g2::fromJacobianBytesLE(buf, { .check_valid = i<2, .to_mont = !(i%2) });
        if(b)
        {
            throw invalid_argument("g2, jacobianLE: serialization not catching invalid input");
        }
    }
    for (int i = 0 ; i < 4; ++i ) {
        auto a = g2::fromAffineBytesBE(std::span<const uint8_t, 192>{buf.begin(),192}, { .check_valid = i<2, .to_mont = !(i%2) });
        if(a)
        {
            throw invalid_argument("g2, affineBE: serialization not catching invalid input");
        }
        auto b = g2::fromAffineBytesLE(std::span<const uint8_t, 192>{buf.begin(),192}, { .check_valid = i<2, .to_mont = !(i%2) });
        if(b)
        {
            throw invalid_argument("g2, affineLE: serialization not catching invalid input");
        }
    }
}

void TestG2IsOnCurve()
{
    g2 zero = g2::zero();
    if(!zero.isOnCurve())
    {
        throw invalid_argument("zero must be on curve");
    }
    fp2 one = fp2::one();
    g2 p = g2({one, one, one});
    if(p.isOnCurve())
    {
        throw invalid_argument("(1, 1) is not on curve");
    }
}

void TestG2AdditiveProperties()
{
    g2 t0, t1;
    g2 zero = g2::zero();
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        g2 b = random_g2();
        t0 = a.add(zero);
        if(!t0.equal(a))
        {
            throw invalid_argument("a + 0 == a");
        }
        t0 = zero.add(zero);
        if(!t0.equal(zero))
        {
            throw invalid_argument("0 + 0 == 0");
        }
        t0 = a.subtract(zero);
        if(!t0.equal(a))
        {
            throw invalid_argument("a - 0 == a");
        }
        t0 = zero.subtract(zero);
        if(!t0.equal(zero))
        {
            throw invalid_argument("0 - 0 == 0");
        }
        t0 = zero.negate();
        if(!t0.equal(zero))
        {
            throw invalid_argument("- 0 == 0");
        }
        t0 = zero.subtract(a);
        t0 = t0.negate();
        if(!t0.equal(a))
        {
            throw invalid_argument(" - (0 - a) == a");
        }
        t0 = zero.dbl();
        if(!t0.equal(zero))
        {
            throw invalid_argument("2 * 0 == 0");
        }
        t0 = a.dbl();
        t0 = t0.subtract(a);
        if(!t0.equal(a) || !t0.isOnCurve())
        {
            throw invalid_argument(" (2 * a) - a == a");
        }
        t0 = a.add(b);
        t1 = b.add(a);
        if(!t0.equal(t1))
        {
            throw invalid_argument("a + b == b + a");
        }
        t0 = a.subtract(b);
        t1 = b.subtract(a);
        t1 = t1.negate();
        if(!t0.equal(t1))
        {
            throw invalid_argument("a - b == - ( b - a )");
        }
        g2 c = random_g2();
        t0 = a.add(b);
        t0 = t0.add(c);
        t1 = a.add(c);
        t1 = t1.add(b);
        if(!t0.equal(t1))
        {
            throw invalid_argument("(a + b) + c == (a + c ) + b");
        }
        t0 = a.subtract(b);
        t0 = t0.subtract(c);
        t1 = a.subtract(c);
        t1 = t1.subtract(b);
        if(!t0.equal(t1))
        {
            throw invalid_argument("(a - b) - c == (a - c) -b");
        }
    }
}

void TestG2MultiplicativeProperties()
{
    g2 t0, t1;
    g2 zero = g2::zero();
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        array<uint64_t, 4> s1 = random_scalar();
        array<uint64_t, 4> s2 = random_scalar(); 
        array<uint64_t, 10> s3;
        array<uint64_t, 4> sone = {1, 0, 0, 0};
        t0 = zero.scale(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == 0");
        }
        t0 = a.scale(sone);
        if(!t0.equal(a))
        {
            throw invalid_argument(" a ^ 1 == a");
        }
        t0 = zero.scale(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == a");
        }
        t0 = a.scale(s1);
        t0 = t0.scale(s2);
        s3 = scalar::multiply<10, 4, 4>(s1, s2);
        t1 = a.scale(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument("G2: (a ^ s1) ^ s2 == a ^ (s1 * s2)");
        }
        t0 = a.scale(s1);
        t1 = a.scale(s2);
        t0 = t0.add(t1);
        s3 = scalar::add<10, 4, 4>(s1, s2);
        t1 = a.scale(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument(" (a ^ s1) + (a ^ s2) == a ^ (s1 + s2)");
        }
    }
}

void TestG2WeightedSumExpected()
{
    g2 one = g2::one();
    vector<array<uint64_t, 4>> scalars = {
        {2, 0, 0, 0},
        {3, 0, 0, 0}
    };
    vector<g2> bases = {one, one};
    g2 expected, result;
    expected = one.scale<1>({5});
    result = g2::weightedSum(bases, scalars);
    if(!expected.equal(result))
    {
        throw invalid_argument("bad multi-exponentiation");
    }
}

void TestG2WeightedSumBatch()
{
    const auto doTest = [](int64_t n) {
        vector<array<uint64_t, 4>> scalars;
        vector<g2> bases;

        for(int64_t i = 0; i < n; i++)
        {
            scalars.push_back(random_scalar());
            bases.push_back(random_g2());
        }

        g2 expected, tmp;
        for(int64_t i = 0; i < n; i++)
        {
            tmp = bases[i].scale(scalars[i]);
            expected = expected.add(tmp);
        }
        g2 result = g2::weightedSum(bases, scalars);
        if(!expected.equal(result))
        {
            throw invalid_argument("bad G2 weighted sum");
        }
    };
    
    doTest(0);
    doTest(1);
    doTest(2);
    doTest(31);
    doTest(32);
    doTest(33);
    doTest(63);
    doTest(64);
    doTest(65);
    doTest(511);
    doTest(512);
    doTest(513);
}

void TestG2MapToCurve()
{
    struct pair
    {
        array<uint8_t, 96> u;
        g2 expected;
    };
    array<pair, 5> pairs = {
        pair{
            array<uint8_t, 96>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            g2({fp2::fromBytesBE(hexToBytes<96>("0a67d12118b5a35bb02d2e86b3ebfa7e23410db93de39fb06d7025fa95e96ffa428a7a27c3ae4dd4b40bd251ac658892018320896ec9eef9d5e619848dc29ce266f413d02dd31d9b9d44ec0c79cd61f18b075ddba6d7bd20b7ff27a4b324bfce")).value(), fp2::fromBytesBE(hexToBytes<96>("04c69777a43f0bda07679d5805e63f18cf4e0e7c6112ac7f70266d199b4f76ae27c6269a3ceebdae30806e9a76aadf5c0260e03644d1a2c321256b3246bad2b895cad13890cbe6f85df55106a0d334604fb143c7a042d878006271865bc35941")).value(), fp2::one()})
        },
        pair{
            array<uint8_t, 96>{0x02, 0x5f, 0xbc, 0x07, 0x71, 0x1b, 0xa2, 0x67, 0xb7, 0xe7, 0x0c, 0x82, 0xca, 0xa7, 0x0a, 0x16, 0xfb, 0xb1, 0xd4, 0x70, 0xae, 0x24, 0xce, 0xef, 0x30, 0x7f, 0x5e, 0x20, 0x00, 0x75, 0x16, 0x77, 0x82, 0x0b, 0x70, 0x13, 0xad, 0x4e, 0x25, 0x49, 0x2d, 0xcf, 0x30, 0x05, 0x2d, 0x3e, 0x5e, 0xca, 0x0e, 0x77, 0x5d, 0x78, 0x27, 0xad, 0xf3, 0x85, 0xb8, 0x3e, 0x20, 0xe4, 0x44, 0x5b, 0xd3, 0xfa, 0xb2, 0x1d, 0x7b, 0x44, 0x98, 0x42, 0x6d, 0xaf, 0x3c, 0x1d, 0x60, 0x8b, 0x9d, 0x41, 0xe9, 0xed, 0xb5, 0xed, 0xa0, 0xdf, 0x02, 0x2e, 0x75, 0x3b, 0x8b, 0xb4, 0xbc, 0x3b, 0xb7, 0xdb, 0x49, 0x14},
            g2({fp2::fromBytesBE(hexToBytes<96>("0d4333b77becbf9f9dfa3ca928002233d1ecc854b1447e5a71f751c9042d000f42db91c1d6649a5e0ad22bd7bf7398b8027e4bfada0b47f9f07e04aec463c7371e68f2fd0c738cd517932ea3801a35acf09db018deda57387b0f270f7a219e4d")).value(), fp2::fromBytesBE(hexToBytes<96>("0cc76dc777ea0d447e02a41004f37a0a7b1fafb6746884e8d9fc276716ccf47e4e0899548a2ec71c2bdf1a2a50e876db053674cba9ef516ddc218fedb37324e6c47de27f88ab7ef123b006127d738293c0277187f7e2f80a299a24d84ed03da7")).value(), fp2::one()})
        },
        pair{
            array<uint8_t, 96>{0x18, 0x70, 0xa7, 0xdb, 0xfd, 0x2a, 0x1d, 0xeb, 0x74, 0x01, 0x5a, 0x35, 0x46, 0xb2, 0x0f, 0x59, 0x80, 0x41, 0xbf, 0x5d, 0x52, 0x02, 0x99, 0x79, 0x56, 0xa9, 0x4a, 0x36, 0x8d, 0x30, 0xd3, 0xf7, 0x0f, 0x18, 0xcd, 0xaa, 0x1d, 0x33, 0xce, 0x97, 0x0a, 0x4e, 0x16, 0xaf, 0x96, 0x1c, 0xbd, 0xcb, 0x04, 0x5a, 0xb3, 0x1c, 0xe4, 0xb5, 0xa8, 0xba, 0x7c, 0x4b, 0x28, 0x51, 0xb6, 0x4f, 0x06, 0x3a, 0x66, 0xcd, 0x12, 0x23, 0xd3, 0xc8, 0x50, 0x05, 0xb7, 0x8e, 0x1b, 0xee, 0xe6, 0x5e, 0x33, 0xc9, 0x0c, 0xee, 0xf0, 0x24, 0x4e, 0x45, 0xfc, 0x45, 0xa5, 0xe1, 0xd6, 0xea, 0xb6, 0x64, 0x4f, 0xdb},
            g2({fp2::fromBytesBE(hexToBytes<96>("18f0f87b40af67c056915dbaf48534c592524e82c1c2b50c3734d02c0172c80df780a60b5683759298a3303c5d94277809349f1cb5b2e55489dcd45a38545343451cc30a1681c57acd4fb0a6db125f8352c09f4a67eb7d1d8242cb7d3405f97b")).value(), fp2::fromBytesBE(hexToBytes<96>("10a2ba341bc689ab947b7941ce6ef39be17acaab067bd32bd652b471ab0792c53a2bd03bdac47f96aaafe96e441f63c002f2d9deb2c7742512f5b8230bf0fd83ea42279d7d39779543c1a43b61c885982b611f6a7a24b514995e8a098496b811")).value(), fp2::one()})
        },
        pair{
            array<uint8_t, 96>{0x08, 0x8f, 0xe3, 0x29, 0xb0, 0x54, 0xdb, 0x8a, 0x64, 0x74, 0xf2, 0x1a, 0x7f, 0xbf, 0xdf, 0x17, 0xb4, 0xc1, 0x80, 0x44, 0xdb, 0x29, 0x9d, 0x90, 0x07, 0xaf, 0x58, 0x2c, 0x3d, 0x5f, 0x17, 0xd0, 0x0e, 0x56, 0xd9, 0x99, 0x21, 0xd4, 0xb5, 0x64, 0x0f, 0xce, 0x44, 0xb0, 0x52, 0x19, 0xb5, 0xde, 0x0b, 0x6e, 0x61, 0x35, 0xa4, 0xcd, 0x31, 0xba, 0x98, 0x0d, 0xdb, 0xd1, 0x15, 0xac, 0x48, 0xab, 0xef, 0x7e, 0xc6, 0x0e, 0x22, 0x6f, 0x26, 0x4d, 0x7b, 0xef, 0xe0, 0x02, 0xc1, 0x65, 0xf3, 0xa4, 0x96, 0xf3, 0x6f, 0x76, 0xdd, 0x52, 0x4e, 0xfd, 0x75, 0xd1, 0x74, 0x22, 0x55, 0x8d, 0x10, 0xb4},
            g2({fp2::fromBytesBE(hexToBytes<96>("19808ec5930a53c7cf5912ccce1cc33f1b3dcff24a53ce1cc4cba41fd6996dbed4843ccdd2eaf6a0cd801e562718d163149fe43777d34f0d25430dea463889bd9393bdfb4932946db23671727081c629ebb98a89604f3433fba1c67d356a4af7")).value(), fp2::fromBytesBE(hexToBytes<96>("04783e391c30c83f805ca271e353582fdf19d159f6a4c39b73acbb637a9b8ac820cfbe2738d683368a7c07ad020e3e3304c0d6793a766233b2982087b5f4a254f261003ccb3262ea7c50903eecef3e871d1502c293f9e063d7d293f6384f4551")).value(), fp2::one()})
        },
        pair{
            array<uint8_t, 96>{0x03, 0xdf, 0x16, 0xa6, 0x6a, 0x05, 0xe4, 0xc1, 0x18, 0x8c, 0x23, 0x47, 0x88, 0xf4, 0x38, 0x96, 0xe0, 0x56, 0x5b, 0xfb, 0x64, 0xac, 0x49, 0xb9, 0x63, 0x9e, 0x6b, 0x28, 0x4c, 0xc4, 0x7d, 0xad, 0x73, 0xc4, 0x7b, 0xb4, 0xea, 0x7e, 0x67, 0x7d, 0xb8, 0xd4, 0x96, 0xbe, 0xb9, 0x07, 0xfb, 0xb6, 0x0f, 0x45, 0xb5, 0x06, 0x47, 0xd6, 0x74, 0x85, 0x29, 0x5a, 0xa9, 0xeb, 0x2d, 0x91, 0xa8, 0x77, 0xb4, 0x48, 0x13, 0x67, 0x7c, 0x67, 0xc8, 0xd3, 0x5b, 0x21, 0x73, 0xff, 0x3b, 0xa9, 0x5f, 0x7b, 0xd0, 0x80, 0x6f, 0x9c, 0xa8, 0xa1, 0x43, 0x6b, 0x8b, 0x9d, 0x14, 0xee, 0x81, 0xda, 0x4d, 0x7e},
            g2({fp2::fromBytesBE(hexToBytes<96>("0b8e0094c886487870372eb6264613a6a087c7eb9804fab789be4e47a57b29eb19b1983a51165a1b5eb025865e9fc63a0804152cbf8474669ad7d1796ab92d7ca21f32d8bed70898a748ed4e4e0ec557069003732fc86866d938538a2ae95552")).value(), fp2::fromBytesBE(hexToBytes<96>("14c80f068ece15a3936bb00c3c883966f75b4e8d9ddde809c11f781ab92d23a2d1d103ad48f6f3bb158bf3e3a406344909e5c8242dd7281ad32c03fe4af3f19167770016255fb25ad9b67ec51d62fade31a1af101e8f6172ec2ee8857662be3a")).value(), fp2::one()})
        },
    };
    for(uint64_t i = 0; i < 5; i++)
    {
        g2 p0 = g2::mapToCurve(fp2::fromBytesBE(pairs[i].u).value());
        if(!p0.equal(pairs[i].expected))
        {
            throw invalid_argument("G2: map to curve fails");
        }
    }
}

///////////////////////////////////////////////////////////

void TestPairingExpected()
{
    fp12 expected = fp12::fromBytesBE(hexToBytes<576>(
        "0f41e58663bf08cf068672cbd01a7ec73baca4d72ca93544deff686bfd6df543d48eaa24afe47e1efde449383b676631" \
        "04c581234d086a9902249b64728ffd21a189e87935a954051c7cdba7b3872629a4fafc05066245cb9108f0242d0fe3ef" \
        "03350f55a7aefcd3c31b4fcb6ce5771cc6a0e9786ab5973320c806ad360829107ba810c5a09ffdd9be2291a0c25a99a2" \
        "11b8b424cd48bf38fcef68083b0b0ec5c81a93b330ee1a677d0d15ff7b984e8978ef48881e32fac91b93b47333e2ba57" \
        "06fba23eb7c5af0d9f80940ca771b6ffd5857baaf222eb95a7d2809d61bfe02e1bfd1b68ff02f0b8102ae1c2d5d5ab1a" \
        "19f26337d205fb469cd6bd15c3d5a04dc88784fbb3d0b2dbdea54d43b2b73f2cbb12d58386a8703e0f948226e47ee89d" \
        "018107154f25a764bd3c79937a45b84546da634b8f6be14a8061e55cceba478b23f7dacaa35c8ca78beae9624045b4b6" \
        "01b2f522473d171391125ba84dc4007cfbf2f8da752f7c74185203fcca589ac719c34dffbbaad8431dad1c1fb597aaa5" \
        "193502b86edb8857c273fa075a50512937e0794e1e65a7617c90d8bd66065b1fffe51d7a579973b1315021ec3c19934f" \
        "1368bb445c7c2d209703f239689ce34c0378a68e72a6b3b216da0e22a5031b54ddff57309396b38c881c4c849ec23e87" \
        "089a1c5b46e5110b86750ec6a532348868a84045483c92b7af5af689452eafabf1a8943e50439f1d59882a98eaa0170f" \
        "1250ebd871fc0a92a7b2d83168d0d727272d441befa15c503dd8e90ce98db3e7b6d194f60839c508a84305aaca1789b6"
    )).value();
    g1 g1One = g1::one();
    g2 g2One = g2::one();
    vector<tuple<g1, g2>> v;
    pairing::add_pair(v, g1One, g2One);
    fp12 r = pairing::calculate(v);
    if(!r.equal(expected))
    {
        throw invalid_argument("0: bad pairing");
    }
    if(!r.isGtValid())
    {
        throw invalid_argument("0: element is not in correct subgroup");
    }
}

void TestPairingNonDegeneracy()
{
    g1 g1Zero = g1::zero();
    g2 g2Zero = g2::zero();
    g1 g1One = g1::one();
    g2 g2One = g2::one();
    // e(g1^a, g2^b) != 1
    {
        vector<tuple<g1, g2>> v;
        pairing::add_pair(v, g1One, g2One);
        fp12 e = pairing::calculate(v);
        if(e.isOne())
        {
            throw invalid_argument("0: pairing result is not expected to be one");
        }
        if(!e.isGtValid())
        {
            throw invalid_argument("0: pairing result is not valid");
        }
    }
    // e(g1^a, 0) == 1
    {
        vector<tuple<g1, g2>> v;
        pairing::add_pair(v, g1One, g2Zero);
        fp12 e = pairing::calculate(v);
        if(!e.isOne())
        {
            throw invalid_argument("1: pairing result is expected to be one");
        }
    }
    // e(0, g2^b) == 1
    {
        vector<tuple<g1, g2>> v;
        pairing::add_pair(v, g1Zero, g2One);
        fp12 e = pairing::calculate(v);
        if(!e.isOne())
        {
            throw invalid_argument("2: pairing result is expected to be one");
        }
    }
    //
    {
        vector<tuple<g1, g2>> v;
        pairing::add_pair(v, g1Zero, g2One);
        pairing::add_pair(v, g1One, g2Zero);
        pairing::add_pair(v, g1Zero, g2Zero);
        fp12 e = pairing::calculate(v);
        if(!e.isOne())
        {
            throw invalid_argument("3: pairing result is expected to be one");
        }
    }
    //
    {
        fp12 expected = fp12::fromBytesBE(hexToBytes<576>(
            "0f41e58663bf08cf068672cbd01a7ec73baca4d72ca93544deff686bfd6df543d48eaa24afe47e1efde449383b676631" \
            "04c581234d086a9902249b64728ffd21a189e87935a954051c7cdba7b3872629a4fafc05066245cb9108f0242d0fe3ef" \
            "03350f55a7aefcd3c31b4fcb6ce5771cc6a0e9786ab5973320c806ad360829107ba810c5a09ffdd9be2291a0c25a99a2" \
            "11b8b424cd48bf38fcef68083b0b0ec5c81a93b330ee1a677d0d15ff7b984e8978ef48881e32fac91b93b47333e2ba57" \
            "06fba23eb7c5af0d9f80940ca771b6ffd5857baaf222eb95a7d2809d61bfe02e1bfd1b68ff02f0b8102ae1c2d5d5ab1a" \
            "19f26337d205fb469cd6bd15c3d5a04dc88784fbb3d0b2dbdea54d43b2b73f2cbb12d58386a8703e0f948226e47ee89d" \
            "018107154f25a764bd3c79937a45b84546da634b8f6be14a8061e55cceba478b23f7dacaa35c8ca78beae9624045b4b6" \
            "01b2f522473d171391125ba84dc4007cfbf2f8da752f7c74185203fcca589ac719c34dffbbaad8431dad1c1fb597aaa5" \
            "193502b86edb8857c273fa075a50512937e0794e1e65a7617c90d8bd66065b1fffe51d7a579973b1315021ec3c19934f" \
            "1368bb445c7c2d209703f239689ce34c0378a68e72a6b3b216da0e22a5031b54ddff57309396b38c881c4c849ec23e87" \
            "089a1c5b46e5110b86750ec6a532348868a84045483c92b7af5af689452eafabf1a8943e50439f1d59882a98eaa0170f" \
            "1250ebd871fc0a92a7b2d83168d0d727272d441befa15c503dd8e90ce98db3e7b6d194f60839c508a84305aaca1789b6"
        )).value();
        vector<tuple<g1, g2>> v;
        pairing::add_pair(v, g1Zero, g2One);
        pairing::add_pair(v, g1One, g2Zero);
        pairing::add_pair(v, g1Zero, g2Zero);
        pairing::add_pair(v, g1One, g2One);
        fp12 e = pairing::calculate(v);
        if(!e.equal(expected))
        {
            throw invalid_argument("4: bad pairing");
        }
    }
}

void TestPairingBilinearity()
{
    // e(a*G1, b*G2) = e(G1, G2)^c
    {
        array<uint64_t, 4> a = {17, 0, 0, 0};
        array<uint64_t, 4> b = {117, 0, 0, 0};
        // c = a * b = 1989
        array<uint64_t, 4> c = {1989, 0, 0, 0};
        g1 G1 = g1::one();
        g2 G2 = g2::one();
        vector<tuple<g1, g2>> v;
        pairing::add_pair(v, G1, G2);
        fp12 e0 = pairing::calculate(v);
        g1 P1 = G1.scale(a);
        g2 P2 = G2.scale(b);
        v = {};
        pairing::add_pair(v, P1, P2);
        fp12 e1 = pairing::calculate(v);
        e0 = e0.cyclotomicExp(c);
        if(!e0.equal(e1))
        {
            throw invalid_argument("bad pairing, 1");
        }
    }
    // e(a * G1, b * G2) = e((a + b) * G1, G2)
    {
        // scalars
        array<uint64_t, 4> a = {17, 0, 0, 0};
        array<uint64_t, 4> b = {117, 0, 0, 0};
        // c = a * b = 1989
        array<uint64_t, 4> c = {1989, 0, 0, 0};
        vector<tuple<g1, g2>> v;
        // LHS
        g1 G1 = g1::one();
        g2 G2 = g2::one();
        G1 = G1.scale(c);
        pairing::add_pair(v, G1, G2);
        // RHS
        g1 P1 = g1::one();
        g2 P2 = g2::one();
        P1 = P1.scale(a);
        P2 = P2.scale(b);
        P1 = P1.negate();
        pairing::add_pair(v, P1, P2);
        // should be one
        if(!pairing::calculate(v).isOne())
        {
            throw invalid_argument("bad pairing, 2");
        }
    }
    // e(a * G1, b * G2) = e((a + b) * G1, G2)
    {
        // scalars
        array<uint64_t, 4> a = {17, 0, 0, 0};
        array<uint64_t, 4> b = {117, 0, 0, 0};
        // c = a * b = 1989
        array<uint64_t, 4> c = {1989, 0, 0, 0};
        vector<tuple<g1, g2>> v;
        // LHS
        g1 G1 = g1::one();
        g2 G2 = g2::one();
        G2 = G2.scale(c);
        pairing::add_pair(v, G1, G2);
        // RHS
        g1 H1 = g1::one();
        g2 H2 = g2::one();
        H1 = H1.scale(a);
        H2 = H2.scale(b);
        H1 = H1.negate();
        pairing::add_pair(v, H1, H2);
        // should be one
        if(!pairing::calculate(v).isOne())
        {
            throw invalid_argument("bad pairing, 3");
        }
    }
}

void TestPairingMulti()
{
    // e(G1, G2) ^ t == e(a01 * G1, a02 * G2) * e(a11 * G1, a12 * G2) * ... * e(an1 * G1, an2 * G2)
    // where t = sum(ai1 * ai2)
    uint64_t numOfPair = 100;
    vector<tuple<g1, g2>> v;
    array<uint64_t, 10> targetExp = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // RHS
    for(uint64_t i = 0; i < numOfPair; i++)
    {
        // (ai1 * G1, ai2 * G2)
        array<uint64_t, 4> a1 = random_scalar();
        array<uint64_t, 4> a2 = random_scalar();
        g1 P1 = g1::one();
        g2 P2 = g2::one();
        P1 = P1.scale(a1);
        P2 = P2.scale(a2);
        pairing::add_pair(v, P1, P2);
        // accumulate targetExp
        // t += (ai1 * ai2)
        array<uint64_t, 10> tmp = scalar::multiply<10, 4, 4>(a1, a2);
        targetExp = scalar::add<10, 10, 10>(targetExp, tmp);
    }
    // LHS
    // e(t * G1, G2)
    g1 T1 = g1::one();
    g2 T2 = g2::one();
    T1 = T1.scale(targetExp);
    T1 = T1.negate();
    pairing::add_pair(v, T1, T2);
    if(!pairing::calculate(v).isOne())
    {
        throw invalid_argument("fail multi pairing");
    }
}

///////////////////////////////////////////////////////////

void TestEIP2333(string seedHex, string masterSkHex, string childSkHex, uint32_t childIndex)
{
    array<uint8_t, 32> masterSk = hexToBytes<32>(masterSkHex);
    array<uint8_t, 32> childSk = hexToBytes<32>(childSkHex);

    array<uint64_t, 4> master = secret_key(hexToBytes(seedHex));
    array<uint64_t, 4> child = derive_child_sk(master, childIndex);

    array<uint8_t, 32> calculatedMaster = scalar::toBytesBE(master);
    array<uint8_t, 32> calculatedChild = scalar::toBytesBE(child);

    if(calculatedMaster.size() != 32)
    {
        throw invalid_argument("TestEIP2333: calculatedMaster size invalid!");
    }
    if(calculatedChild.size() != 32)
    {
        throw invalid_argument("TestEIP2333: calculatedChild size invalid!");
    }
    for(int i=0; i<32; i++)
    {
        if(calculatedMaster[i] != masterSk[i])
        {
            throw invalid_argument("TestEIP2333: calculatedMaster != masterSk");
        }
    }
    for (int i=0; i<32; i++)
    {
        if(calculatedChild[i] != childSk[i])
        {
            throw invalid_argument("TestEIP2333: calculatedChild != childSk");
        }
    }
}

void TestsEIP2333()
{
    // The comments in the test cases correspond to  integers that are converted to
    // bytes using python int.to_bytes(32, "big").hex(), since the EIP spec provides ints, but c++
    // does not support bigint by default
    {
        TestEIP2333("3141592653589793238462643383279502884197169399375105820974944592",
                    // 36167147331491996618072159372207345412841461318189449162487002442599770291484
                    "4ff5e145590ed7b71e577bb04032396d1619ff41cb4e350053ed2dce8d1efd1c",
                    // 41787458189896526028601807066547832426569899195138584349427756863968330588237
                    "5c62dcf9654481292aafa3348f1d1b0017bbfb44d6881d26d2b17836b38f204d",
                    3141592653
        );
    }
    {
        TestEIP2333("0x0099FF991111002299DD7744EE3355BBDD8844115566CC55663355668888CC00",
                    // 13904094584487173309420026178174172335998687531503061311232927109397516192843
                    "1ebd704b86732c3f05f30563dee6189838e73998ebc9c209ccff422adee10c4b",
                    // 12482522899285304316694838079579801944734479969002030150864436005368716366140
                    "1b98db8b24296038eae3f64c25d693a269ef1e4d7ae0f691c572a46cf3c0913c",
                    4294967295
        );
    }
    {
        TestEIP2333("0xd4e56740f876aef8c010b86a40d5f56745a118d0906a34e69aec8c0db1cb8fa3",
                    // 44010626067374404458092393860968061149521094673473131545188652121635313364506
                    "614d21b10c0e4996ac0608e0e7452d5720d95d20fe03c59a3321000a42432e1a",
                    // 4011524214304750350566588165922015929937602165683407445189263506512578573606
                    "08de7136e4afc56ae3ec03b20517d9c1232705a747f588fd17832f36ae337526",
                    42
        );
    }
    {
        TestEIP2333("c55257c360c07c72029aebc1b53c05ed0362ada38ead3e3e9efa3708e53495531f09a6987599d18264c1e1c92f2cf141630c7a3c4ab7c81b2f001698e7463b04",
                    // 5399117110774477986698372024995405256382522670366369834617409486544348441851
                    "0x0befcabff4a664461cc8f190cdd51c05621eb2837c71a1362df5b465a674ecfb",
                    // 11812940737387919040225825939013910852517748782307378293770044673328955938106
                    "1a1de3346883401f1e3b2281be5774080edb8e5ebe6f776b0f7af9fea942553a",
                    0
        );
    }
}

void TestUnhardenedHDKeys()
{
    {
        const vector<uint8_t> seed = {1, 50, 6, 244, 24, 199, 1, 25, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};

        array<uint64_t, 4> sk = secret_key(seed);
        g1 pk = public_key(sk);

        array<uint64_t, 4> childSk = derive_child_sk_unhardened(sk, 42);
        g1 childPk = derive_child_g1_unhardened(pk, 42);

        if(!public_key(childSk).equal(childPk))
        {
            throw invalid_argument("error");
        }

        array<uint64_t, 4> grandchildSk = derive_child_sk_unhardened(childSk, 12142);
        g1 grandcihldPk = derive_child_g1_unhardened(childPk, 12142);

        if(!public_key(grandchildSk).equal(grandcihldPk))
        {
            throw invalid_argument("error");
        }
    }
    {
        const vector<uint8_t> seed = {2, 50, 6, 244, 24, 199, 1, 25, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};

        array<uint64_t, 4> sk = secret_key(seed);
        g1 pk = public_key(sk);

        array<uint64_t, 4> childSk = derive_child_sk_unhardened(sk, 42);
        g1 childPk = derive_child_g1_unhardened(pk, 42);

        array<uint64_t, 4> childSkHardened = derive_child_sk(sk, 42);
        if(!public_key(childSk).equal(childPk))
        {
            throw invalid_argument("SkToG1(childSk) != childPk");
        }
        if(childSkHardened == childSk)
        {
            throw invalid_argument("childSkHardened == childSk");
        }
        if(public_key(childSkHardened).equal(childPk))
        {
            throw invalid_argument("SkToG1(childSkHardened) == childPk");
        }
    }
}

void TestIETFVectors()
{
    string sig1BasicHex = "96ba34fac33c7f129d602a0bc8a3d43f9abc014eceaab7359146b4b150e57b808645738f35671e9e10e0d862a30cab70074eb5831d13e6a5b162d01eebe687d0164adbd0a864370a7c222a2768d7704da254f1bf1823665bc2361f9dd8c00e99";
    string sk = "0x0101010101010101010101010101010101010101010101010101010101010101";
    vector<uint8_t> msg = {3, 1, 4, 1, 5, 9};
    array<uint64_t, 4UL> skobj = sk_from_bytes(hexToBytes<32>(sk), false);
    g2 sig = sign(skobj, msg).affine();
    g2 sig1 = g2::fromCompressedBytesBE(hexToBytes<96>(sig1BasicHex)).value();
    
    array<uint8_t, 96> bytes = sig.toCompressedBytesBE();
    string s = bytesToHex<96>(bytes);

    if(!sig.affine().equal(sig1))
    {
        throw invalid_argument("signatures must be equal");
    }
    if(s != ("0x" + sig1BasicHex))
    {
        throw invalid_argument("signature strings must be equal");
    }
}

void TestChiaVectors()
{
    vector<uint8_t> seed1(32, 0x00);  // All 0s
    vector<uint8_t> seed2(32, 0x01);  // All 1s
    vector<uint8_t> message1 = {7, 8, 9};
    vector<uint8_t> message2 = {10, 11, 12};

    array<uint64_t, 4> sk1 = secret_key(seed1);
    g1 pk1 = public_key(sk1);
    g2 sig1 = sign(sk1, message1);
    if(!verify(pk1, message1, sig1))
    {
        throw invalid_argument("sig1 verification failed");
    }

    array<uint64_t, 4> sk2 = secret_key(seed2);
    g1 pk2 = public_key(sk2);
    g2 sig2 = sign(sk2, message2);
    if(!verify(pk2, message2, sig2))
    {
        throw invalid_argument("sig2 verification failed");
    }

    g2 aggSig1 = aggregate_signatures(std::array{sig1, sig2});
    if(!aggregate_verify(std::array{pk1, pk2}, std::array{message1, message2}, aggSig1))
    {
        throw invalid_argument("aggSig1 verification failed");
    }

    vector<uint8_t> message3 = {1, 2, 3};
    vector<uint8_t> message4 = {1, 2, 3, 4};
    vector<uint8_t> message5 = {1, 2};

    g2 sig3 = sign(sk1, message3);
    g2 sig4 = sign(sk1, message4);
    g2 sig5 = sign(sk2, message5);

    g2 aggSig2 = aggregate_signatures(std::array{sig3, sig4, sig5});
    
    if(!aggregate_verify(std::array{pk1, pk1, pk2}, vector<vector<uint8_t>>{message3, message4, message5}, aggSig2))
    {
        throw invalid_argument("aggSig2 verification failed");
    }
}

void TestChiaVectors2()
{
    vector<uint8_t> message1 = {1, 2, 3, 40};
    vector<uint8_t> message2 = {5, 6, 70, 201};
    vector<uint8_t> message3 = {9, 10, 11, 12, 13};
    vector<uint8_t> message4 = {15, 63, 244, 92, 0, 1};

    vector<uint8_t> seed1(32, 0x02);  // All 2s
    vector<uint8_t> seed2(32, 0x03);  // All 3s

    array<uint64_t, 4> sk1 = secret_key(seed1);
    array<uint64_t, 4> sk2 = secret_key(seed2);

    g1 pk1 = public_key(sk1);
    g1 pk2 = public_key(sk2);

    g2 sig1 = sign(sk1, message1);
    g2 sig2 = sign(sk2, message2);
    g2 sig3 = sign(sk2, message1);
    g2 sig4 = sign(sk1, message3);
    g2 sig5 = sign(sk1, message1);
    g2 sig6 = sign(sk1, message4);


    g2 aggSigL = aggregate_signatures(std::array{sig1, sig2});
    g2 aggSigR = aggregate_signatures(std::array{sig3, sig4, sig5});
    g2 aggSig  = aggregate_signatures(std::array{aggSigL, aggSigR, sig6});

    if(!aggregate_verify(std::array{pk1, pk2, pk2, pk1, pk1, pk1},
                         vector<vector<uint8_t>>{message1, message2, message1, message3, message1, message4}, aggSig))
    {
        throw invalid_argument("aggregate verify error");
    }
}

void TestSignatures()
{
    {
        vector<uint8_t> message1 = {1, 65, 254, 88, 90, 45, 22};

        vector<uint8_t> seed(32, 0x30);
        array<uint64_t, 4> sk1 = secret_key(seed);
        g1 pk1 = public_key(sk1);
        array<uint64_t, 4> sk2 = sk1;

        array<uint8_t, 32> skBytes = sk_to_bytes(sk2);
        array<uint64_t, 4> sk4 = sk_from_bytes(skBytes);

        g1 pk2 = pk1;
        g2 sig1 = sign(sk4, message1);
        g2 sig2 = sig1;

        if(!verify(pk2, message1, sig2))
        {
            throw invalid_argument("verification failed!");
        }
    }
    {
        vector<uint8_t> message1 = {1, 65, 254, 88, 90, 45, 22};
        vector<uint8_t> seed(32, 0x40);
        vector<uint8_t> seed3(32, 0x50);

        array<uint64_t, 4> sk1 = secret_key(seed);
        array<uint64_t, 4> sk2 = sk1;
        array<uint64_t, 4> sk3 = secret_key(seed3);
        g1 pk1 = public_key(sk1);
        g1 pk2 = public_key(sk2);
        g1 pk3 = pk2;
        g1 pk4 = public_key(sk3);
        g2 sig1 = sign(sk1, message1);
        g2 sig2 = sign(sk1, message1);
        g2 sig3 = sign(sk2, message1);
        g2 sig4 = sign(sk3, message1);

        if(sk1 != sk2) throw invalid_argument("sk1 must equal sk2!");
        if(sk1 == sk3) throw invalid_argument("sk1 must NOT equal sk3!");
        if(!pk1.equal(pk2)) throw invalid_argument("pk1 must equal pk2!");
        if(!pk2.equal(pk3)) throw invalid_argument("pk2 must equal pk3!");
        if(pk1.equal(pk4)) throw invalid_argument("pk1 must NOT equal pk4!");
        if(!sig1.equal(sig2)) throw invalid_argument("sig1 must equal sig2!");
        if(!sig2.equal(sig3)) throw invalid_argument("sig2 must equal sig3!");
        if(sig3.equal(sig4)) throw invalid_argument("sig3 must NOT equal sig4!");

        if(pk1.toCompressedBytesBE() != pk2.toCompressedBytesBE()) throw invalid_argument("serialized pk1 must equal serialized pk2!");
        if(sig1.toCompressedBytesBE() != sig2.toCompressedBytesBE()) throw invalid_argument("serialized sig1 must equal serialized sig2!");
    }
    {
        vector<uint8_t> message1 = {1, 65, 254, 88, 90, 45, 22};

        vector<uint8_t> seed(32, 0x40);
        array<uint64_t, 4> sk1 = secret_key(seed);
        g1 pk1 = public_key(sk1);

        array<uint8_t, 32> skData = sk_to_bytes(sk1);
        array<uint64_t, 4> sk2 = sk_from_bytes(skData);
        if(sk1 != sk2) throw invalid_argument("sk1 must equal sk2!");

        array<uint8_t, 96> pkData = pk1.toAffineBytesBE();

        g1 pk2 = g1::fromAffineBytesBE(pkData).value();
        if(!pk1.equal(pk2)) throw invalid_argument("pk1 must equal pk2!");

        g2 sig1 = sign(sk1, message1);

        array<uint8_t, 192> sigData = sig1.toAffineBytesBE();

        g2 sig2 = g2::fromAffineBytesBE(sigData).value();
        if(!sig1.equal(sig2)) throw invalid_argument("sig1 must equal sig2!");

        if(!verify(pk2, message1, sig2)) throw invalid_argument("verify failed!");
    }
    {
        vector<uint8_t> message = {100, 2, 254, 88, 90, 45, 23};

        vector<uint8_t> seed(32, 0x50);
        vector<uint8_t> seed2(32, 0x70);

        array<uint64_t, 4> sk1 = secret_key(seed);
        array<uint64_t, 4> sk2 = secret_key(seed2);

        g1 pk1 = public_key(sk1);
        g1 pk2 = public_key(sk2);

        g2 sig1 = sign(sk1, message);
        g2 sig2 = sign(sk2, message);

        g2 aggSig = aggregate_signatures(std::array{sig1, sig2});
        if(aggregate_verify(std::array{pk1, pk2}, vector<vector<uint8_t>>{message, message}, aggSig, true))
        {
            throw invalid_argument("Should not verify aggregate with same message under BasicScheme");
        }
    }
}

void TestAugScheme()
{
    vector<uint8_t> message = {100, 2, 254, 88, 90, 45, 23};

    vector<uint8_t> seed(32, 0x50);
    vector<uint8_t> seed2(32, 0x70);

    array<uint64_t, 4> sk1 = secret_key(seed);
    array<uint64_t, 4> sk2 = secret_key(seed2);

    g1 pk1 = public_key(sk1);
    g1 pk2 = public_key(sk2);
    array<uint8_t, 96UL> pk1_bytes = pk1.toAffineBytesBE();
    array<uint8_t, 96UL> pk2_bytes = pk2.toAffineBytesBE();

    // Augmented Scheme: Each signer extends the same message with their individual public keys
    vector<uint8_t> augMsg1 = message;
    augMsg1.insert(augMsg1.end(), pk1_bytes.begin(), pk1_bytes.end());
    vector<uint8_t> augMsg2 = message;
    augMsg2.insert(augMsg2.end(), pk2_bytes.begin(), pk2_bytes.end());
    g2 sig1Aug = sign(sk1, augMsg1);
    g2 sig2Aug = sign(sk2, augMsg2);
    g2 aggSigAug = aggregate_signatures(std::array{sig1Aug, sig2Aug});
    if(!aggregate_verify(std::array{pk1, pk2}, vector<vector<uint8_t>>{augMsg1, augMsg2}, aggSigAug, true))
    {
        throw invalid_argument("Should verify aggregate with same message under AugScheme");
    }
}

void TestAggregateSKs()
{
    const vector<uint8_t> message = {100, 2, 254, 88, 90, 45, 23};
    const vector<uint8_t> seed(32, 0x07);
    const vector<uint8_t> seed2(32, 0x08);

    const array<uint64_t, 4> sk1 = secret_key(seed);
    const g1 pk1 = public_key(sk1);

    const array<uint64_t, 4> sk2 = secret_key(seed2);
    const g1 pk2 = public_key(sk2);

    const array<uint64_t, 4> aggSk = aggregate_secret_keys(std::array{sk1, sk2});
    const array<uint64_t, 4> aggSkAlt = aggregate_secret_keys(std::array{sk2, sk1});
    if(aggSk != aggSkAlt) throw invalid_argument("aggSk != aggSkAlt");

    const g1 aggPubKey = pk1.add(pk2);
    if(!aggPubKey.equal(public_key(aggSk))) throw invalid_argument("aggPubKey != public_key(aggSk)");

    const g2 sig1 = sign(sk1, message);
    const g2 sig2 = sign(sk2, message);

    const g2 aggSig2 = sign(aggSk, message);


    const g2 aggSig = aggregate_signatures(std::array{sig1, sig2});
    if(!aggSig.equal(aggSig2)) throw invalid_argument("aggSig != aggSig2");

    // Verify as a single G2Element
    if(!verify(aggPubKey, message, aggSig)) throw invalid_argument("aggSig verify failed");
    if(!verify(aggPubKey, message, aggSig2)) throw invalid_argument("aggSig2 verify failed");

    // Verify aggregate with both keys (Fails since not distinct)
    if(aggregate_verify(std::array{pk1, pk2}, vector<vector<uint8_t>>{message, message}, aggSig, true)) throw invalid_argument("aggregate verify of aggSig must fail");
    if(aggregate_verify(std::array{pk1, pk2}, vector<vector<uint8_t>>{message, message}, aggSig2, true)) throw invalid_argument("aggregate verify of aggSig2 must fail");

    // Try the same with distinct message, and same sk
    vector<uint8_t> message2 = {200, 29, 54, 8, 9, 29, 155, 55};
    g2 sig3 = sign(sk2, message2);
    g2 aggSigFinal = aggregate_signatures(std::array{aggSig, sig3});
    g2 aggSigAlt = aggregate_signatures(std::array{sig1, sig2, sig3});
    g2 aggSigAlt2 = aggregate_signatures(std::array{sig1, sig3, sig2});
    if(!aggSigFinal.equal(aggSigAlt)) throw invalid_argument("aggSigFinal != aggSigAlt");
    if(!aggSigFinal.equal(aggSigAlt2)) throw invalid_argument("aggSigFinal != aggSigAlt2");

    array<uint64_t, 4> skFinal = aggregate_secret_keys(std::array{aggSk, sk2});
    array<uint64_t, 4> skFinalAlt = aggregate_secret_keys(std::array{sk2, sk1, sk2});
    if(skFinal != skFinalAlt) throw invalid_argument("skFinal != skFinalAlt");
    if(skFinal == aggSk) throw invalid_argument("skFinal == aggSk");

    g1 pkFinal = aggPubKey.add(pk2);
    g1 pkFinalAlt = pk2.add(pk1).add(pk2);
    if(!pkFinal.equal(pkFinalAlt)) throw invalid_argument("pkFinal != pkFinalAlt");
    if(pkFinal.equal(aggPubKey)) throw invalid_argument("pkFinal == aggPubKey");

    // Verify with aggPubKey (since we have multiple messages)
    if(!aggregate_verify(std::array{aggPubKey, pk2}, vector<vector<uint8_t>>{message, message2}, aggSigFinal, true)) throw invalid_argument("verify with aggPubKey failed");
}

void TestPopScheme()
{
    {
        vector<uint8_t> seed1(32, 0x06);
        vector<uint8_t> seed2(32, 0x07);
        vector<uint8_t> msg1 = {7, 8, 9};
        vector<uint8_t> msg2 = {10, 11, 12};
        vector<vector<uint8_t>> msgs = {msg1, msg2};

        array<uint64_t, 4> sk1 = secret_key(seed1);
        g1 pk1 = public_key(sk1);
        g2 sig1 = sign(sk1, msg1);

        if(!verify(pk1, msg1, sig1)) throw invalid_argument("verify failed");

        array<uint64_t, 4> sk2 = secret_key(seed2);
        g1 pk2 = public_key(sk2);
        g2 sig2 = sign(sk2, msg2);

        // Wrong sig
        if(verify(pk1, msg1, sig2)) throw invalid_argument("must fail: wrong sig");
        // Wrong msg
        if(verify(pk1, msg2, sig1)) throw invalid_argument("must fail: wrong msg");
        // Wrong pk
        if(verify(pk2, msg1, sig1)) throw invalid_argument("must fail: wrong pk");

        g2 aggsig = aggregate_signatures(std::array{sig1, sig2});
        if(!aggregate_verify(std::array{pk1, pk2}, msgs, aggsig)) throw invalid_argument("aggregate_verify failed");

        // PopVerify
        g2 proof1 = pop_prove(sk1);
        if(!pop_verify(pk1, proof1)) throw invalid_argument("pop_verify failed");

        // FastAggregateVerify
        // We want sk2 to sign the same message
        g2 sig2_same = sign(sk2, msg1);
        g2 aggsig_same = aggregate_signatures(std::array{sig1, sig2_same});
        if(!pop_fast_aggregate_verify(std::array{pk1, pk2}, msg1, aggsig_same)) throw invalid_argument("pop_fast_aggregate_verify failed");
    }
    {
        // test template versions of aggregate_signatures
        struct enh_pk { std::string name; g1 pk; };
        struct enh_sig { std::string name; g2 sig; };

        vector<uint8_t> seed1(32, 0x06);
        vector<uint8_t> seed2(32, 0x07);
        vector<uint8_t> msg1 = {7, 8, 9};
        vector<uint8_t> msg2 = {10, 11, 12};
        vector<vector<uint8_t>> msgs = {msg1, msg2};

        array<uint64_t, 4> sk1 = secret_key(seed1);
        enh_pk pk1 { "pk1", public_key(sk1) };
        enh_sig sig1 { "sig1",  sign(sk1, msg1) };

        array<uint64_t, 4> sk2 = secret_key(seed2);
        enh_pk pk2 { "pk2", public_key(sk2) };
        enh_sig sig2 { "sig2",  sign(sk2, msg1) };

        std::array pks { pk1, pk2 };
        std::array sigs { sig1, sig2 };

        g1 agg_pk = aggregate_public_keys(std::span{pks.begin(), pks.size()},
                                          [](const enh_pk& epk) { return epk.pk; });
        
        g2 agg_sig = aggregate_signatures(std::span{sigs.begin(), sigs.size()},
                                          [](const enh_sig& esig) { return esig.sig; });
        
        
        if (!verify(agg_pk, msg1, agg_sig))
            throw invalid_argument("verify failed");
    }
    {
        // from README:
        vector<uint8_t> seed = {0,  50, 6,  244, 24,  199, 1,  25,  52,  88,  192,
                                19, 18, 12, 89,  6,   220, 18, 102, 58,  209, 82,
                                12, 62, 89, 110, 182, 9,   44, 20,  254, 22};
        vector<uint8_t> message = {1, 2, 3, 4, 5};
        seed[0] = 1;
        array<uint64_t, 4> sk1 = secret_key(seed);
        g1 pk1 = public_key(sk1);
        seed[0] = 2;
        array<uint64_t, 4> sk2 = secret_key(seed);
        g1 pk2 = public_key(sk2);
        seed[0] = 3;
        array<uint64_t, 4> sk3 = secret_key(seed);
        g1 pk3 = public_key(sk3);

        // If the same message is signed, you can use Proof of Posession (PopScheme) for efficiency
        // A proof of possession MUST be passed around with the PK to ensure security.
        g2 popSig1 = sign(sk1, message);
        g2 popSig2 = sign(sk2, message);
        g2 popSig3 = sign(sk3, message);
        g2 pop1 = pop_prove(sk1);
        g2 pop2 = pop_prove(sk2);
        g2 pop3 = pop_prove(sk3);

        if(!pop_verify(pk1, pop1)) throw invalid_argument("pop_verify 1 failed");
        if(!pop_verify(pk2, pop2)) throw invalid_argument("pop_verify 2 failed");
        if(!pop_verify(pk3, pop3)) throw invalid_argument("pop_verify 3 failed");
        g2 popSigAgg = aggregate_signatures(std::array{popSig1, popSig2, popSig3});

        if(!pop_fast_aggregate_verify(std::array{pk1, pk2, pk3}, message, popSigAgg)) throw invalid_argument("pop_fast_aggregate_verify failed");

        // Aggregate public key, indistinguishable from a single public key
        g1 popAggPk = pk1.add(pk2).add(pk3);
        if(!verify(popAggPk, message, popSigAgg)) throw invalid_argument("verify popSigAgg failed");

        // Aggregate private keys
        array<uint64_t, 4> aggSk = aggregate_secret_keys(std::array{sk1, sk2, sk3});
        if(!sign(aggSk, message).equal(popSigAgg)) throw invalid_argument("sign(aggSk, message) != popSigAgg");
    }
}

// Test Vectors from FileCoin
#include "test_vectors.inc"

void TestExtraVectors()
{
    const int num_case = sizeof(tv_msg) / sizeof(char *);
    for (int i = 0; i < num_case; i++)
    {
        try
        {
            auto msg_vec = std::vector<uint8_t>(tv_msg[i], tv_msg[i] + strlen(tv_msg[i]));

            g2 hash = fromMessage(msg_vec, CIPHERSUITE_ID);
            g2 hash_tv = g2::fromCompressedBytesBE(hexToBytes<96>(tv_g2[i])).value();

            if (!hash.affine().equal(hash_tv))
            {
                throw invalid_argument("hash as g2 must be equal");
            }
            if (bytesToHex<96>(hash.toCompressedBytesBE()) != string("0x") + tv_g2[i])
            {
                throw invalid_argument("hash strings must be equal");
            }

            array<uint64_t, 4UL> skobj = sk_from_bytes(hexToBytes<32>(tv_priv[i]), false);
            g2 sig = sign(skobj, msg_vec).affine();
            g2 sig_tv = g2::fromCompressedBytesBE(hexToBytes<96>(tv_sig[i])).value();

            if (!sig.affine().equal(sig_tv))
            {
                throw invalid_argument("signatures must be equal");
            }
            if (bytesToHex<96>(sig.toCompressedBytesBE()) != (string("0x") + tv_sig[i]))
            {
                throw invalid_argument("signature strings must be equal");
            }

            g1 pk = public_key(skobj);
            g1 pk_tv = g1::fromCompressedBytesBE(hexToBytes<48>(tv_pub[i])).value();

            if (!pk.affine().equal(pk_tv))
            {
                throw invalid_argument("public key must be equal");
            }
            if (bytesToHex<96>(sig.toCompressedBytesBE()) != (string("0x") + tv_sig[i]))
            {
                throw invalid_argument("public key strings must be equal");
            }

            if (!verify(pk, msg_vec, sig))
            {
                throw invalid_argument("failed to verify with pk");
            }
        }
        catch (std::exception &e)
        {
            printf("test %d failed: %s \n", i, e.what());
            throw;
        }
    }
}

void TestOutOfRangeInputs() {
    // This test is to make sure multiplication wiil not fail if the inputs is just slightly larger than p
    // The 4(p-1) limit may be not that strict. But we should only relax this limit if we are absolutely sure this 
    // will not cause problems all the methods calling _ladd/_lsubstract/_ldouble.
    auto p = *fp::fromBytesBE(hexToBytes<48>("1A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAB"), { .check_valid = false, .to_mont = false });
    auto pminus1 = *fp::fromBytesBE(hexToBytes<48>("1A0111EA397FE69A4B1BA7B6434BACD764774B84F38512BF6730D2A0F6B0F6241EABFFFEB153FFFFB9FEFFFFFFFFAAAA"), { .check_valid = false, .to_mont = false });
    // 2^383, largest possible input to multiplication during the inverse().
    auto two383 = *fp::fromBytesBE(hexToBytes<48>("400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"), { .check_valid = false, .to_mont = false });
    // 4(p-1) * 4(p-1) will work
    for (int i = 0 ; i < 3; ++ i) {
        auto a = pminus1;
        auto b = pminus1;
        auto a2 = pminus1;
        a2 = a2.fromMont().toMont();
        auto b2 = pminus1;
        b2 = b2.fromMont().toMont();
        for (int j = 0; j < i; ++ j) {
            _ldouble(&a,&a);
            _ldouble(&b,&b);
            a2 = a2.dbl();
            b2 = b2.dbl();
        }
        auto c = a.multiply(b); 
        auto c2 = a2.multiply(b2);

        if (!c.equal(c2)) {
            cout << "multiplication 2^i(p-1) * 2^i(p-1) failed: i = "<< i;
            throw;
        }
        
    }

    // 2p * 2p will work
    for (int i = 0 ; i < 2; ++ i) {
        auto a = p;
        auto b = p;
        auto a2 = p;
        a2 = a2.fromMont().toMont();
        auto b2 = p;
        b2 = b2.fromMont().toMont();
        for (int j = 0; j < i; ++ j) {
            _ldouble(&a,&a);
            _ldouble(&b,&b);
            a2 = a2.dbl();
            b2 = b2.dbl();
        }
        auto c = a.multiply(b); 
        auto c2 = a2.multiply(b2);

        if (!c.equal(c2)) {
            cout << "multiplication 2^i(p) * 2^i(p) failed: i = "<< i;
            throw;
        }
        
    }

    {
        auto a = two383;
        auto b = two383;
        auto a2 = two383;
        a2 = a2.fromMont().toMont();
        auto b2 = two383;
        b2 = b2.fromMont().toMont();
        
        auto c = a.multiply(b); 
        auto c2 = a2.multiply(b2);

        if (!c.equal(c2)) {
            cout << "multiplication 2^383 * 2^383 failed";
            throw;
        }
    }
}

int main()
{
    TestScalar();

    TestFieldElementValidation();
    TestFieldElementEquality();
    TestFieldElementArithmeticCornerCases();
    TestFieldElementHelpers();
    TestFieldElementSerialization();
    TestFieldElementByteInputs();

    TestArithmeticOpraters();

    TestSqrt();
    TestInverse();
    TestMod();
    TestExp();

    TestG1Serialization();
    TestG1SerializationGarbage();
    TestG1IsOnCurve();
    TestG1AdditiveProperties();
    TestG1MultiplicativePropertiesExpected();
    TestG1MultiplicativeProperties();
    TestG1WeightedSumExpected();
    TestG1WeightedSumBatch();
    TestG1MapToCurve();

    TestG2Serialization();
    TestG2SerializationGarbage();
    TestG2IsOnCurve();
    TestG2AdditiveProperties();
    TestG2MultiplicativeProperties();
    TestG2WeightedSumExpected();
    TestG2WeightedSumBatch();
    TestG2MapToCurve();

    TestPairingExpected();
    TestPairingNonDegeneracy();
    TestPairingBilinearity();
    TestPairingMulti();

    TestsEIP2333();
    TestUnhardenedHDKeys();
    TestIETFVectors();
    TestChiaVectors();
    TestChiaVectors2();

    TestSignatures();
    TestAugScheme();
    TestAggregateSKs();
    TestPopScheme();

    TestExtraVectors();
    TestOutOfRangeInputs();

    return 0;
}
