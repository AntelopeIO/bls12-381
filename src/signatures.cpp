#include <bls12-381/bls12-381.hpp>
#include "sha256.hpp"
#include <set>

using namespace std;

namespace bls12_381
{

// Domain Separation Tags
const string CIPHERSUITE_ID = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_NUL_";
const string POP_CIPHERSUITE_ID = "BLS_POP_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";

int hkdf256_hmac(
    uint8_t *mac,
    const uint8_t *in,
    uint64_t in_len,
    const uint8_t *key,
    uint64_t key_len
)
{
    const uint64_t block_size = 64;
    const uint32_t RLC_MD_LEN = 32;
    uint8_t opad[block_size + RLC_MD_LEN];
    uint8_t *ipad = reinterpret_cast<uint8_t*>(malloc(block_size + in_len));
    uint8_t _key[block_size];

    if(ipad == NULL)
    {
        //throw invalid_argument("hmac: could not allocate memory!");
        return -1;
    }

    if(key_len > block_size)
    {
        sha256 sha;
        sha.update(key, key_len);
        sha.digest(_key);
        key = _key;
        key_len = RLC_MD_LEN;
    }
    if(key_len <= block_size)
    {
        memcpy(_key, key, key_len);
        memset(_key + key_len, 0, block_size - key_len);
        key = _key;
    }
    for(uint64_t i = 0; i < block_size; i++)
    {
        opad[i] = 0x5C ^ key[i];
        ipad[i] = 0x36 ^ key[i];
    }
    memcpy(ipad + block_size, in, in_len);
    sha256 sha_opad;
    sha_opad.update(ipad, block_size + in_len);
    sha_opad.digest(opad + block_size);
    sha256 sha_mac;
    sha_mac.update(opad, block_size + RLC_MD_LEN);
    sha_mac.digest(mac);

    free(ipad);
    return 0;
}

int hkdf256_extract(
    uint8_t* prk_output,
    const uint8_t* salt,
    const size_t saltLen,
    const uint8_t* ikm,
    const size_t ikm_len
)
{
    // assert(saltLen == 4);  // Used for EIP2333 key derivation
    // assert(ikm_len == 32);  // Used for EIP2333 key derivation
    // Hash256 used as the hash function (sha256)
    // PRK Output is 32 bytes (HashLen)
    return hkdf256_hmac(prk_output, ikm, ikm_len, salt, saltLen);
}

int hkdf256_expand(
    uint8_t* okm,
    size_t L,
    const uint8_t* prk,
    const uint8_t* info,
    const size_t infoLen
)
{
    if(L > 255 * 32)
    {
        //throw invalid_argument("assert(L <= 255 * 32)");
        return -1;
    }
    if(infoLen < 0)
    {
        //throw invalid_argument("assert(infoLen >= 0)");
        return -2;
    }
    size_t N = (L + 32 - 1) / 32; // Round up
    size_t bytesWritten = 0;

    array<uint8_t, 32> T;
    uint8_t* hmacInput1 = reinterpret_cast<uint8_t*>(malloc(infoLen + 1));
    uint8_t* hmacInput = reinterpret_cast<uint8_t*>(malloc(32 + infoLen + 1));

    if(N < 1 || N > 255)
    {
        //throw invalid_argument("assert(N >= 1 && N <= 255)");
        return -3;
    }

    for(size_t i = 1; i <= N; i++)
    {
        if(i == 1)
        {
            memcpy(hmacInput1, info, infoLen);
            hmacInput1[infoLen] = i;
            hkdf256_hmac(T.data(), hmacInput1, infoLen + 1, prk, 32);
        }
        else
        {
            memcpy(hmacInput, T.data(), 32);
            memcpy(hmacInput + 32, info, infoLen);
            hmacInput[32 + infoLen] = i;
            hkdf256_hmac(T.data(), hmacInput, 32 + infoLen + 1, prk, 32);
        }
        size_t to_write = L - bytesWritten;
        if(to_write > 32)
        {
            to_write = 32;
        }
        if(to_write <= 0 || to_write > 32)
        {
            //throw invalid_argument("assert(to_write > 0 && to_write <= 32)");
            return -4;
        }
        memcpy(okm + bytesWritten, T.data(), to_write);
        bytesWritten += to_write;
    }
    free(hmacInput1);
    free(hmacInput);
    if(bytesWritten != L)
    {
        //throw invalid_argument("assert(bytesWritten == L)");
        return -5;
    }
    return 0;
}

int hkdf256_extract_expand(
    uint8_t* output,
    size_t outputLen,
    const uint8_t* key,
    size_t keyLen,
    const uint8_t* salt,
    size_t saltLen,
    const uint8_t* info,
    size_t infoLen
)
{
    array<uint8_t, 32> prk;
    int r = hkdf256_extract(prk.data(), salt, saltLen, key, keyLen);
    if(0 != r) return r;
    return hkdf256_expand(output, outputLen, prk.data(), info, infoLen);
}

array<uint64_t, 4> secret_key(std::span<const uint8_t> seed)
{
    // KeyGen
    // 1. PRK = HKDF-Extract("BLS-SIG-KEYGEN-SALT-", IKM || I2OSP(0, 1))
    // 2. OKM = HKDF-Expand(PRK, keyInfo || I2OSP(L, 2), L)
    // 3. SK = OS2IP(OKM) mod r
    // 4. return SK

    const uint8_t info[1] = {};
    const size_t infoLen = 0;

    // Required by the ietf spec to be at least 32 bytes
    if(seed.size() < 32)
    {
        //throw invalid_argument("Seed size must be at least 32 bytes");
        return {0, 0, 0, 0};
    }

    // "BLS-SIG-KEYGEN-SALT-" in ascii
    uint8_t saltHkdf[32] = {66, 76, 83, 45, 83, 73, 71, 45, 75, 69, 89, 71, 69, 78, 45, 83, 65, 76, 84, 45};
    uint8_t saltLen = 20;

    uint8_t *ikmHkdf = reinterpret_cast<uint8_t*>(malloc(seed.size() + 1));
    memcpy(ikmHkdf, &seed[0], seed.size());
    ikmHkdf[seed.size()] = 0;

    // `ceil((3 * ceil(log2(r))) / 16)`, where `r` is the order of the BLS 12-381 curve
    const uint8_t L = 48;
    array<uint8_t, L> okmHkdf;

    uint8_t keyInfoHkdf[infoLen + 2];
    memcpy(keyInfoHkdf, info, infoLen);
    keyInfoHkdf[infoLen] = 0;  // Two bytes for L, 0 and 48
    keyInfoHkdf[infoLen + 1] = L;

    array<uint64_t, 4> sk;
    while(true)
    {
        hkdf256_extract_expand(
            okmHkdf.data(),
            L,
            ikmHkdf,
            seed.size() + 1,
            saltHkdf,
            saltLen,
            keyInfoHkdf,
            infoLen + 2
        );

        // Make sure private key is less than the curve order
        array<uint64_t, 6> skBn = scalar::fromBytesBE<6>(span<uint8_t, 48>(okmHkdf.begin(), okmHkdf.end()));
        array<uint64_t, 6> quotient = {};

        bn_divn_safe(quotient, sk, skBn, fp::Q);

        if(!scalar::equal<4>(sk, {0, 0, 0, 0}))
        {
            break;
        }

        sk.fill(0);

        sha256 sha;
        sha.update(saltHkdf, saltLen);
        sha.digest(saltHkdf);
        saltLen = sizeof(saltHkdf);
    }

    free(ikmHkdf);

    return sk;
}

void ikm_to_lamport_sk(
    uint8_t* outputLamportSk,
    const uint8_t* ikm,
    size_t ikmLen,
    const uint8_t* salt,
    size_t saltLen
)
{
    // Expands the ikm to 255*HASH_LEN bytes for the lamport sk
    const uint8_t info[1] = {};
    hkdf256_extract_expand(outputLamportSk, 32 * 255, ikm, ikmLen, salt, saltLen, info, 0);
}

void parent_sk_to_lamport_pk(
    uint8_t* outputLamportPk,
    const array<uint64_t, 4>& parentSk,
    uint32_t index
)
{
    array<uint8_t, 4> salt;
    array<uint8_t, 32> ikm;
    array<uint8_t, 32> notIkm;
    array<uint8_t, 32 * 255> lamport0;
    array<uint8_t, 32 * 255> lamport1;

    for(size_t i = 0; i < 4; i++)
    {
        salt[3 - i] = (index >> (i * 8));
    }
    ikm = scalar::toBytesBE<4>(parentSk);

    // Flips the bits
    for(size_t i = 0; i < 32; i++)
    {
        notIkm[i] = ikm[i] ^ 0xff;
    }

    ikm_to_lamport_sk(lamport0.data(), ikm.data(), 32, salt.data(), 4);
    ikm_to_lamport_sk(lamport1.data(), notIkm.data(), 32, salt.data(), 4);

    array<uint8_t, 32 * 255 * 2> lamportPk;

    for(size_t i = 0; i < 255; i++)
    {
        sha256 sha;
        sha.update(lamport0.data() + i * 32, 32);
        sha.digest(lamportPk.data() + i * 32);
    }

    for(size_t i=0; i < 255; i++)
    {
        sha256 sha;
        sha.update(lamport1.data() + i * 32, 32);
        sha.digest(lamportPk.data() + 255 * 32 + i * 32);
    }
    sha256 sha;
    sha.update(lamportPk.data(), 32 * 255 * 2);
    sha.digest(outputLamportPk);
}

array<uint64_t, 4> derive_child_sk(
    const array<uint64_t, 4>& parentSk,
    uint32_t index
)
{
    array<uint8_t, 32> lamportPk;
    parent_sk_to_lamport_pk(lamportPk.data(), parentSk, index);
    vector<uint8_t> lamportPkVector(lamportPk.data(), lamportPk.data() + 32);
    array<uint64_t, 4> child = secret_key(lamportPkVector);
    return child;
}

array<uint64_t, 4> derive_child_sk_unhardened(
    const array<uint64_t, 4>& parentSk,
    uint32_t index
)
{
    array<uint8_t, 48 + 4> buf;
    array<uint8_t, 32> digest;
    memcpy(buf.data(), public_key(parentSk).toCompressedBytesBE().data(), 48);
    
    for(size_t i = 0; i < 4; i++)
    {
        buf[48 + 3 - i] = (index >> (i * 8));
    }
    sha256 sha;
    sha.update(buf.data(), 48 + 4);
    sha.digest(digest.data());

    array<uint64_t, 4> ret = aggregate_secret_keys(std::array{parentSk, sk_from_bytes(digest, true)});
    return ret;
}

g1 derive_child_g1_unhardened(
    const g1& pk,
    uint32_t index
)
{
    array<uint8_t, 48 + 4> buf;
    array<uint8_t, 32> digest;
    memcpy(buf.data(), g1(pk).toCompressedBytesBE().data(), 48);

    for(size_t i = 0; i < 4; i++)
    {
        buf[48 + 3 - i] = (index >> (i * 8));
    }
    sha256 sha;
    sha.update(buf.data(), 48 + 4);
    sha.digest(digest.data());

    array<uint64_t, 4> nonce = scalar::fromBytesBE<4>(span<uint8_t, 32>(digest.begin(), digest.end()));
    array<uint64_t, 4> quotient = {};
    array<uint64_t, 4> remainder = {};

    bn_divn_safe(quotient, remainder, nonce, fp::Q);

    return pk.add(g1::one().scale(remainder));
}

g2 derive_child_g2_unhardened(
    const g2& pk,
    uint32_t index
)
{
    array<uint8_t, 96 + 4> buf;
    array<uint8_t, 32> digest;
    memcpy(buf.data(), g2(pk).toCompressedBytesBE().data(), 96);
    
    for(size_t i = 0; i < 4; i++)
    {
        buf[96 + 3 - i] = (index >> (i * 8));
    }
    sha256 sha;
    sha.update(buf.data(), 96 + 4);
    sha.digest(digest.data());

    array<uint64_t, 4> nonce = scalar::fromBytesBE<4>(span<uint8_t, 32>(digest.begin(), digest.end()));
    array<uint64_t, 4> quotient = {};
    array<uint64_t, 4> remainder = {};

    bn_divn_safe(quotient, remainder, nonce, fp::Q);

    return pk.add(g2::one().scale(remainder));
}

array<uint64_t, 4> aggregate_secret_keys(std::span<const std::array<uint64_t, 4>> sks)
{
    if(sks.empty())
    {
        //throw std::length_error("Number of private keys must be at least 1");
        return {0, 0, 0, 0};
    }

    array<uint64_t, 4> state = {};
    array<uint64_t, 4> ret = {};
    for(uint64_t i = 0; i < sks.size(); i++)
    {
        state = scalar::add<4, 4, 4>(ret, sks[i]);
        ret.fill(0);
        array<uint64_t, 4> quotient = {};

        bn_divn_safe(quotient, ret, state, fp::Q);
    }

    return ret;
}

array<uint8_t, 32> sk_to_bytes(const array<uint64_t, 4>& sk)
{
    return scalar::toBytesBE<4>(sk);
}

array<uint64_t, 4> sk_from_bytes(
    const array<uint8_t, 32>& in,
    const bool modOrder
)
{
    array<uint64_t, 4> sk = scalar::fromBytesBE<4>(span<const uint8_t, 32>(in.begin(), in.end()));

    if(modOrder)
    {
        array<uint64_t, 4> quotient = {};
        array<uint64_t, 4> remainder = {};

        bn_divn_safe(quotient, remainder, sk, fp::Q);
        return remainder;
    }
    else
    {
        if(scalar::cmp<4>(sk, fp::Q) >= 0)
        {
            //throw std::invalid_argument("PrivateKey byte data must be less than the group order");
            return {0, 0, 0, 0};
        }
    }

    return sk;
}

g1 public_key(const array<uint64_t, 4>& sk)
{
    return g1::one().scale(sk).affine();
}

// Construct an extensible-output function based on SHA256
int xmd_sh256(
    uint8_t *buf,
    int buf_len,
    const uint8_t *in,
    int in_len,
    const uint8_t *dst,
    int dst_len
)
{
    const unsigned int SHA256HashSize = 32;
    const unsigned int SHA256_Message_Block_Size = 64;
    const unsigned ell = (buf_len + SHA256HashSize - 1) / SHA256HashSize;
    if (buf_len < 0 || ell > 255 || dst_len > 255)
    {
        return -1;
    }
    const uint8_t Z_pad[SHA256_Message_Block_Size] = { 0, };
    const uint8_t l_i_b_0_str[] = {
        static_cast<uint8_t>(buf_len >> 8),
        static_cast<uint8_t>(buf_len & 0xff),
        0,
        static_cast<uint8_t>(dst_len)
    };
    const uint8_t *dstlen_str = l_i_b_0_str + 3;
    uint8_t b_0[SHA256HashSize];
    sha256 sha;
    sha.update(Z_pad, SHA256_Message_Block_Size);
    sha.update(in, in_len);
    sha.update(l_i_b_0_str, 3);
    sha.update(dst, dst_len);
    sha.update(dstlen_str, 1);
    sha.digest(b_0);
    uint8_t b_i[SHA256HashSize + 1] = { 0, };
    for (unsigned i = 1; i <= ell; ++i)
    {
        for (unsigned j = 0; j < SHA256HashSize; ++j)
        {
            b_i[j] = b_0[j] ^ b_i[j];
        }
        b_i[SHA256HashSize] = i;
        sha256 s;
        s.update(b_i, SHA256HashSize + 1);
        s.update(dst, dst_len);
        s.update(dstlen_str, 1);
        s.digest(b_i);
        const int rem_after = buf_len - i * SHA256HashSize;
        const int copy_len = SHA256HashSize + (rem_after < 0 ? rem_after : 0);
        memcpy(buf + (i - 1) * SHA256HashSize, b_i, copy_len);
    }
    return 0;
}


g2 fromMessage(
    std::span<const uint8_t> msg,
    const string& dst
)
{
    uint8_t buf[4 * 64];
    xmd_sh256(buf, 4 * 64, msg.data(), msg.size(), reinterpret_cast<const uint8_t*>(dst.c_str()), dst.length());

    array<uint64_t, 8> k = {};
    fp2 t = fp2::zero();
    fp2 x, y, z = fp2::one();
    g2 p, q;

    k = scalar::fromBytesBE<8>(span<uint8_t, 64>(buf, buf + 64));
    t.c0 = fp::modPrime(k);
    k = scalar::fromBytesBE<8>(span<uint8_t, 64>(buf + 64, buf + 2*64));
    t.c1 = fp::modPrime(k);

    tie(x, y) = g2::swuMapG2(t);
    p = g2({x, y, z}).isogenyMap();

    k = scalar::fromBytesBE<8>(span<uint8_t, 64>(buf + 2*64, buf + 3*64));
    t.c0 = fp::modPrime(k);
    k = scalar::fromBytesBE<8>(span<uint8_t, 64>(buf + 3*64, buf + 4*64));
    t.c1 = fp::modPrime(k);

    tie(x, y) = g2::swuMapG2(t);
    q = g2({x, y, z}).isogenyMap();

    return p.add(q).clearCofactor();
}

g2 sign(
    const array<uint64_t, 4>& sk,
    std::span<const uint8_t> msg
)
{
    g2 p = fromMessage(msg, CIPHERSUITE_ID);
    return p.scale(sk);
}

bool verify(
    const g1& pubkey,
    std::span<const uint8_t> message,
    const g2& signature
)
{
    vector<tuple<g1, g2>> v;
    pairing::add_pair(v, g1::one().negate(), signature);
    const g2 hashedPoint = fromMessage(message, CIPHERSUITE_ID);
    pairing::add_pair(v, pubkey, hashedPoint);

    if(!pubkey.isOnCurve() || !pubkey.inCorrectSubgroup())
    {
        return false;
    }
    if(!signature.isOnCurve() || !signature.inCorrectSubgroup())
    {
        return false;
    }

    // 1 =? prod e(pubkey[i], hash[i]) * e(-g1, aggSig)
    return fp12::one().equal(pairing::calculate(v));
}

g1 aggregate_public_keys(std::span<const g1> pks)
{
    g1 agg_pk = g1({fp::zero(), fp::zero(), fp::zero()});
    for(const g1& pk : pks)
    {
        agg_pk = agg_pk.add(pk);
    }
    return agg_pk;
}

g2 aggregate_signatures(std::span<const g2> sigs)
{
    g2 agg_sig = g2({fp2::zero(), fp2::zero(), fp2::zero()});
    for(const g2& sig : sigs)
    {
        agg_sig = agg_sig.add(sig);
    }
    return agg_sig;
}

bool aggregate_verify(
    std::span<const g1> pubkeys,
    std::span<const std::vector<uint8_t>> messages,
    const g2& signature,
    const bool checkForDuplicateMessages
)
{
    vector<tuple<g1, g2>> v;
    pairing::add_pair(v, g1::one().negate(), signature);

    if(!signature.isOnCurve() || !signature.inCorrectSubgroup())
    {
        return false;
    }

    if(pubkeys.size() != messages.size())
    {
        return false;
    }

    if(checkForDuplicateMessages)
    {
        // A std::set will filter out duplicates on calls of 'insert'
        set<vector<uint8_t>> setMessages;
        for(const auto& message : messages)
        {
            setMessages.insert({message.begin(), message.end()});
        }
        // if it still has the same size as the vector from before all messages are unique
        if(setMessages.size() != pubkeys.size())
        {
            return false;
        }
    }

    for(size_t i = 0; i < pubkeys.size(); i++)
    {
        if(!pubkeys[i].isOnCurve() || !pubkeys[i].inCorrectSubgroup())
        {
            return false;
        }
        pairing::add_pair(v, pubkeys[i], fromMessage(messages[i], CIPHERSUITE_ID));
    }

    // 1 =? prod e(pubkey[i], hash[i]) * e(-g1, aggSig)
    return fp12::one().equal(pairing::calculate(v));
}

g2 pop_prove(const array<uint64_t, 4>& sk)
{
    g1 pk = public_key(sk);
    array<uint8_t, 96> msg = pk.toAffineBytesLE(from_mont::yes);
    g2 hashed_key = fromMessage(vector<uint8_t>(msg.begin(), msg.end()), POP_CIPHERSUITE_ID);
    return hashed_key.scale(sk);
}

bool pop_verify(
    const g1& pubkey,
    const g2& signature_proof
)
{
    array<uint8_t, 96> msg = pubkey.toAffineBytesLE(from_mont::yes);
    const g2 hashedPoint = fromMessage(vector<uint8_t>(msg.begin(), msg.end()), POP_CIPHERSUITE_ID);

    if(!pubkey.isOnCurve() || !pubkey.inCorrectSubgroup())
    {
        return false;
    }
    if(!signature_proof.isOnCurve() || !signature_proof.inCorrectSubgroup())
    {
        return false;
    }

    vector<tuple<g1, g2>> v;
    pairing::add_pair(v, g1::one().negate(), signature_proof);
    pairing::add_pair(v, pubkey, hashedPoint);

    // 1 =? prod e(pubkey[i], hash[i]) * e(-g1, aggSig)
    return fp12::one().equal(pairing::calculate(v));
}

bool pop_fast_aggregate_verify(
    std::span<const g1> pubkeys,
    std::span<const uint8_t> message,
    const g2& signature
)
{
    if(pubkeys.size() == 0)
    {
        return false;
    }

    return verify(aggregate_public_keys(pubkeys), message, signature);
}

} // namespace bls12_381
