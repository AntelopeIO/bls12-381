#include <cstdint>
#include <cstdlib>
#include <array>
#include <vector>
#include <string>

using namespace std;

namespace bls12_381
{

class g1;
class g2;

extern const string CIPHERSUITE_ID;
extern const string POP_CIPHERSUITE_ID;

// Used to generate a domain separated extended sha256 hash used in 'map to curve'
void xmd_sh256(
    uint8_t *buf,
    int buf_len,
    const uint8_t *in,
    int in_len,
    const uint8_t *dst,
    int dst_len
);

// Implements HMAC based on SHA256 as specified in RFC 2104: https://www.rfc-editor.org/rfc/rfc2104
void hkdf256_hmac(
    uint8_t *mac,
    const uint8_t *in,
    uint64_t in_len,
    const uint8_t *key,
    uint64_t key_len
);

// Implements HKDF Extract as specified in RFC 5869: https://www.rfc-editor.org/rfc/rfc5869#section-2.2
void hkdf256_extract(
    uint8_t* prk_output,
    const uint8_t* salt,
    const size_t saltLen,
    const uint8_t* ikm,
    const size_t ikm_len
);

// Implements HKDF Expand as specified in RFC 5869: https://www.rfc-editor.org/rfc/rfc5869#section-2.3
void hkdf256_expand(
    uint8_t* okm,
    size_t L,
    const uint8_t* prk,
    const uint8_t* info,
    const size_t infoLen
);

// Implements HKDF Extract + Expand as specified in RFC 5869: https://www.rfc-editor.org/rfc/rfc5869
void hkdf256_extract_expand(
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
    const array<uint64_t, 4>& parentSk,
    uint32_t index
);

// Implements Derive Child SK as specified in EIP 2333: https://eips.ethereum.org/EIPS/eip-2333#derive_child_sk
array<uint64_t, 4> derive_child_sk(
    const array<uint64_t, 4>& parentSk,
    uint32_t index
);

array<uint64_t, 4> derive_child_sk_unhardened(
    const array<uint64_t, 4>& parentSk,
    uint32_t index
);

g1 derive_shild_g1_unhardened(
    const g1& pk,
    uint32_t index
);

g2 derive_child_g2_unhardened(
    const g2& pk,
    uint32_t index
);

// Implements secret key derivation based on HKDF Mod R as specified in EIP 2333: https://eips.ethereum.org/EIPS/eip-2333#hkdf_mod_r
array<uint64_t, 4> secret_key(const vector<uint8_t>& seed);

// Derive public key from a BLS private key
g1 public_key(const array<uint64_t, 4>& sk);

// Sign message with a private key
g2 sign(
    const array<uint64_t, 4>& sk,
    const vector<uint8_t>& msg
);

// Verify signature of a message using a public key
bool verify(
    const g1& pubkey,
    const vector<uint8_t>& message,
    const g2& signature
);

// Aggregate private keys
array<uint64_t, 4> aggregate_secret_keys(const vector<array<uint64_t, 4>>& sks);

// Aggregate public keys
g1 aggregate_public_keys(const vector<g1>& pks);

// Aggregate signatures
g2 aggregate_signatures(const vector<g2>& sigs);

// Aggregate verify using a set of public keys, a set of messages and an aggregated signature
// the boolean parameter enables an additional check for dublicate messages (possible attack
// vector: see page 6 of https://crypto.stanford.edu/~dabo/pubs/papers/aggreg.pdf, "A potential
// attack on aggregate signatures.")
bool aggregate_verify(
    const vector<g1>& pubkeys,
    const vector<vector<uint8_t>> &messages,
    const g2& signature,
    const bool checkForDuplicateMessages = false
);

// Create new BLS private key from bytes. Enable modulo division to ensure scalar is element of the field
array<uint64_t, 4> sk_from_bytes(
    const array<uint8_t, 32>& in,
    const bool modOrder = false
);

// Write private key as bytes
array<uint8_t, 32> sk_to_bytes(const array<uint64_t, 4>& sk);

// Proof Of Possession Scheme: Prove
g2 pop_prove(const array<uint64_t, 4>& sk);

bool pop_verify(
    const g1& pubkey,
    const g2& signature_proof
);

bool pop_fast_aggregate_verify(
    const vector<g1>& pubkeys,
    const vector<uint8_t>& message,
    const g2& signature
);

} // namespace bls12_381