#include "../include/bls12_381.hpp"

using namespace std;

namespace bls12_381
{
namespace groth16
{

Proof::Proof() : a(g1::zero()), b(g2::zero()), c(g1::zero())
{
}

Proof::Proof(const g1& a, const g2& b, const g1& c) : a(a), b(b), c(c)
{
}

Proof::Proof(const Proof& proof) : a(proof.a), b(proof.b), c(proof.c)
{
}

Proof Proof::fromJacobianBytesBE(const span<const uint8_t, 576> in, const bool check, const bool raw)
{
    g1 a = g1::fromJacobianBytesBE(span<const uint8_t, 144>(&in[  0], &in[144]), check, raw);
    g2 b = g2::fromJacobianBytesBE(span<const uint8_t, 288>(&in[144], &in[432]), check, raw);
    g1 c = g1::fromJacobianBytesBE(span<const uint8_t, 144>(&in[432], &in[576]), check, raw);
    return Proof(a, b, c);
}

Proof Proof::fromJacobianBytesLE(const span<const uint8_t, 576> in, const bool check, const bool raw)
{
    g1 a = g1::fromJacobianBytesLE(span<const uint8_t, 144>(&in[  0], &in[144]), check, raw);
    g2 b = g2::fromJacobianBytesLE(span<const uint8_t, 288>(&in[144], &in[432]), check, raw);
    g1 c = g1::fromJacobianBytesLE(span<const uint8_t, 144>(&in[432], &in[576]), check, raw);
    return Proof(a, b, c);
}

Proof Proof::fromAffineBytesBE(const span<const uint8_t, 384> in, const bool check, const bool raw)
{
    g1 a = g1::fromAffineBytesBE(span<const uint8_t,  96>(&in[  0], &in[ 96]), check, raw);
    g2 b = g2::fromAffineBytesBE(span<const uint8_t, 192>(&in[ 96], &in[288]), check, raw);
    g1 c = g1::fromAffineBytesBE(span<const uint8_t,  96>(&in[288], &in[384]), check, raw);
    return Proof(a, b, c);
}

Proof Proof::fromAffineBytesLE(const span<const uint8_t, 384> in, const bool check, const bool raw)
{
    g1 a = g1::fromAffineBytesLE(span<const uint8_t,  96>(&in[  0], &in[ 96]), check, raw);
    g2 b = g2::fromAffineBytesLE(span<const uint8_t, 192>(&in[ 96], &in[288]), check, raw);
    g1 c = g1::fromAffineBytesLE(span<const uint8_t,  96>(&in[288], &in[384]), check, raw);
    return Proof(a, b, c);
}

Proof Proof::fromCompressedBytesBE(const span<const uint8_t, 192> in)
{
    g1 a = g1::fromCompressedBytesBE(span<const uint8_t, 48>(&in[  0], &in[ 48]));
    g2 b = g2::fromCompressedBytesBE(span<const uint8_t, 96>(&in[ 48], &in[144]));
    g1 c = g1::fromCompressedBytesBE(span<const uint8_t, 48>(&in[144], &in[192]));
    return Proof(a, b, c);
}

void Proof::toJacobianBytesBE(const span<uint8_t, 576> out, const bool raw) const
{
    memcpy(&out[  0], &a.toJacobianBytesBE(raw)[0], 144);
    memcpy(&out[144], &b.toJacobianBytesBE(raw)[0], 288);
    memcpy(&out[432], &c.toJacobianBytesBE(raw)[0], 144);
}

void Proof::toJacobianBytesLE(const span<uint8_t, 576> out, const bool raw) const
{
    memcpy(&out[  0], &a.toJacobianBytesLE(raw)[0], 144);
    memcpy(&out[144], &b.toJacobianBytesLE(raw)[0], 288);
    memcpy(&out[432], &c.toJacobianBytesLE(raw)[0], 144);
}

void Proof::toAffineBytesBE(const span<uint8_t, 384> out, const bool raw) const
{
    memcpy(&out[  0], &a.toAffineBytesBE(raw)[0], 96);
    memcpy(&out[ 96], &b.toAffineBytesBE(raw)[0], 192);
    memcpy(&out[288], &c.toAffineBytesBE(raw)[0], 96);
}

void Proof::toAffineBytesLE(const span<uint8_t, 384> out, const bool raw) const
{
    memcpy(&out[  0], &a.toAffineBytesLE(raw)[0], 96);
    memcpy(&out[ 96], &b.toAffineBytesLE(raw)[0], 192);
    memcpy(&out[288], &c.toAffineBytesLE(raw)[0], 96);
}

void Proof::toCompressedBytesBE(const span<uint8_t, 192> out) const
{
    memcpy(&out[  0], &a.toCompressedBytesBE()[0], 48);
    memcpy(&out[ 48], &b.toCompressedBytesBE()[0], 96);
    memcpy(&out[144], &c.toCompressedBytesBE()[0], 48);
}

array<uint8_t, 576> Proof::toJacobianBytesBE(const bool raw) const
{
    array<uint8_t, 576> out;
    toJacobianBytesBE(out, raw);
    return out;
}

array<uint8_t, 576> Proof::toJacobianBytesLE(const bool raw) const
{
    array<uint8_t, 576> out;
    toJacobianBytesLE(out, raw);
    return out;
}

array<uint8_t, 384> Proof::toAffineBytesBE(const bool raw) const
{
    array<uint8_t, 384> out;
    toAffineBytesBE(out, raw);
    return out;
}

array<uint8_t, 384> Proof::toAffineBytesLE(const bool raw) const
{
    array<uint8_t, 384> out;
    toAffineBytesLE(out, raw);
    return out;
}

array<uint8_t, 192> Proof::toCompressedBytesBE() const
{
    array<uint8_t, 192> out;
    toCompressedBytesBE(out);
    return out;
}

bool Proof::equal(const Proof& proof) const
{
    return a.equal(proof.a) && b.equal(proof.b) && c.equal(proof.c);
}

VerifyingKey::VerifyingKey() :
    alpha_g1(g1::zero()),
    beta_g1(g1::zero()),
    beta_g2(g2::zero()),
    gamma_g2(g2::zero()),
    delta_g1(g1::zero()),
    delta_g2(g2::zero()),
    ic(vector<g1>())
{
}

VerifyingKey::VerifyingKey(
    const g1& alpha_g1,
    const g1& beta_g1,
    const g2& beta_g2,
    const g2& gamma_g2,
    const g1& delta_g1,
    const g2& delta_g2,
    const vector<g1>& ic
) :
    alpha_g1(alpha_g1),
    beta_g1(beta_g1),
    beta_g2(beta_g2),
    gamma_g2(gamma_g2),
    delta_g1(delta_g1),
    delta_g2(delta_g2),
    ic(ic)
{
}

VerifyingKey::VerifyingKey(const VerifyingKey& vk) :
    alpha_g1(vk.alpha_g1),
    beta_g1(vk.beta_g1),
    beta_g2(vk.beta_g2),
    gamma_g2(vk.gamma_g2),
    delta_g1(vk.delta_g1),
    delta_g2(vk.delta_g2),
    ic(vk.ic)
{
}

VerifyingKey VerifyingKey::fromJacobianBytesBE(const uint8_t* in, const bool check, const bool raw)
{
    g1 alpha_g1 = g1::fromJacobianBytesBE(span<const uint8_t, 144>(&in[   0], &in[ 144]), check, raw);
    g1 beta_g1  = g1::fromJacobianBytesBE(span<const uint8_t, 144>(&in[ 144], &in[ 288]), check, raw);
    g2 beta_g2  = g2::fromJacobianBytesBE(span<const uint8_t, 288>(&in[ 288], &in[ 576]), check, raw);
    g2 gamma_g2 = g2::fromJacobianBytesBE(span<const uint8_t, 288>(&in[ 576], &in[ 864]), check, raw);
    g1 delta_g1 = g1::fromJacobianBytesBE(span<const uint8_t, 144>(&in[ 864], &in[1008]), check, raw);
    g2 delta_g2 = g2::fromJacobianBytesBE(span<const uint8_t, 288>(&in[1008], &in[1296]), check, raw);
    vector<g1> ic;
    uint32_t n_ic = in[1296] << 24 | in[1297] << 16 | in[1298] << 8 | in[1299];
    ic.reserve(n_ic);
    for(uint32_t i = 0; i < n_ic; i++)
    {
        ic.push_back(g1::fromJacobianBytesBE(span<const uint8_t, 144>(&in[1300 + i*144], &in[1300 + (i+1)*144]), check, raw));
    }
    return VerifyingKey(
        alpha_g1,
        beta_g1,
        beta_g2,
        gamma_g2,
        delta_g1,
        delta_g2,
        ic
    );
}

VerifyingKey VerifyingKey::fromJacobianBytesLE(const uint8_t* in, const bool check, const bool raw)
{
    g1 alpha_g1 = g1::fromJacobianBytesLE(span<const uint8_t, 144>(&in[   0], &in[ 144]), check, raw);
    g1 beta_g1  = g1::fromJacobianBytesLE(span<const uint8_t, 144>(&in[ 144], &in[ 288]), check, raw);
    g2 beta_g2  = g2::fromJacobianBytesLE(span<const uint8_t, 288>(&in[ 288], &in[ 576]), check, raw);
    g2 gamma_g2 = g2::fromJacobianBytesLE(span<const uint8_t, 288>(&in[ 576], &in[ 864]), check, raw);
    g1 delta_g1 = g1::fromJacobianBytesLE(span<const uint8_t, 144>(&in[ 864], &in[1008]), check, raw);
    g2 delta_g2 = g2::fromJacobianBytesLE(span<const uint8_t, 288>(&in[1008], &in[1296]), check, raw);
    vector<g1> ic;
    uint32_t n_ic = in[1296] | in[1297] << 8 | in[1298] << 16 | in[1299] << 24;
    ic.reserve(n_ic);
    for(uint32_t i = 0; i < n_ic; i++)
    {
        ic.push_back(g1::fromJacobianBytesLE(span<const uint8_t, 144>(&in[1300 + i*144], &in[1300 + (i+1)*144]), check, raw));
    }
    return VerifyingKey(
        alpha_g1,
        beta_g1,
        beta_g2,
        gamma_g2,
        delta_g1,
        delta_g2,
        ic
    );
}

VerifyingKey VerifyingKey::fromAffineBytesBE(const uint8_t* in, const bool check, const bool raw)
{
    g1 alpha_g1 = g1::fromAffineBytesBE(span<const uint8_t,  96>(&in[  0], &in[ 96]), check, raw);
    g1 beta_g1  = g1::fromAffineBytesBE(span<const uint8_t,  96>(&in[ 96], &in[192]), check, raw);
    g2 beta_g2  = g2::fromAffineBytesBE(span<const uint8_t, 192>(&in[192], &in[384]), check, raw);
    g2 gamma_g2 = g2::fromAffineBytesBE(span<const uint8_t, 192>(&in[384], &in[576]), check, raw);
    g1 delta_g1 = g1::fromAffineBytesBE(span<const uint8_t,  96>(&in[576], &in[672]), check, raw);
    g2 delta_g2 = g2::fromAffineBytesBE(span<const uint8_t, 192>(&in[672], &in[864]), check, raw);
    vector<g1> ic;
    uint32_t n_ic = in[864] << 24 | in[865] << 16 | in[866] << 8 | in[867];
    ic.reserve(n_ic);
    for(uint32_t i = 0; i < n_ic; i++)
    {
        ic.push_back(g1::fromAffineBytesBE(span<const uint8_t, 96>(&in[868 + i*96], &in[868 + (i+1)*96]), check, raw));
    }
    return VerifyingKey(
        alpha_g1,
        beta_g1,
        beta_g2,
        gamma_g2,
        delta_g1,
        delta_g2,
        ic
    );
}

VerifyingKey VerifyingKey::fromAffineBytesLE(const uint8_t* in, const bool check, const bool raw)
{
    g1 alpha_g1 = g1::fromAffineBytesLE(span<const uint8_t,  96>(&in[  0], &in[ 96]), check, raw);
    g1 beta_g1  = g1::fromAffineBytesLE(span<const uint8_t,  96>(&in[ 96], &in[192]), check, raw);
    g2 beta_g2  = g2::fromAffineBytesLE(span<const uint8_t, 192>(&in[192], &in[384]), check, raw);
    g2 gamma_g2 = g2::fromAffineBytesLE(span<const uint8_t, 192>(&in[384], &in[576]), check, raw);
    g1 delta_g1 = g1::fromAffineBytesLE(span<const uint8_t,  96>(&in[576], &in[672]), check, raw);
    g2 delta_g2 = g2::fromAffineBytesLE(span<const uint8_t, 192>(&in[672], &in[864]), check, raw);
    vector<g1> ic;
    uint32_t n_ic = in[864] | in[865] << 8 | in[866] << 16 | in[867] << 24;
    ic.reserve(n_ic);
    for(uint32_t i = 0; i < n_ic; i++)
    {
        ic.push_back(g1::fromAffineBytesLE(span<const uint8_t, 96>(&in[868 + i*96], &in[868 + (i+1)*96]), check, raw));
    }
    return VerifyingKey(
        alpha_g1,
        beta_g1,
        beta_g2,
        gamma_g2,
        delta_g1,
        delta_g2,
        ic
    );
}

VerifyingKey VerifyingKey::fromCompressedBytesBE(const uint8_t* in)
{
    g1 alpha_g1 = g1::fromCompressedBytesBE(span<const uint8_t, 48>(&in[  0], &in[ 48]));
    g1 beta_g1  = g1::fromCompressedBytesBE(span<const uint8_t, 48>(&in[ 48], &in[ 96]));
    g2 beta_g2  = g2::fromCompressedBytesBE(span<const uint8_t, 96>(&in[ 96], &in[192]));
    g2 gamma_g2 = g2::fromCompressedBytesBE(span<const uint8_t, 96>(&in[192], &in[288]));
    g1 delta_g1 = g1::fromCompressedBytesBE(span<const uint8_t, 48>(&in[288], &in[336]));
    g2 delta_g2 = g2::fromCompressedBytesBE(span<const uint8_t, 96>(&in[336], &in[432]));
    vector<g1> ic;
    uint32_t n_ic = in[432] << 24 | in[433] << 16 | in[434] << 8 | in[435];
    ic.reserve(n_ic);
    for(uint32_t i = 0; i < n_ic; i++)
    {
        ic.push_back(g1::fromCompressedBytesBE(span<const uint8_t, 48>(&in[436 + i*48], &in[436 + (i+1)*48])));
    }
    return VerifyingKey(
        alpha_g1,
        beta_g1,
        beta_g2,
        gamma_g2,
        delta_g1,
        delta_g2,
        ic
    );
}

void VerifyingKey::toJacobianBytesBE(uint8_t* out, const bool raw) const
{
    memcpy(&out[   0], &alpha_g1.toJacobianBytesBE(raw)[0], 144);
    memcpy(&out[ 144],  &beta_g1.toJacobianBytesBE(raw)[0], 144);
    memcpy(&out[ 288],  &beta_g2.toJacobianBytesBE(raw)[0], 288);
    memcpy(&out[ 576], &gamma_g2.toJacobianBytesBE(raw)[0], 288);
    memcpy(&out[ 864], &delta_g1.toJacobianBytesBE(raw)[0], 144);
    memcpy(&out[1008], &delta_g2.toJacobianBytesBE(raw)[0], 288);
    uint32_t n_ic = ic.size();
    out[1296] = n_ic >> 24;
    out[1297] = n_ic >> 16;
    out[1298] = n_ic >>  8;
    out[1299] = n_ic;
    for(uint32_t i = 0; i < n_ic; i++)
    {
        memcpy(&out[1300 + i*144], &ic[i].toJacobianBytesBE(raw)[0], 144);
    }
}

void VerifyingKey::toJacobianBytesLE(uint8_t* out, const bool raw) const
{
    memcpy(&out[   0], &alpha_g1.toJacobianBytesLE(raw)[0], 144);
    memcpy(&out[ 144],  &beta_g1.toJacobianBytesLE(raw)[0], 144);
    memcpy(&out[ 288],  &beta_g2.toJacobianBytesLE(raw)[0], 288);
    memcpy(&out[ 576], &gamma_g2.toJacobianBytesLE(raw)[0], 288);
    memcpy(&out[ 864], &delta_g1.toJacobianBytesLE(raw)[0], 144);
    memcpy(&out[1008], &delta_g2.toJacobianBytesLE(raw)[0], 288);
    uint32_t n_ic = ic.size();
    out[1296] = n_ic;
    out[1297] = n_ic >>  8;
    out[1298] = n_ic >> 16;
    out[1299] = n_ic >> 24;
    for(uint32_t i = 0; i < ic.size(); i++)
    {
        memcpy(&out[1300 + i*144], &ic[i].toJacobianBytesLE(raw)[0], 144);
    }
}

void VerifyingKey::toAffineBytesBE(uint8_t* out, const bool raw) const
{
    memcpy(&out[  0], &alpha_g1.toAffineBytesBE(raw)[0],  96);
    memcpy(&out[ 96],  &beta_g1.toAffineBytesBE(raw)[0],  96);
    memcpy(&out[192],  &beta_g2.toAffineBytesBE(raw)[0], 192);
    memcpy(&out[384], &gamma_g2.toAffineBytesBE(raw)[0], 192);
    memcpy(&out[576], &delta_g1.toAffineBytesBE(raw)[0],  96);
    memcpy(&out[672], &delta_g2.toAffineBytesBE(raw)[0], 192);
    uint32_t n_ic = ic.size();
    out[864] = n_ic >> 24;
    out[865] = n_ic >> 16;
    out[866] = n_ic >>  8;
    out[867] = n_ic;
    for(uint32_t i = 0; i < ic.size(); i++)
    {
        memcpy(&out[868 + i*96], &ic[i].toAffineBytesBE(raw)[0], 96);
    }
}

void VerifyingKey::toAffineBytesLE(uint8_t* out, const bool raw) const
{
    memcpy(&out[  0], &alpha_g1.toAffineBytesLE(raw)[0],  96);
    memcpy(&out[ 96],  &beta_g1.toAffineBytesLE(raw)[0],  96);
    memcpy(&out[192],  &beta_g2.toAffineBytesLE(raw)[0], 192);
    memcpy(&out[384], &gamma_g2.toAffineBytesLE(raw)[0], 192);
    memcpy(&out[576], &delta_g1.toAffineBytesLE(raw)[0],  96);
    memcpy(&out[672], &delta_g2.toAffineBytesLE(raw)[0], 192);
    uint32_t n_ic = ic.size();
    out[864] = n_ic;
    out[865] = n_ic >>  8;
    out[866] = n_ic >> 16;
    out[867] = n_ic >> 24;
    for(uint32_t i = 0; i < ic.size(); i++)
    {
        memcpy(&out[868 + i*96], &ic[i].toAffineBytesLE(raw)[0], 96);
    }
}

void VerifyingKey::toCompressedBytesBE(uint8_t* out) const
{
    memcpy(&out[  0], &alpha_g1.toCompressedBytesBE()[0], 48);
    memcpy(&out[ 48],  &beta_g1.toCompressedBytesBE()[0], 48);
    memcpy(&out[ 96],  &beta_g2.toCompressedBytesBE()[0], 96);
    memcpy(&out[192], &gamma_g2.toCompressedBytesBE()[0], 96);
    memcpy(&out[288], &delta_g1.toCompressedBytesBE()[0], 48);
    memcpy(&out[336], &delta_g2.toCompressedBytesBE()[0], 96);
    uint32_t n_ic = ic.size();
    out[432] = n_ic >> 24;
    out[433] = n_ic >> 16;
    out[434] = n_ic >>  8;
    out[435] = n_ic;
    for(uint32_t i = 0; i < ic.size(); i++)
    {
        memcpy(&out[436 + i*48], &ic[i].toCompressedBytesBE()[0], 48);
    }
}

vector<uint8_t> VerifyingKey::toJacobianBytesBE(const bool raw) const
{
    vector<uint8_t> out;
    out.resize(1296 + 4 + ic.size()*144);
    toJacobianBytesBE(out.data(), raw);
    return out;
}

vector<uint8_t> VerifyingKey::toJacobianBytesLE(const bool raw) const
{
    vector<uint8_t> out;
    out.resize(1296 + 4 + ic.size()*144);
    toJacobianBytesLE(out.data(), raw);
    return out;
}

vector<uint8_t> VerifyingKey::toAffineBytesBE(const bool raw) const
{
    vector<uint8_t> out;
    out.resize(860 + 4 + ic.size()*96);
    toAffineBytesBE(out.data(), raw);
    return out;
}

vector<uint8_t> VerifyingKey::toAffineBytesLE(const bool raw) const
{
    vector<uint8_t> out;
    out.resize(860 + 4 + ic.size()*96);
    toAffineBytesLE(out.data(), raw);
    return out;
}

vector<uint8_t> VerifyingKey::toCompressedBytesBE() const
{
    vector<uint8_t> out;
    out.resize(432 + 4 + ic.size()*48);
    toCompressedBytesBE(out.data());
    return out;
}

bool VerifyingKey::equal(const VerifyingKey& vk) const
{
    if(ic.size() != vk.ic.size()) return false;
    for(uint32_t i = 0; i < ic.size(); i++)
    {
        if(!ic[i].equal(vk.ic[i])) return false;
    }
    return 
        alpha_g1.equal(vk.alpha_g1) &&
        beta_g1.equal(vk.beta_g1) &&
        beta_g2.equal(vk.beta_g2) &&
        gamma_g2.equal(vk.gamma_g2) &&
        delta_g1.equal(vk.delta_g1) &&
        delta_g2.equal(vk.delta_g2);
}

PreparedVerifyingKey::PreparedVerifyingKey() :
    alpha_g1_beta_g2(fp12::zero()),
    neg_gamma_g2(g2::zero()),
    neg_delta_g2(g2::zero()),
    ic(vector<g1>())
{
}

PreparedVerifyingKey::PreparedVerifyingKey(
    const fp12& alpha_g1_beta_g2,
    const g2& neg_gamma_g2,
    const g2& neg_delta_g2,
    const std::vector<g1>& ic
) :
    alpha_g1_beta_g2(alpha_g1_beta_g2),
    neg_gamma_g2(neg_gamma_g2),
    neg_delta_g2(neg_delta_g2),
    ic(ic)
{
}

PreparedVerifyingKey::PreparedVerifyingKey(const PreparedVerifyingKey& pvk) :
    alpha_g1_beta_g2(pvk.alpha_g1_beta_g2),
    neg_gamma_g2(pvk.neg_gamma_g2),
    neg_delta_g2(pvk.neg_delta_g2),
    ic(pvk.ic)
{
}

PreparedVerifyingKey PreparedVerifyingKey::fromBytesBE(const uint8_t* in, const bool check, const bool raw)
{
    fp12 alpha_g1_beta_g2 = fp12::fromBytesBE(span<const uint8_t, 576>(&in[   0], &in[ 576]), check, raw);
    g2 neg_gamma_g2 = g2::fromJacobianBytesBE(span<const uint8_t, 288>(&in[ 576], &in[ 864]), check, raw);
    g2 neg_delta_g2 = g2::fromJacobianBytesBE(span<const uint8_t, 288>(&in[ 864], &in[1152]), check, raw);
    vector<g1> ic;
    uint32_t n_ic = in[1152] << 24 | in[1153] << 16 | in[1154] << 8 | in[1155];
    ic.reserve(n_ic);
    for(uint32_t i = 0; i < n_ic; i++)
    {
        ic.push_back(g1::fromJacobianBytesBE(span<const uint8_t, 144>(&in[1156 + i*144], &in[1156 + (i+1)*144]), check, raw));
    }
    return PreparedVerifyingKey(
        alpha_g1_beta_g2,
        neg_gamma_g2,
        neg_delta_g2,
        ic
    );
}

PreparedVerifyingKey PreparedVerifyingKey::fromBytesLE(const uint8_t* in, const bool check, const bool raw)
{
    fp12 alpha_g1_beta_g2 = fp12::fromBytesLE(span<const uint8_t, 576>(&in[   0], &in[ 576]), check, raw);
    g2 neg_gamma_g2 = g2::fromJacobianBytesLE(span<const uint8_t, 288>(&in[ 576], &in[ 864]), check, raw);
    g2 neg_delta_g2 = g2::fromJacobianBytesLE(span<const uint8_t, 288>(&in[ 864], &in[1152]), check, raw);
    vector<g1> ic;
    uint32_t n_ic = in[1152] | in[1153] << 8 | in[1154] << 16 | in[1155] << 24;
    ic.reserve(n_ic);
    for(uint32_t i = 0; i < n_ic; i++)
    {
        ic.push_back(g1::fromJacobianBytesLE(span<const uint8_t, 144>(&in[1156 + i*144], &in[1156 + (i+1)*144]), check, raw));
    }
    return PreparedVerifyingKey(
        alpha_g1_beta_g2,
        neg_gamma_g2,
        neg_delta_g2,
        ic
    );
}

void PreparedVerifyingKey::toBytesBE(uint8_t* out, const bool raw) const
{
    memcpy(&out[   0],     &alpha_g1_beta_g2.toBytesBE(raw)[0], 576);
    memcpy(&out[ 576], &neg_gamma_g2.toJacobianBytesBE(raw)[0], 288);
    memcpy(&out[ 864], &neg_delta_g2.toJacobianBytesBE(raw)[0], 288);
    uint32_t n_ic = ic.size();
    out[1152] = n_ic >> 24;
    out[1153] = n_ic >> 16;
    out[1154] = n_ic >>  8;
    out[1155] = n_ic;
    for(uint32_t i = 0; i < n_ic; i++)
    {
        memcpy(&out[1156 + i*144], &ic[i].toJacobianBytesBE(raw)[0], 144);
    }
}

void PreparedVerifyingKey::toBytesLE(uint8_t* out, const bool raw) const
{
    memcpy(&out[   0],     &alpha_g1_beta_g2.toBytesLE(raw)[0], 576);
    memcpy(&out[ 576], &neg_gamma_g2.toJacobianBytesLE(raw)[0], 288);
    memcpy(&out[ 864], &neg_delta_g2.toJacobianBytesLE(raw)[0], 288);
    uint32_t n_ic = ic.size();
    out[1152] = n_ic;
    out[1153] = n_ic >>  8;
    out[1154] = n_ic >> 16;
    out[1155] = n_ic >> 24;
    for(uint32_t i = 0; i < n_ic; i++)
    {
        memcpy(&out[1156 + i*144], &ic[i].toJacobianBytesLE(raw)[0], 144);
    }
}

std::vector<uint8_t> PreparedVerifyingKey::toBytesBE(const bool raw) const
{
    vector<uint8_t> out;
    out.resize(1152 + 4 + ic.size()*144);
    toBytesBE(out.data(), raw);
    return out;
}

std::vector<uint8_t> PreparedVerifyingKey::toBytesLE(const bool raw) const
{
    vector<uint8_t> out;
    out.resize(1152 + 4 + ic.size()*144);
    toBytesLE(out.data(), raw);
    return out;
}

bool PreparedVerifyingKey::equal(const PreparedVerifyingKey& pvk) const
{
    if(ic.size() != pvk.ic.size()) return false;
    for(uint32_t i = 0; i < ic.size(); i++)
    {
        if(!ic[i].equal(pvk.ic[i])) return false;
    }
    return 
        alpha_g1_beta_g2.equal(pvk.alpha_g1_beta_g2) &&
        neg_gamma_g2.equal(pvk.neg_gamma_g2) &&
        neg_delta_g2.equal(pvk.neg_delta_g2);
}

PreparedVerifyingKey prepare_verifying_key(const VerifyingKey& vk)
{
    vector<tuple<g1, g2>> v;
    pairing::add_pair(v, vk.alpha_g1, vk.beta_g2);
    return PreparedVerifyingKey(
        pairing::calculate(v),
        vk.gamma_g2.neg(),
        vk.delta_g2.neg(),
        vk.ic
    );
}

bool verify_proof(
    const PreparedVerifyingKey& pvk,
    const Proof& proof,
    vector<array<uint64_t, 4>>& public_inputs
)
{
    if(public_inputs.size() + 1 != pvk.ic.size())
    {
        return false;
    }

    g1 acc = pvk.ic[0];
    for(uint32_t i = 1; i < pvk.ic.size(); i++)
    {
        acc = acc.add(pvk.ic[i].mulScalar(public_inputs[i-1]));
    }
    
    // The original verification equation is:
    // A * B = alpha * beta + inputs * gamma + C * delta
    // ... however, we rearrange it so that it is:
    // A * B - inputs * gamma - C * delta = alpha * beta
    // or equivalently:
    // A * B + inputs * (-gamma) + C * (-delta) = alpha * beta
    // which allows us to do a single final exponentiation.// The original verification equation is:
    // A * B = alpha * beta + inputs * gamma + C * delta
    // ... however, we rearrange it so that it is:
    // A * B - inputs * gamma - C * delta = alpha * beta
    // or equivalently:
    // A * B + inputs * (-gamma) + C * (-delta) = alpha * beta
    // which allows us to do a single final exponentiation.

    vector<tuple<g1, g2>> v;
    pairing::add_pair(v, proof.a,   proof.b);
    pairing::add_pair(v, acc,       pvk.neg_gamma_g2);
    pairing::add_pair(v, proof.c,   pvk.neg_delta_g2);
    return pvk.alpha_g1_beta_g2.equal(pairing::calculate(v));
}

} // namespace groth16
} // namespace bls12_381