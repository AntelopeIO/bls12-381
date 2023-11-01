#include <chrono>
#include <bls12-381/bls12-381.hpp>
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
    return g1::one().scale(k);
}

g2 random_g2()
{
    array<uint64_t, 4> k = random_scalar();
    return g2::one().scale(k);
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
        p.scale(s);
    }
    endStopwatch(testName, start, numIters);
}

void benchG1WeightedSum() {
    string testName = "G2 WeightedSum";
    const int numIters = 10000;
    g1 p = random_g1();
    vector<g1> bases = {p,p,p,p,p,p,p,p};
    vector<array<uint64_t, 4>> scalars = {
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff}
    };
    auto start = startStopwatch();
    for (int i = 0; i < numIters; i++) {
        g1::weightedSum(bases, scalars);
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
        p.scale(s);
    }
    endStopwatch(testName, start, numIters);
}

void benchG2WeightedSum() {
    string testName = "G2 WeightedSum";
    const int numIters = 10000;
    g2 p = random_g2();
    vector<g2> bases = {p,p,p,p,p,p,p,p};
    vector<array<uint64_t, 4>> scalars = {
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff},
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff}
    };
    auto start = startStopwatch();
    for (int i = 0; i < numIters; i++) {
        g2::weightedSum(bases, scalars);
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

int main(int argc, char* argv[])
{
    benchG1Add();
    benchG1Mul();
    benchG1WeightedSum();
    benchG2Add();
    benchG2Mul();
    benchG2WeightedSum();
    benchPairing();
}