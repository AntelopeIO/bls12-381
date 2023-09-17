#include <bls12-381/bls12-381.hpp>

using namespace std;

namespace bls12_381
{

vector<uint8_t> hexToBytes(std::string_view s)
{
    uint64_t start_idx = 0;
    if(s[0] == '0' && s[1] == 'x')
    {
        start_idx = 2;
    }

    if(s.length() % 2 != 0)
    {
        // string length invalid!
        return {};
    }

    vector<uint8_t> bytes;
    uint64_t num_bytes = (s.length() - start_idx) / 2;
    bytes.reserve(num_bytes);
    for(size_t i = 0, j = start_idx; i < num_bytes; i++, j += 2)
    {
        bytes.push_back((s[j] % 32 + 9) % 25 * 16 + (s[j+1] % 32 + 9) % 25);
    }

    return bytes;
}

string bytesToHex(std::span<const uint8_t> in)
{
    constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    string s(2 + in.size() * 2, ' ');
    s[0] = '0';
    s[1] = 'x';
    for(uint64_t i = 0; i < in.size(); ++i)
    {
        s[2 + 2*i]     = hexmap[(in[i] & 0xF0) >> 4];
        s[2 + 2*i+1]   = hexmap[ in[i] & 0x0F      ];
    }
    return s;
}


// HELPER FUNCTIONS
// for p mod q calculations
#define RLC_MASK(B) ((-(uint64_t)((B) >= 64)) | (((uint64_t)1 << ((B) % 64)) - 1))
uint64_t bn_lshb_low(uint64_t *c, const uint64_t *a, int size, int bits)
{
    int i;
    uint64_t r, carry, shift, mask;

    shift = 64 - bits;
    carry = 0;
    mask = RLC_MASK(bits);
    for(i = 0; i < size; i++, a++, c++)
    {
        // Get the needed least significant bits.
        r = ((*a) >> shift) & mask;
        // Shift left the operand.
        *c = ((*a) << bits) | carry;
        // Update the carry.
        carry = r;
    }
    return carry;
}

void dv_lshd(uint64_t *c, const uint64_t *a, int size, int digits)
{
    uint64_t *top;
    const uint64_t *bot;
    int i;

    top = c + size - 1;
    bot = a + size - 1 - digits;

    for(i = 0; i < size - digits; i++, top--, bot--)
    {
        *top = *bot;
    }
    for(i = 0; i < digits; i++, c++)
    {
        *c = 0;
    }
}

int dv_cmp(const uint64_t *a, const uint64_t *b, int size)
{
    int i, r;

    a += (size - 1);
    b += (size - 1);

    r = 0;
    for(i = 0; i < size; i++, --a, --b)
    {
        if(*a != *b && r == 0)
        {
            r = (*a > *b ? 1 : -1);
        }
    }
    return r;
}

uint64_t bn_subn_low(uint64_t *c, const uint64_t *a, const uint64_t *b, int size)
{
    int i;
    uint64_t carry, r0, diff;

    // Zero the carry.
    carry = 0;
    for(i = 0; i < size; i++, a++, b++, c++)
    {
        diff = (*a) - (*b);
        r0 = diff - carry;
        carry = ((*a) < (*b)) || (carry && !diff);
        (*c) = r0;
    }
    return carry;
}

void dv_rshd(uint64_t *c, const uint64_t *a, int size, int digits)
{
    const uint64_t *top;
    uint64_t *bot;
    int i;

    top = a + digits;
    bot = c;

    for(i = 0; i < size - digits; i++, top++, bot++)
    {
        *bot = *top;
    }
    for(; i < size; i++, bot++)
    {
        *bot = 0;
    }
}

uint64_t bn_mul1_low(uint64_t *c, const uint64_t *a, uint64_t digit, int size)
{
    uint64_t r0, r1, carry = 0;
    for(int i = 0; i < size; i++, a++, c++)
    {
        r1 = (static_cast<__uint128_t>(*a) * static_cast<__uint128_t>(digit)) >> (64);
        r0 = (*a) * (digit);
        *c = r0 + carry;
        carry = r1 + (*c < carry);
    }
    return carry;
}

uint64_t bn_sub1_low(uint64_t *c, const uint64_t *a, uint64_t digit, int size)
{
    int i;
    uint64_t carry, r0;

    carry = digit;
    for(i = 0; i < size && carry; i++, c++, a++)
    {
        r0 = (*a) - carry;
        carry = (r0 > (*a));
        (*c) = r0;
    }
    for(; i < size; i++, a++, c++)
    {
        (*c) = (*a);
    }
    return carry;
}

uint64_t bn_addn_low(uint64_t *c, const uint64_t *a, const uint64_t *b, int size)
{
    int i;
    uint64_t carry, c0, c1, r0, r1;

    carry = 0;
    for(i = 0; i < size; i++, a++, b++, c++)
    {
        r0 = (*a) + (*b);
        c0 = (r0 < (*a));
        r1 = r0 + carry;
        c1 = (r1 < r0);
        carry = c0 | c1;
        (*c) = r1;
    }
    return carry;
}

uint64_t bn_add1_low(uint64_t *c, const uint64_t *a, uint64_t digit, int size)
{
    int i;
    uint64_t carry, r0;

    carry = digit;
    for(i = 0; i < size && carry; i++, a++, c++)
    {
        r0 = (*a) + carry;
        carry = (r0 < carry);
        (*c) = r0;
    }
    for(; i < size; i++, a++, c++)
    {
        (*c) = (*a);
    }
    return carry;
}

uint64_t bn_rshb_low(uint64_t *c, const uint64_t *a, int size, int bits)
{
    int i;
    uint64_t r, carry, shift, mask;

    c += size - 1;
    a += size - 1;
    // Prepare the bit mask.
    shift = (64 - bits) % 64;
    carry = 0;
    mask = RLC_MASK(bits);
    for(i = size - 1; i >= 0; i--, a--, c--)
    {
        // Get the needed least significant bits.
        r = (*a) & mask;
        // Shift left the operand.
        *c = ((*a) >> bits) | (carry << shift);
        // Update the carry.
        carry = r;
    }
    return carry;
}

void bn_divn_low(uint64_t *c, uint64_t *d, uint64_t *a, int sa, uint64_t *b, int sb)
{
    int norm, i, n, t, sd;
    uint64_t carry, t1[3], t2[3];

    // Normalize x and y so that the leading digit of y is bigger than 2^(RLC_DIG-1).
    // std::countl_zero will return 64 if uint64_t(0) is passed in. 
    // The %64 operation will handle this case.
    norm = (64 - std::countl_zero(b[sb - 1])) % 64;

    if(norm < 64 - 1)
    {
        norm = (64 - 1) - norm;
        carry = bn_lshb_low(a, a, sa, norm);
        if(carry)
        {
            a[sa++] = carry;
        }
        carry = bn_lshb_low(b, b, sb, norm);
        if(carry)
        {
            b[sb++] = carry;
        }
    }
    else
    {
        norm = 0;
    }

    n = sa - 1;
    t = sb - 1;

    // Shift y so that the most significant digit of y is aligned with the most significant digit of x.
    dv_lshd(b, b, sb + (n - t), (n - t));

    // Find the most significant digit of the quotient.
    while(dv_cmp(a, b, sa) != -1)
    {
        c[n - t]++;
        bn_subn_low(a, a, b, sa);
    }

    // Shift y back.
    dv_rshd(b, b, sb + (n - t), (n - t));

    // Find the remaining digits.
    for(i = n; i >= (t + 1); i--)
    {

        if(i > sa)
        {
            continue;
        }

        if(a[i] == b[t])
        {
            c[i - t - 1] = RLC_MASK(64);
        }
        else
        {
            c[i - t - 1] = (((__uint128_t)(a[i]) << (64)) | (a[i - 1])) / (b[t]);
        }

        c[i - t - 1]++;
        do
        {
            c[i - t - 1]--;
            t1[0] = (t - 1 < 0) ? 0 : b[t - 1];
            t1[1] = b[t];

            carry = bn_mul1_low(t1, t1, c[i - t - 1], 2);
            t1[2] = carry;

            t2[0] = (i - 2 < 0) ? 0 : a[i - 2];
            t2[1] = (i - 1 < 0) ? 0 : a[i - 1];
            t2[2] = a[i];
        }
        while(dv_cmp(t1, t2, 3) == 1);

        carry = bn_mul1_low(d, b, c[i - t - 1], sb);
        sd = sb;
        if(carry)
        {
            d[sd++] = carry;
        }

        carry = bn_subn_low(a + (i - t - 1), a + (i - t - 1), d, sd);
        sd += (i - t - 1);
        if(sa - sd > 0)
        {
            carry = bn_sub1_low(a + sd, a + sd, carry, sa - sd);
        }

        if(carry)
        {
            sd = sb + (i - t - 1);
            carry = bn_addn_low(a + (i - t - 1), a + (i - t - 1), b, sb);
            carry = bn_add1_low(a + sd, a + sd, carry, sa - sd);
            c[i - t - 1]--;
        }
    }
    // Remainder should be not be longer than the divisor.
    bn_rshb_low(d, a, sb, norm);
}

} // namespace bls12_381
