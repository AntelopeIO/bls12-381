#include <cstdint>
#include <tuple>

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

namespace bls12_381
{

class fp;

void init(bool cpu_features = true);

void _add(fp* z, const fp* x, const fp* y);
void _double(fp* z, const fp* x);
void _subtract(fp* z, const fp* x, const fp* y);
void _negate(fp* z, const fp* x);
void _square(fp* z, const fp* x);

// Those are low level calls that will not perform modular reduction after the operation 
// Please use with caution. Overflow will happen.
void _ladd(fp* z, const fp* x, const fp* y);
void _ldouble(fp* z, const fp* x);
void _lsubtract(fp* z, const fp* x, const fp* y);

// Multiplication will work fine as long as both operands are smaller than around 4p. 
// The "smaller than 4p" here means the montgomery form itself as number is less than 4p.
// Therefore, at most ONE _ladd/_lsubstract/_ldouble is allowed before passing the result to _multiply,
// unless the algorithm makes sure the number is small.
#if defined(__x86_64__) && defined(__ELF__)
extern void _multiply(fp*, const fp*, const fp*);
#elif defined(__x86_64__)
extern void (*_multiply)(fp*, const fp*, const fp*);
#else
void _multiply(fp*, const fp*, const fp*);
#endif

// Add64 returns the sum with carry of x, y and carry: sum = x + y + carry.
// The carry input must be 0 or 1; otherwise the behavior is undefined.
// The carryOut output is guaranteed to be 0 or 1.
//
// This function's execution time does not depend on the inputs.
std::tuple<uint64_t, uint64_t> Add64(
    const uint64_t& x,
    const uint64_t& y,
    const uint64_t& carry
);

// Sub64 returns the difference of x, y and borrow: diff = x - y - borrow.
// The borrow input must be 0 or 1; otherwise the behavior is undefined.
// The borrowOut output is guaranteed to be 0 or 1.
//
// This function's execution time does not depend on the inputs.
std::tuple<uint64_t, uint64_t> Sub64(
    const uint64_t& x,
    const uint64_t& y,
    const uint64_t& borrow
);

// Mul64 returns the 128-bit product of x and y: (hi, lo) = x * y
// with the product bits' upper half returned in hi and the lower
// half returned in lo.
//
// This function's execution time does not depend on the inputs.
std::tuple<uint64_t, uint64_t> Mul64(
    const uint64_t& x,
    const uint64_t& y
);

std::tuple<uint64_t, uint64_t, uint64_t> madd(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& t,
    const uint64_t& u,
    const uint64_t& v
);

// madd0 hi = a*b + c (discards lo bits)
uint64_t madd0(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c
);

// madd1 hi, lo = a*b + c
std::tuple<uint64_t, uint64_t> madd1(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c
);

// madd2 hi, lo = a*b + c + d
std::tuple<uint64_t, uint64_t> madd2(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c,
    const uint64_t& d
);

// madd2s superhi, hi, lo = 2*a*b + c + d + e
std::tuple<uint64_t, uint64_t, uint64_t> madd2s(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c,
    const uint64_t& d,
    const uint64_t& e
);

std::tuple<uint64_t, uint64_t, uint64_t> madd1s(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& d,
    const uint64_t& e
);

std::tuple<uint64_t, uint64_t, uint64_t> madd2sb(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c,
    const uint64_t& e
);

std::tuple<uint64_t, uint64_t, uint64_t> madd1sb(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& e
);

std::tuple<uint64_t, uint64_t> madd3(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c,
    const uint64_t& d,
    const uint64_t& e
);

} // namespace bls12_381
