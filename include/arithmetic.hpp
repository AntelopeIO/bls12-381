#include <cstdint>
#include <tuple>
#include <functional>

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

namespace bls12_381
{

class fp;

extern std::function<void(fp*, const fp*, const fp*)> _add;
extern std::function<void(fp*, const fp*)> _addAssign;
extern std::function<void(fp*, const fp*, const fp*)> _ladd;
extern std::function<void(fp*, const fp*)> _laddAssign;
extern std::function<void(fp*, const fp*)> _double;
extern std::function<void(fp*)> _doubleAssign;
extern std::function<void(fp*, const fp*)> _ldouble;
extern std::function<void(fp*, const fp*, const fp*)> _sub;
extern std::function<void(fp*, const fp*)> _subAssign;
extern std::function<void(fp*, const fp*)> _lsubAssign;
extern std::function<void(fp*, const fp*)> _neg;
extern std::function<void(fp*, const fp*, const fp*)> _mul;
extern std::function<void(fp*, const fp*)> _square;

void init();

void _add_cpp(fp* z, const fp* x, const fp* y);
void _addAssign_cpp(fp* x, const fp* y);
void _ladd_cpp(fp* z, const fp* x, const fp* y);
void _laddAssign_cpp(fp* x, const fp* y);
void _double_cpp(fp* z, const fp* x);
void _doubleAssign_cpp(fp* z);
void _ldouble_cpp(fp* z, const fp* x);
void _sub_cpp(fp* z, const fp* x, const fp* y);
void _subAssign_cpp(fp* z, const fp* x);
void _lsubAssign_cpp(fp* z, const fp* x);
void _neg_cpp(fp* z, const fp* x);
void _mul_cpp(fp* z, const fp* x, const fp* y);
void _square_cpp(fp* z, const fp* x);

#ifdef __x86_64__
void _add_x86_64(fp* z, const fp* x, const fp* y);
void _addAssign_x86_64(fp* x, const fp* y);
void _ladd_x86_64(fp* z, const fp* x, const fp* y);
void _laddAssign_x86_64(fp* x, const fp* y);
void _double_x86_64(fp* z, const fp* x);
void _doubleAssign_x86_64(fp* z);
void _ldouble_x86_64(fp* z, const fp* x);
void _sub_x86_64(fp* z, const fp* x, const fp* y);
void _subAssign_x86_64(fp* z, const fp* x);
void _lsubAssign_x86_64(fp* z, const fp* x);
void _neg_x86_64(fp* z, const fp* x);
void _mul_x86_64(fp* z, const fp* x, const fp* y);
void _square_x86_64(fp* z, const fp* x);
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