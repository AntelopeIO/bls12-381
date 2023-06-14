#include <chrono>
#include <bls12-381.hpp>
#include <iostream>
#include <random>

using namespace std;
using namespace bls12_381;

std::chrono::time_point<std::chrono::steady_clock> startStopwatch() {
    return std::chrono::steady_clock::now();
}

void endStopwatch(string testName,
                  std::chrono::time_point<std::chrono::steady_clock> start,
                  int numIters) {
    auto end = std::chrono::steady_clock::now();
    auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            end - start);

    cout << endl << testName << endl;
    cout << "Total: " << numIters << " runs in " << now_ns.count()
         << " ns" << endl;
    cout << "Avg: " << now_ns.count() / static_cast<double>(numIters)
         << " ns" << endl;
}

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
    return g1::one().mulScalar(k);
}

g2 random_g2()
{
    array<uint64_t, 4> k = random_scalar();
    return g2::one().mulScalar(k);
}

g1 rand_g1()
{
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dis;

    return g1({fp({
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen)
    }),fp({
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen)
    }),fp({
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen)
    }),});
}
g2 rand_g2()
{
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dis;

    return g2({fp2({fp({
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen)
    }),fp({
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen)
    }),}),fp2({fp({
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen)
    }),fp({
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen)
    })}),fp2({fp({
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen)
    }),fp({
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen),
        dis(gen)
    })}),});
}

void benchG1Add() {
    string testName = "G1 Addition";
    const int numIters = 1000000;
    g1 p = random_g1();

    auto start = startStopwatch();

    for (int i = 0; i < numIters; i++) {
        p.add(p);
    }
    endStopwatch(testName, start, numIters);
}

void benchG1Mul() {
    string testName = "G1 Multiplication";
    const int numIters = 10000;
    g1 p = random_g1();
    array<uint64_t, 4> s = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};

    auto start = startStopwatch();

    for (int i = 0; i < numIters; i++) {
        p.mulScalar(s);
    }
    endStopwatch(testName, start, numIters);
}

void benchG2Add() {
    string testName = "G2 Addition";
    const int numIters = 1000000;
    g2 p = random_g2();

    auto start = startStopwatch();

    for (int i = 0; i < numIters; i++) {
        p.add(p);
    }
    endStopwatch(testName, start, numIters);
}

void benchG2Mul() {
    string testName = "G2 Multiplication";
    const int numIters = 10000;
    g2 p = random_g2();
    array<uint64_t, 4> s = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};

    auto start = startStopwatch();

    for (int i = 0; i < numIters; i++) {
        p.mulScalar(s);
    }
    endStopwatch(testName, start, numIters);
}

void benchPairing() {
    string testName = "Pairing";
    const int numIters = 10000;
    g1 g1One = g1::one();
    g2 g2One = g2::one();
    vector<tuple<g1, g2>> v;
    pairing::add_pair(v, g1One, g2One);

    auto start = startStopwatch();

    for (int i = 0; i < numIters; i++) {
        pairing::calculate(v);
    }
    endStopwatch(testName, start, numIters);
}

void measureMarshalling()
{
    string testName = "measureMarshalling";
    const int numIters = 96;
    g1 g1One = g1::one();
    g2 g2One = g2::one();
    array<uint64_t, 4> s = random_scalar();
    vector<tuple<g1, g2>> v;
    vector<bls12_381::g1> pv1;
    vector<bls12_381::g2> pv2;
    vector<array<uint64_t, 4>> sv;
    v.reserve(numIters);
    pv1.reserve(numIters);
    pv2.reserve(numIters);
    sv.reserve(numIters);

    std::array<uint8_t, 144> r1 = g1One.toJacobianBytesLE(true);
    std::array<uint8_t, 288> r2 = g2One.toJacobianBytesLE(true);

    //auto start = startStopwatch();
    for (int i = 0; i < numIters; i++) {
        scalar::fromBytesLE<4>(scalar::toBytesLE<4>(s));
        g1::fromJacobianBytesLE(r1, false, true);
        g2::fromJacobianBytesLE(r2, false, true);
        //pairing::add_pair(v, g1One, g2One);
        sv.push_back(s);
        pv1.push_back(g1One);
        pv2.push_back(g2One);
    }
    auto start = startStopwatch();
    //pairing::calculate(v);
    g1 r = g1::multiExp(pv1, sv);
    endStopwatch(testName, start, numIters);
    if(r.isZero()) benchPairing();
}

void benchPairing2() {
    string testName = "Pairing";
    const int numIters = 50000;
    g1 g1One = rand_g1();
    g2 g2One = rand_g2();
    vector<tuple<g1, g2>> v;
    v.reserve(numIters);

    auto start = startStopwatch();

    for (int i = 0; i < numIters; i++)
    {
        pairing::add_pair(v, g1One, g2One);
    }
    pairing::calculate(v);
    endStopwatch(testName, start, numIters);
}

int main(int argc, char* argv[])
{
    init();
    //benchG1Add();
    //benchG1Mul();
    //benchG2Add();
    //benchG2Mul();
    //benchPairing();
    measureMarshalling();
    //benchPairing2();
}