#pragma once
#include <array>
#include <vector>

namespace bls12_381
{

class fp12;
class g1;
class g2;

// This is the equivalent of the [groth16](https://github.com/zkcrypto/bellman/tree/main/src/groth16) module of
// the zkcrypto bellman Rust crate (verifier part only).
namespace groth16
{
    class Proof
    {
    public:
        g1 a;
        g2 b;
        g1 c;

        Proof();
        Proof(const g1& a, const g2& b, const g1& c);
        Proof(const Proof& proof);
        static Proof fromJacobianBytesBE(const std::span<const uint8_t, 576> in, const bool check = false, const bool raw = false);
        static Proof fromJacobianBytesLE(const std::span<const uint8_t, 576> in, const bool check = false, const bool raw = false);
        static Proof fromAffineBytesBE(const std::span<const uint8_t, 384> in, const bool check = false, const bool raw = false);
        static Proof fromAffineBytesLE(const std::span<const uint8_t, 384> in, const bool check = false, const bool raw = false);
        static Proof fromCompressedBytesBE(const std::span<const uint8_t, 192> in);
        void toJacobianBytesBE(const std::span<uint8_t, 576> out, const bool raw = false) const;
        void toJacobianBytesLE(const std::span<uint8_t, 576> out, const bool raw = false) const;
        void toAffineBytesBE(const std::span<uint8_t, 384> out, const bool raw = false) const;
        void toAffineBytesLE(const std::span<uint8_t, 384> out, const bool raw = false) const;
        void toCompressedBytesBE(const std::span<uint8_t, 192> out) const;
        std::array<uint8_t, 576> toJacobianBytesBE(const bool raw = false) const;
        std::array<uint8_t, 576> toJacobianBytesLE(const bool raw = false) const;
        std::array<uint8_t, 384> toAffineBytesBE(const bool raw = false) const;
        std::array<uint8_t, 384> toAffineBytesLE(const bool raw = false) const;
        std::array<uint8_t, 192> toCompressedBytesBE() const;
        bool equal(const Proof& proof) const;
    };

    class VerifyingKey
    {
    public:
        // alpha in g1 for verifying and for creating A/C elements of proof. Never the point at infinity.
        g1 alpha_g1;
        // beta in g1 and g2 for verifying and for creating B/C elements of proof. Never the point at infinity.
        g1 beta_g1;
        g2 beta_g2;
        // gamma in g2 for verifying. Never the point at infinity.
        g2 gamma_g2;
        // delta in g1/g2 for verifying and proving, essentially the magic trapdoor that forces the prover to
        // evaluate the C element of the proof with only components from the CRS. Never the point at infinity.
        g1 delta_g1;
        g2 delta_g2;
        // Elements of the form (beta * u_i(tau) + alpha v_i(tau) + w_i(tau)) / gamma for all public inputs.
        // Because all public inputs have a dummy constraint, this is the same size as the number of inputs,
        // and never contains points at infinity.
        std::vector<g1> ic;

        VerifyingKey();
        VerifyingKey(
            const g1& alpha_g1,
            const g1& beta_g1,
            const g2& beta_g2,
            const g2& gamma_g2,
            const g1& delta_g1,
            const g2& delta_g2,
            const std::vector<g1>& ic
        );
        VerifyingKey(const VerifyingKey& vk);
        static VerifyingKey fromJacobianBytesBE(const uint8_t* in, const bool check = false, const bool raw = false);
        static VerifyingKey fromJacobianBytesLE(const uint8_t* in, const bool check = false, const bool raw = false);
        static VerifyingKey fromAffineBytesBE(const uint8_t* in, const bool check = false, const bool raw = false);
        static VerifyingKey fromAffineBytesLE(const uint8_t* in, const bool check = false, const bool raw = false);
        static VerifyingKey fromCompressedBytesBE(const uint8_t* in);
        void toJacobianBytesBE(uint8_t* out, const bool raw = false) const;
        void toJacobianBytesLE(uint8_t* out, const bool raw = false) const;
        void toAffineBytesBE(uint8_t* out, const bool raw = false) const;
        void toAffineBytesLE(uint8_t* out, const bool raw = false) const;
        void toCompressedBytesBE(uint8_t* out) const;
        std::vector<uint8_t> toJacobianBytesBE(const bool raw = false) const;
        std::vector<uint8_t> toJacobianBytesLE(const bool raw = false) const;
        std::vector<uint8_t> toAffineBytesBE(const bool raw = false) const;
        std::vector<uint8_t> toAffineBytesLE(const bool raw = false) const;
        std::vector<uint8_t> toCompressedBytesBE() const;
        bool equal(const VerifyingKey& vk) const;
    };

    class PreparedVerifyingKey
    {
    public:
        // Pairing result of alpha*beta
        fp12 alpha_g1_beta_g2;
        // -gamma in G2
        g2 neg_gamma_g2;
        // -delta in G2
        g2 neg_delta_g2;
        // Copy of 'ic' from `vk`
        std::vector<g1> ic;

        PreparedVerifyingKey();
        PreparedVerifyingKey(
            const fp12& alpha_g1_beta_g2,
            const g2& neg_gamma_g2,
            const g2& neg_delta_g2,
            const std::vector<g1>& ic
        );
        PreparedVerifyingKey(const PreparedVerifyingKey& pvk);
        static PreparedVerifyingKey fromBytesBE(const uint8_t* in, const bool check = false, const bool raw = false);
        static PreparedVerifyingKey fromBytesLE(const uint8_t* in, const bool check = false, const bool raw = false);
        void toBytesBE(uint8_t* out, const bool raw = false) const;
        void toBytesLE(uint8_t* out, const bool raw = false) const;
        std::vector<uint8_t> toBytesBE(const bool raw = false) const;
        std::vector<uint8_t> toBytesLE(const bool raw = false) const;
        bool equal(const PreparedVerifyingKey& pvk) const;
    };

    PreparedVerifyingKey prepare_verifying_key(const VerifyingKey& vk);

    bool verify_proof(
        const PreparedVerifyingKey& pvk,
        const Proof& proof,
        std::vector<std::array<uint64_t, 4>>& public_inputs
    );

} // namespace groth16
} // namespace bls12_381