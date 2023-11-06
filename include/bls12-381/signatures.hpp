#include <cstdint>
#include <cstdlib>
#include <array>
#include <span>
#include <vector>
#include <string>
#include <bls12-381/g.hpp>
#include <bls12-381/fp.hpp>

namespace bls12_381 {

class g1;
class g2;

extern const std::string CIPHERSUITE_ID;
extern const std::string POP_CIPHERSUITE_ID;

// Used to generate a domain separated extended sha256 hash used in 'map to curve'
int xmd_sh256(
    uint8_t *buf,
    int buf_len,
    const uint8_t *in,
    int in_len,
    const uint8_t *dst,
    int dst_len
);

// Implements HMAC based on SHA256 as specified in RFC 2104: https://www.rfc-editor.org/rfc/rfc2104
int hkdf256_hmac(
    uint8_t *mac,
    const uint8_t *in,
    uint64_t in_len,
    const uint8_t *key,
    uint64_t key_len
);

// Implements HKDF Extract as specified in RFC 5869: https://www.rfc-editor.org/rfc/rfc5869#section-2.2
int hkdf256_extract(
    uint8_t* prk_output,
    const uint8_t* salt,
    const size_t saltLen,
    const uint8_t* ikm,
    const size_t ikm_len
);

// Implements HKDF Expand as specified in RFC 5869: https://www.rfc-editor.org/rfc/rfc5869#section-2.3
int hkdf256_expand(
    uint8_t* okm,
    size_t L,
    const uint8_t* prk,
    const uint8_t* info,
    const size_t infoLen
);

// Implements HKDF Extract + Expand as specified in RFC 5869: https://www.rfc-editor.org/rfc/rfc5869
int hkdf256_extract_expand(
    uint8_t* output,
    size_t outputLen,
    const uint8_t* key,
    size_t keyLen,
    const uint8_t* salt,
    size_t saltLen,
    const uint8_t* info,
    size_t infoLen
);

// Implements IKM To Lamport SK as specified in EIP 2333: https://eips.ethereum.org/EIPS/eip-2333#ikm_to_lamport_sk
void ikm_to_lamport_sk(
    uint8_t* outputLamportSk,
    const uint8_t* ikm,
    size_t ikmLen,
    const uint8_t* salt,
    size_t saltLen
);

// Implements Parent SK To Lamport PK as specified in EIP 2333: https://eips.ethereum.org/EIPS/eip-2333#parent_sk_to_lamport_pk
void parent_sk_to_lamport_pk(
    uint8_t* outputLamportPk,
    const std::array<uint64_t, 4>& parentSk,
    uint32_t index
);

// Implements Derive Child SK as specified in EIP 2333: https://eips.ethereum.org/EIPS/eip-2333#derive_child_sk
std::array<uint64_t, 4> derive_child_sk(
    const std::array<uint64_t, 4>& parentSk,
    uint32_t index
);

std::array<uint64_t, 4> derive_child_sk_unhardened(
    const std::array<uint64_t, 4>& parentSk,
    uint32_t index
);

g1 derive_child_g1_unhardened(
    const g1& pk,
    uint32_t index
);

g2 derive_child_g2_unhardened(
    const g2& pk,
    uint32_t index
);

// Implements secret key derivation based on HKDF Mod R as specified in EIP 2333: https://eips.ethereum.org/EIPS/eip-2333#hkdf_mod_r
std::array<uint64_t, 4> secret_key(std::span<const uint8_t> seed);

// Derive public key from a BLS private key
g1 public_key(const std::array<uint64_t, 4>& sk);

g2 fromMessage(
    std::span<const uint8_t> msg,
    const std::string& dst
);

// Sign message with a private key
g2 sign(
    const std::array<uint64_t, 4>& sk,
    std::span<const uint8_t> msg
);

// Verify signature of a message using a public key
bool verify(
    const g1& pubkey,
    std::span<const uint8_t> message,
    const g2& signature
);

// Aggregate private keys
std::array<uint64_t, 4> aggregate_secret_keys(std::span<const std::array<uint64_t, 4>> sks);

// Aggregate public keys
g1 aggregate_public_keys(std::span<const g1> pks);

// Aggregate signatures
g2 aggregate_signatures(std::span<const g2> sigs);

// Aggregate verify using a set of public keys, a set of messages and an aggregated signature
// the boolean parameter enables an additional check for duplicate messages (possible attack
// vector: see page 6 of https://crypto.stanford.edu/~dabo/pubs/papers/aggreg.pdf, "A potential
// attack on aggregate signatures.")
bool aggregate_verify(
    std::span<const g1> pubkeys,
    std::span<const std::vector<uint8_t>> messages,
    const g2& signature,
    const bool checkForDuplicateMessages = false
);

// `f` is an accessor function in case we have a span of objects of type T containing public keys: `g1 f(const T&)`
// `pred` can be used to aggregate public keys specified in a bit mask for example: `bool pred(const T&, size_t)`
template<class T, class F, class PRED = decltype([](const T&, size_t){return true;})>
inline auto aggregate_public_keys(std::span<T> pks, F&& f, PRED&& pred = PRED())
    -> decltype(f(*pks.data()), pred(*pks.data(), 0), g1()) {
    g1 agg_pk = g1({fp::zero(), fp::zero(), fp::zero()});
    for (size_t i=0; i<pks.size(); ++i)
        if (std::forward<PRED>(pred)(pks[i], i))
            agg_pk = agg_pk.add(std::forward<F>(f)(pks[i]));
    return agg_pk;
}

// f is an accessor function in case we have a span of objects of type T containing signatures: `g2 f(const T&)`
// pred can be used to aggregate signatures specified in a bit mask for example: `bool pred(const T&, size_t)`
template<class T, class F, class PRED = decltype([](const T&, size_t){return true;})>
inline auto aggregate_signatures(std::span<T> sigs, F&& f, PRED&& pred = PRED()) 
    -> decltype(f(*sigs.data()), pred(*sigs.data(), 0), g2()) {
    g2 agg_sig = g2({fp2::zero(), fp2::zero(), fp2::zero()});
    for (size_t i=0; i<sigs.size(); ++i)
        if (std::forward<PRED>(pred)(sigs[i], i))
            agg_sig = agg_sig.add(std::forward<F>(f)(sigs[i]));
    return agg_sig;
}

// Create new BLS private key from bytes. Enable modulo division to ensure scalar is element of the field
std::array<uint64_t, 4> sk_from_bytes(
    const std::array<uint8_t, 32>& in,
    const bool modOrder = false
);

// Write private key as bytes
std::array<uint8_t, 32> sk_to_bytes(const std::array<uint64_t, 4>& sk);

// Proof Of Possession Scheme: Prove
g2 pop_prove(const std::array<uint64_t, 4>& sk);

bool pop_verify(
    const g1& pubkey,
    const g2& signature_proof
);

bool pop_fast_aggregate_verify(
    std::span<const g1> pubkeys,
    std::span<const uint8_t> message,
    const g2& signature
);

} // namespace bls12_381
