#include <chrono>
#include <bls12-381/bls12-381.hpp>
#include <iostream>
#include <random>

using std::string;
using std::vector;
using std::array;
using std::cout;
using std::endl;

using namespace bls12_381;

std::chrono::time_point<std::chrono::steady_clock> startStopwatch() {
    return std::chrono::steady_clock::now();
}

void endStopwatch(string testName,
                  std::chrono::time_point<std::chrono::steady_clock> start,
                  int numIters) {
    auto end = std::chrono::steady_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start);

    cout << '\n' << testName << '\n';
    cout << "Total: " << numIters << " runs in " << now_ms.count()
         << " ms" << '\n';
    cout << "Avg: " << now_ms.count() / static_cast<double>(numIters)
         << " ms" << '\n';
}

std::array<uint8_t, 32> getRandomSeed() {
    std::array<uint64_t, 4> buf;
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    buf[0] = dis(gen);
    buf[1] = dis(gen);
    buf[2] = dis(gen);
    buf[3] = dis(gen);
    std::array<uint8_t, 32> res;
    memcpy(res.data(), buf.data(), sizeof(res));
    return res;
}

void IntToFourBytes(uint8_t* result,
                    const uint32_t input) {
    for (size_t i = 0; i < 4; i++) {
        result[3 - i] = (input >> (i * 8));
    }
}

void benchSigs() {
    string testName = "Signing";
    const int numIters = 5000;
    array<uint64_t, 4> sk = secret_key(getRandomSeed());
    array<uint8_t, 48UL> pk = public_key(sk).toCompressedBytesBE();

    auto start = startStopwatch();

    for (int i = 0; i < numIters; i++) {
        sign(sk, pk);
    }
    endStopwatch(testName, start, numIters);
}

void benchVerification() {
    string testName = "Verification";
    const int numIters = 10000;
    array<uint64_t, 4> sk = secret_key(getRandomSeed());
    g1 pk = public_key(sk);

    std::vector<g2> sigs;

    for (int i = 0; i < numIters; i++) {
        uint8_t message[4];
        IntToFourBytes(message, i);
        sigs.push_back(sign(sk, message));
    }

    auto start = startStopwatch();
    for (int i = 0; i < numIters; i++) {
        uint8_t message[4];
        IntToFourBytes(message, i);
        bool ok = verify(pk, message, sigs[i]);
        if(!ok) throw std::invalid_argument("benchVerification: !ok");
    }
    endStopwatch(testName, start, numIters);
}

void benchBatchVerification() {
    const int numIters = 100000;

    vector<array<uint8_t, 96UL>> sig_bytes;
    vector<array<uint8_t, 48UL>> pk_bytes;
    vector<vector<uint8_t>> ms;

    for (int i = 0; i < numIters; i++) {
        uint8_t message[4];
        IntToFourBytes(message, i);
        vector<uint8_t> messageBytes(message, message + 4);
        array<uint64_t, 4> sk = secret_key(getRandomSeed());
        g1 pk = public_key(sk);
        sig_bytes.push_back(sign(sk, messageBytes).toCompressedBytesBE());
        pk_bytes.push_back(pk.toCompressedBytesBE());
        ms.push_back(messageBytes);
    }

    vector<g1> pks;
    pks.reserve(numIters);

    auto start = startStopwatch();
    for (auto const& pk : pk_bytes) {
        pks.emplace_back(g1::fromCompressedBytesBE(pk).value());
    }
    endStopwatch("Public key validation", start, numIters);

    vector<g2> sigs;
    sigs.reserve(numIters);

    start = startStopwatch();
    for (auto const& sig : sig_bytes) {
        sigs.emplace_back(g2::fromCompressedBytesBE(sig).value());
    }
    endStopwatch("Signature validation", start, numIters);

    start = startStopwatch();
    g2 aggSig = aggregate_signatures(sigs);
    endStopwatch("Aggregation", start, numIters);

    start = startStopwatch();
    bool ok = aggregate_verify(pks, ms, aggSig);
    if(!ok) throw std::invalid_argument("benchBatchVerification: !ok");
    endStopwatch("Batch verification", start, numIters);
}

void benchFastAggregateVerification() {
    const int numIters = 5000;

    vector<g2> sigs;
    vector<g1> pks;
    vector<uint8_t> message = {1, 2, 3, 4, 5, 6, 7, 8};
    vector<g2> pops;

    for (int i = 0; i < numIters; i++) {
        array<uint64_t, 4> sk = secret_key(getRandomSeed());
        g1 pk = public_key(sk);
        sigs.push_back(sign(sk, message));
        pops.push_back(pop_prove(sk));
        pks.push_back(pk);
    }

    auto start = startStopwatch();
    g2 aggSig = aggregate_signatures(sigs);
    endStopwatch("PopScheme Aggregation", start, numIters);


    start = startStopwatch();
    for (int i = 0; i < numIters; i++) {
        bool ok = pop_verify(pks[i], pops[i]);
        if(!ok) throw std::invalid_argument("benchBatchVerification, pop_verify: !ok");
    }
    endStopwatch("PopScheme Proofs verification", start, numIters);

    start = startStopwatch();
    bool ok = pop_fast_aggregate_verify(pks, message, aggSig);
    if(!ok) throw std::invalid_argument("benchBatchVerification, pop_fast_aggregate_verify: !ok");
    endStopwatch("PopScheme verification", start, numIters);
}

int main(int argc, char* argv[])
{
    benchSigs();
    benchVerification();
    benchBatchVerification();
    benchFastAggregateVerification();
}