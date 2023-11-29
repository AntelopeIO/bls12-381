#include <bls12-381/bls12-381.hpp>
#ifdef __x86_64__
#include <cpuid.h>
#endif

using namespace std;

namespace bls12_381
{

#ifdef __x86_64__
void _add(fp* z, const fp* x, const fp* y)
{
    // x86_64 calling convention (https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI):
    // z => %rdi
    // x => %rsi
    // y => %rdx
    // callee needs to restore registers r15, r14, r13, r12, rbx before returning
    asm("push %r15;");
    asm("push %r14;");
    asm("push %r13;");
    asm("push %r12;");
    asm("push %rbx;");
    asm("mov     (%rsi),%r8;");
    asm("mov 0x08(%rsi),%r9;");
    asm("mov 0x10(%rsi),%r10;");
    asm("mov 0x18(%rsi),%r11;");
    asm("mov 0x20(%rsi),%r12;");
    asm("mov 0x28(%rsi),%r13;");
    asm("add     (%rdx),%r8;");
    asm("adc 0x08(%rdx),%r9;");
    asm("adc 0x10(%rdx),%r10;");
    asm("adc 0x18(%rdx),%r11;");
    asm("adc 0x20(%rdx),%r12;");
    asm("adc 0x28(%rdx),%r13;");
    asm("mov %r8,%r14;");
    asm("mov %r9,%r15;");
    asm("mov %r10,%rcx;");
    asm("mov %r11,%rdx;");
    asm("mov %r12,%rsi;");
    asm("mov %r13,%rbx;");
    asm("mov $0xb9feffffffffaaab,%rax;");
    asm("sub %rax,%r14;");
    asm("mov $0x1eabfffeb153ffff,%rax;");
    asm("sbb %rax,%r15;");
    asm("mov $0x6730d2a0f6b0f624,%rax;");
    asm("sbb %rax,%rcx;");
    asm("mov $0x64774b84f38512bf,%rax;");
    asm("sbb %rax,%rdx;");
    asm("mov $0x4b1ba7b6434bacd7,%rax;");
    asm("sbb %rax,%rsi;");
    asm("mov $0x1a0111ea397fe69a,%rax;");
    asm("sbb %rax,%rbx;");
    asm("cmovae %r14,%r8;");
    asm("cmovae %r15,%r9;");
    asm("cmovae %rcx,%r10;");
    asm("cmovae %rdx,%r11;");
    asm("cmovae %rsi,%r12;");
    asm("cmovae %rbx,%r13;");
    asm("mov %r8,     (%rdi);");
    asm("mov %r9, 0x08(%rdi);");
    asm("mov %r10,0x10(%rdi);");
    asm("mov %r11,0x18(%rdi);");
    asm("mov %r12,0x20(%rdi);");
    asm("mov %r13,0x28(%rdi);");
    asm("pop %rbx;");
    asm("pop %r12;");
    asm("pop %r13;");
    asm("pop %r14;");
    asm("pop %r15;");
}
#else
void _add(fp* z, const fp* x, const fp* y)
{
    uint64_t carry, _;

    tie(z->d[0], carry) = Add64(x->d[0], y->d[0], 0);
    tie(z->d[1], carry) = Add64(x->d[1], y->d[1], carry);
    tie(z->d[2], carry) = Add64(x->d[2], y->d[2], carry);
    tie(z->d[3], carry) = Add64(x->d[3], y->d[3], carry);
    tie(z->d[4], carry) = Add64(x->d[4], y->d[4], carry);
    tie(z->d[5], _)     = Add64(x->d[5], y->d[5], carry);

    // if z > q --> z -= q
    // note: this is NOT constant time
    if(!(z->d[5] < fp::MODULUS.d[5] || (z->d[5] == fp::MODULUS.d[5] && (z->d[4] < fp::MODULUS.d[4] || (z->d[4] == fp::MODULUS.d[4] && (z->d[3] < fp::MODULUS.d[3] || (z->d[3] == fp::MODULUS.d[3] && (z->d[2] < fp::MODULUS.d[2] || (z->d[2] == fp::MODULUS.d[2] && (z->d[1] < fp::MODULUS.d[1] || (z->d[1] == fp::MODULUS.d[1] && (z->d[0] < fp::MODULUS.d[0]))))))))))))
    {
        uint64_t b;
        tie(z->d[0], b) = Sub64(z->d[0], fp::MODULUS.d[0], 0);
        tie(z->d[1], b) = Sub64(z->d[1], fp::MODULUS.d[1], b);
        tie(z->d[2], b) = Sub64(z->d[2], fp::MODULUS.d[2], b);
        tie(z->d[3], b) = Sub64(z->d[3], fp::MODULUS.d[3], b);
        tie(z->d[4], b) = Sub64(z->d[4], fp::MODULUS.d[4], b);
        tie(z->d[5], _) = Sub64(z->d[5], fp::MODULUS.d[5], b);
    }
}
#endif

#ifdef __x86_64__
void _ladd(fp* z, const fp* x, const fp* y)
{
    // x86_64 calling convention (https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI):
    // z => %rdi
    // x => %rsi
    // y => %rdx
    // callee needs to restore registers r15, r14, r13, r12, rbx (if clobbed) before returning
    asm("mov     (%rsi),%r8;");
    asm("mov 0x08(%rsi),%r9;");
    asm("mov 0x10(%rsi),%r10;");
    asm("mov 0x18(%rsi),%r11;");
    asm("mov 0x20(%rsi),%rcx;");
    asm("mov 0x28(%rsi),%rax;");
    asm("add     (%rdx),%r8;");
    asm("adc 0x08(%rdx),%r9;");
    asm("adc 0x10(%rdx),%r10;");
    asm("adc 0x18(%rdx),%r11;");
    asm("adc 0x20(%rdx),%rcx;");
    asm("adc 0x28(%rdx),%rax;");
    asm("mov %r8,     (%rdi);");
    asm("mov %r9, 0x08(%rdi);");
    asm("mov %r10,0x10(%rdi);");
    asm("mov %r11,0x18(%rdi);");
    asm("mov %rcx,0x20(%rdi);");
    asm("mov %rax,0x28(%rdi);");
}
#else
void _ladd(fp* z, const fp* x, const fp* y)
{
    uint64_t carry, _;
    tie(z->d[0], carry) = Add64(x->d[0], y->d[0], 0);
    tie(z->d[1], carry) = Add64(x->d[1], y->d[1], carry);
    tie(z->d[2], carry) = Add64(x->d[2], y->d[2], carry);
    tie(z->d[3], carry) = Add64(x->d[3], y->d[3], carry);
    tie(z->d[4], carry) = Add64(x->d[4], y->d[4], carry);
    tie(z->d[5], _)     = Add64(x->d[5], y->d[5], carry);
}
#endif

#ifdef __x86_64__
void _double(fp* z, const fp* x)
{
    // x86_64 calling convention (https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI):
    // z => %rdi
    // x => %rsi
    // callee needs to restore registers r15, r14, r13, r12, rbx before returning
    asm("push %r15;");
    asm("push %r14;");
    asm("push %r13;");
    asm("push %r12;");
    asm("push %rbx;");
    asm("mov     (%rsi),%r8;");
    asm("mov 0x08(%rsi),%r9;");
    asm("mov 0x10(%rsi),%r10;");
    asm("mov 0x18(%rsi),%r11;");
    asm("mov 0x20(%rsi),%r12;");
    asm("mov 0x28(%rsi),%r13;");
    asm("add %r8,%r8;");
    asm("adc %r9,%r9;");
    asm("adc %r10,%r10;");
    asm("adc %r11,%r11;");
    asm("adc %r12,%r12;");
    asm("adc %r13,%r13;");
    asm("mov %r8,%r14;");
    asm("mov %r9,%r15;");
    asm("mov %r10,%rcx;");
    asm("mov %r11,%rdx;");
    asm("mov %r12,%rsi;");
    asm("mov %r13,%rbx;");
    asm("mov $0xb9feffffffffaaab,%rax;");
    asm("sub %rax,%r14;");
    asm("mov $0x1eabfffeb153ffff,%rax;");
    asm("sbb %rax,%r15;");
    asm("mov $0x6730d2a0f6b0f624,%rax;");
    asm("sbb %rax,%rcx;");
    asm("mov $0x64774b84f38512bf,%rax;");
    asm("sbb %rax,%rdx;");
    asm("mov $0x4b1ba7b6434bacd7,%rax;");
    asm("sbb %rax,%rsi;");
    asm("mov $0x1a0111ea397fe69a,%rax;");
    asm("sbb %rax,%rbx;");
    asm("cmovae %r14,%r8;");
    asm("cmovae %r15,%r9;");
    asm("cmovae %rcx,%r10;");
    asm("cmovae %rdx,%r11;");
    asm("cmovae %rsi,%r12;");
    asm("cmovae %rbx,%r13;");
    asm("mov %r8,     (%rdi);");
    asm("mov %r9, 0x08(%rdi);");
    asm("mov %r10,0x10(%rdi);");
    asm("mov %r11,0x18(%rdi);");
    asm("mov %r12,0x20(%rdi);");
    asm("mov %r13,0x28(%rdi);");
    asm("pop %rbx;");
    asm("pop %r12;");
    asm("pop %r13;");
    asm("pop %r14;");
    asm("pop %r15;");
}
#else
void _double(fp* z, const fp* x)
{
    uint64_t carry, _;

    tie(z->d[0], carry) = Add64(x->d[0], x->d[0], 0);
    tie(z->d[1], carry) = Add64(x->d[1], x->d[1], carry);
    tie(z->d[2], carry) = Add64(x->d[2], x->d[2], carry);
    tie(z->d[3], carry) = Add64(x->d[3], x->d[3], carry);
    tie(z->d[4], carry) = Add64(x->d[4], x->d[4], carry);
    tie(z->d[5], _)     = Add64(x->d[5], x->d[5], carry);

    // if z > q --> z -= q
    // note: this is NOT constant time
    if(!(z->d[5] < fp::MODULUS.d[5] || (z->d[5] == fp::MODULUS.d[5] && (z->d[4] < fp::MODULUS.d[4] || (z->d[4] == fp::MODULUS.d[4] && (z->d[3] < fp::MODULUS.d[3] || (z->d[3] == fp::MODULUS.d[3] && (z->d[2] < fp::MODULUS.d[2] || (z->d[2] == fp::MODULUS.d[2] && (z->d[1] < fp::MODULUS.d[1] || (z->d[1] == fp::MODULUS.d[1] && (z->d[0] < fp::MODULUS.d[0]))))))))))))
    {
        uint64_t b;
        tie(z->d[0], b) = Sub64(z->d[0], fp::MODULUS.d[0], 0);
        tie(z->d[1], b) = Sub64(z->d[1], fp::MODULUS.d[1], b);
        tie(z->d[2], b) = Sub64(z->d[2], fp::MODULUS.d[2], b);
        tie(z->d[3], b) = Sub64(z->d[3], fp::MODULUS.d[3], b);
        tie(z->d[4], b) = Sub64(z->d[4], fp::MODULUS.d[4], b);
        tie(z->d[5], _) = Sub64(z->d[5], fp::MODULUS.d[5], b);
    }
}
#endif

#ifdef __x86_64__
void _ldouble(fp* z, const fp* x)
{
    // x86_64 calling convention (https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI):
    // z => %rdi
    // x => %rsi
    // callee needs to restore registers r15, r14, r13, r12, rbx (if clobbed) before returning
    asm("mov     (%rsi),%r8;");
    asm("mov 0x08(%rsi),%r9;");
    asm("mov 0x10(%rsi),%r10;");
    asm("mov 0x18(%rsi),%r11;");
    asm("mov 0x20(%rsi),%rcx;");
    asm("mov 0x28(%rsi),%rax;");
    asm("add %r8,%r8;");
    asm("adc %r9,%r9;");
    asm("adc %r10,%r10;");
    asm("adc %r11,%r11;");
    asm("adc %rcx,%rcx;");
    asm("adc %rax,%rax;");
    asm("mov %r8,     (%rdi);");
    asm("mov %r9, 0x08(%rdi);");
    asm("mov %r10,0x10(%rdi);");
    asm("mov %r11,0x18(%rdi);");
    asm("mov %rcx,0x20(%rdi);");
    asm("mov %rax,0x28(%rdi);");
}
#else
void _ldouble(fp* z, const fp* x)
{
    uint64_t carry, _;

    tie(z->d[0], carry) = Add64(x->d[0], x->d[0], 0);
    tie(z->d[1], carry) = Add64(x->d[1], x->d[1], carry);
    tie(z->d[2], carry) = Add64(x->d[2], x->d[2], carry);
    tie(z->d[3], carry) = Add64(x->d[3], x->d[3], carry);
    tie(z->d[4], carry) = Add64(x->d[4], x->d[4], carry);
    tie(z->d[5], _)     = Add64(x->d[5], x->d[5], carry);
}
#endif

#ifdef __x86_64__
void _subtract(fp* z, const fp* x, const fp* y)
{
    // x86_64 calling convention (https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI):
    // z => %rdi
    // x => %rsi
    // y => %rdx
    // callee needs to restore registers r15, r14, r13, r12, rbx before returning
    asm("push %r15;");
    asm("push %r14;");
    asm("push %r13;");
    asm("push %r12;");
    asm("push %rbx;");
    asm("xor %rax,%rax;");
    asm("mov     (%rsi),%r8;");
    asm("mov 0x08(%rsi),%r9;");
    asm("mov 0x10(%rsi),%r10;");
    asm("mov 0x18(%rsi),%r11;");
    asm("mov 0x20(%rsi),%r12;");
    asm("mov 0x28(%rsi),%r13;");
    asm("sub     (%rdx),%r8;");
    asm("sbb 0x08(%rdx),%r9;");
    asm("sbb 0x10(%rdx),%r10;");
    asm("sbb 0x18(%rdx),%r11;");
    asm("sbb 0x20(%rdx),%r12;");
    asm("sbb 0x28(%rdx),%r13;");
    asm("mov $0xb9feffffffffaaab,%r14;");
    asm("mov $0x1eabfffeb153ffff,%r15;");
    asm("mov $0x6730d2a0f6b0f624,%rcx;");
    asm("mov $0x64774b84f38512bf,%rdx;");
    asm("mov $0x4b1ba7b6434bacd7,%rsi;");
    asm("mov $0x1a0111ea397fe69a,%rbx;");
    asm("cmovae %rax,%r14;");
    asm("cmovae %rax,%r15;");
    asm("cmovae %rax,%rcx;");
    asm("cmovae %rax,%rdx;");
    asm("cmovae %rax,%rsi;");
    asm("cmovae %rax,%rbx;");
    asm("add %r14,%r8;");
    asm("adc %r15,%r9;");
    asm("adc %rcx,%r10;");
    asm("adc %rdx,%r11;");
    asm("adc %rsi,%r12;");
    asm("adc %rbx,%r13;");
    asm("mov %r8,     (%rdi);");
    asm("mov %r9, 0x08(%rdi);");
    asm("mov %r10,0x10(%rdi);");
    asm("mov %r11,0x18(%rdi);");
    asm("mov %r12,0x20(%rdi);");
    asm("mov %r13,0x28(%rdi);");
    asm("pop %rbx;");
    asm("pop %r12;");
    asm("pop %r13;");
    asm("pop %r14;");
    asm("pop %r15;");
}
#else
void _subtract(fp* z, const fp* x, const fp* y)
{
    uint64_t b;
    tie(z->d[0], b) = Sub64(x->d[0], y->d[0], 0);
    tie(z->d[1], b) = Sub64(x->d[1], y->d[1], b);
    tie(z->d[2], b) = Sub64(x->d[2], y->d[2], b);
    tie(z->d[3], b) = Sub64(x->d[3], y->d[3], b);
    tie(z->d[4], b) = Sub64(x->d[4], y->d[4], b);
    tie(z->d[5], b) = Sub64(x->d[5], y->d[5], b);
    if(b != 0)
    {
        uint64_t c, _;
        tie(z->d[0], c) = Add64(z->d[0], fp::MODULUS.d[0], 0);
        tie(z->d[1], c) = Add64(z->d[1], fp::MODULUS.d[1], c);
        tie(z->d[2], c) = Add64(z->d[2], fp::MODULUS.d[2], c);
        tie(z->d[3], c) = Add64(z->d[3], fp::MODULUS.d[3], c);
        tie(z->d[4], c) = Add64(z->d[4], fp::MODULUS.d[4], c);
        tie(z->d[5], _) = Add64(z->d[5], fp::MODULUS.d[5], c);
    }
}
#endif

#ifdef __x86_64__
void _lsubtract(fp* z, const fp* x, const fp* y)
{
    // x86_64 calling convention (https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI):
    // z => %rdi
    // x => %rsi
    // y => %rdx
    // callee needs to restore registers r15, r14, r13, r12, rbx before returning
    asm("mov     (%rsi),%r8;");
    asm("mov 0x08(%rsi),%r9;");
    asm("mov 0x10(%rsi),%r10;");
    asm("mov 0x18(%rsi),%r11;");
    asm("mov 0x20(%rsi),%rcx;");
    asm("mov 0x28(%rsi),%rax;");
    asm("sub     (%rdx),%r8;");
    asm("sbb 0x08(%rdx),%r9;");
    asm("sbb 0x10(%rdx),%r10;");
    asm("sbb 0x18(%rdx),%r11;");
    asm("sbb 0x20(%rdx),%rcx;");
    asm("sbb 0x28(%rdx),%rax;");
    asm("mov %r8,     (%rdi);");
    asm("mov %r9, 0x08(%rdi);");
    asm("mov %r10,0x10(%rdi);");
    asm("mov %r11,0x18(%rdi);");
    asm("mov %rcx,0x20(%rdi);");
    asm("mov %rax,0x28(%rdi);");
}
#else
void _lsubtract(fp* z, const fp* x, const fp* y)
{
    uint64_t b, _;
    tie(z->d[0], b) = Sub64(x->d[0], y->d[0], 0);
    tie(z->d[1], b) = Sub64(x->d[1], y->d[1], b);
    tie(z->d[2], b) = Sub64(x->d[2], y->d[2], b);
    tie(z->d[3], b) = Sub64(x->d[3], y->d[3], b);
    tie(z->d[4], b) = Sub64(x->d[4], y->d[4], b);
    tie(z->d[5], _) = Sub64(x->d[5], y->d[5], b);
}
#endif

#ifdef __x86_64__
void __negate(fp* z, const fp* x)
{
    // x86_64 calling convention (https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI):
    // z => %rdi
    // x => %rsi
    // callee needs to restore registers r15, r14, r13, r12, rbx before returning
    asm("mov $0xb9feffffffffaaab,%r8;");
    asm("mov $0x1eabfffeb153ffff,%r9;");
    asm("mov $0x6730d2a0f6b0f624,%r10;");
    asm("mov $0x64774b84f38512bf,%r11;");
    asm("mov $0x4b1ba7b6434bacd7,%rcx;");
    asm("mov $0x1a0111ea397fe69a,%rax;");
    asm("sub     (%rsi),%r8;");
    asm("sbb 0x08(%rsi),%r9;");
    asm("sbb 0x10(%rsi),%r10;");
    asm("sbb 0x18(%rsi),%r11;");
    asm("sbb 0x20(%rsi),%rcx;");
    asm("sbb 0x28(%rsi),%rax;");
    asm("mov %r8,     (%rdi);");
    asm("mov %r9, 0x08(%rdi);");
    asm("mov %r10,0x10(%rdi);");
    asm("mov %r11,0x18(%rdi);");
    asm("mov %rcx,0x20(%rdi);");
    asm("mov %rax,0x28(%rdi);");
}
void _negate(fp* z, const fp* x)
{
    __negate(z, x);
    // put zero check after __neg because gcc messes up %rdi in -O3 (doesn't restore it before inlining asm code)
    if(x->isZero())
    {
        *z = *x;
        return;
    }
}
#else
void _negate(fp* z, const fp* x)
{
    if(x->isZero())
    {
        *z = *x;
        return;
    }
    uint64_t borrow, _;
    tie(z->d[0], borrow) = Sub64(fp::MODULUS.d[0], x->d[0], 0);
    tie(z->d[1], borrow) = Sub64(fp::MODULUS.d[1], x->d[1], borrow);
    tie(z->d[2], borrow) = Sub64(fp::MODULUS.d[2], x->d[2], borrow);
    tie(z->d[3], borrow) = Sub64(fp::MODULUS.d[3], x->d[3], borrow);
    tie(z->d[4], borrow) = Sub64(fp::MODULUS.d[4], x->d[4], borrow);
    tie(z->d[5], _)      = Sub64(fp::MODULUS.d[5], x->d[5], borrow);
}
#endif

#ifdef __x86_64__
void __multiply(fp* z, const fp* x, const fp* y)
{
    // x86_64 calling convention (https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI):
    // z => %rdi (=> stack)
    // x => %rsi
    // y => %rdx (=> %rdi)
    // callee needs to restore registers r15, r14, r13, r12, rbx before returning
    asm("push %rbp;");
    asm("push %r15;");
    asm("push %r14;");
    asm("push %r13;");
    asm("push %r12;");
    asm("push %rbx;");

    // this pushes the constants (INP and MODULUS) used in this function to the stack
    // TODO: should be referenced from data section instead
    asm("mov $0x89f3fffcfffcfffd,%rax;");
    asm("push %rax;");
    asm("mov %rsp,%rbp;");
    asm("mov $0xb9feffffffffaaab,%rax;");
    asm("push %rax;");
    asm("mov $0x1eabfffeb153ffff,%rax;");
    asm("push %rax;");
    asm("mov $0x6730d2a0f6b0f624,%rax;");
    asm("push %rax;");
    asm("mov $0x64774b84f38512bf,%rax;");
    asm("push %rax;");
    asm("mov $0x4b1ba7b6434bacd7,%rax;");
    asm("push %rax;");
    asm("mov $0x1a0111ea397fe69a,%rax;");
    asm("push %rax;");

    asm("mov $0,%r9;");
    asm("mov $0,%r10;");
    asm("mov $0,%r11;");
    asm("mov $0,%r12;");
    asm("mov $0,%r13;");
    asm("mov $0,%r14;");
    asm("mov $0,%r15;");

    asm("push %rdi;");                  // save z for later
    asm("mov %rdx,%rdi;");              // move y to %rdi to free up %rdx for use in mul
    // i0
    asm("mov     (%rsi),%rcx;");
    asm("mov     (%rdi),%rax;");
    asm("mul %rcx;");
    asm("push %rax;");
    asm("mov %rdx,%r8;");
    asm("mov 0x08(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r8;");
    asm("adc %rdx,%r9;");
    asm("mov 0x10(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r9;");
    asm("adc %rdx,%r10;");
    asm("mov 0x18(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r10;");
    asm("adc %rdx,%r11;");
    asm("mov 0x20(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r11;");
    asm("adc %rdx,%r12;");
    asm("mov 0x28(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r12;");
    asm("adc %rdx,%r13;");
    // i1
    asm("mov 0x08(%rsi),%rcx;");
    asm("mov $0,%rbx;");
    asm("mov     (%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r8;");
    asm("adc %rdx,%r9;");
    asm("adc $0,%r10;");
    asm("adc $0,%rbx;");
    asm("push %r8;");
    asm("mov $0,%r8;");
    asm("mov 0x08(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r9;");
    asm("adc %rdx,%r10;");
    asm("adc %rbx,%r11;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x10(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r10;");
    asm("adc %rdx,%r11;");
    asm("adc %rbx,%r12;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x18(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r11;");
    asm("adc %rdx,%r12;");
    asm("adc %rbx,%r13;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x20(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r12;");
    asm("adc %rdx,%r13;");
    asm("adc %rbx,%r14;");
    asm("mov 0x28(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r13;");
    asm("adc %rdx,%r14;");
    // i2
    asm("mov 0x10(%rsi),%rcx;");
    asm("mov $0,%rbx;");
    asm("mov     (%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r9;");
    asm("adc %rdx,%r10;");
    asm("adc $0,%r11;");
    asm("adc $0,%rbx;");
    asm("push %r9;");
    asm("mov $0,%r9;");
    asm("mov 0x08(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r10;");
    asm("adc %rdx,%r11;");
    asm("adc %rbx,%r12;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x10(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r11;");
    asm("adc %rdx,%r12;");
    asm("adc %rbx,%r13;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x18(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r12;");
    asm("adc %rdx,%r13;");
    asm("adc %rbx,%r14;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x20(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r13;");
    asm("adc %rdx,%r14;");
    asm("adc %rbx,%r15;");
    asm("mov 0x28(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r14;");
    asm("adc %rdx,%r15;");
    // i3
    asm("mov 0x18(%rsi),%rcx;");
    asm("mov $0,%rbx;");
    asm("mov     (%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r10;");
    asm("adc %rdx,%r11;");
    asm("adc $0,%r12;");
    asm("adc $0,%rbx;");
    asm("mov 0x08(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r11;");
    asm("adc %rdx,%r12;");
    asm("adc %rbx,%r13;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x10(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r12;");
    asm("adc %rdx,%r13;");
    asm("adc %rbx,%r14;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x18(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r13;");
    asm("adc %rdx,%r14;");
    asm("adc %rbx,%r15;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x20(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r14;");
    asm("adc %rdx,%r15;");
    asm("adc %rbx,%r8;");
    asm("mov 0x28(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r15;");
    asm("adc %rdx,%r8;");
    // i4
    asm("mov 0x20(%rsi),%rcx;");
    asm("mov $0,%rbx;");
    asm("mov     (%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r11;");
    asm("adc %rdx,%r12;");
    asm("adc $0,%r13;");
    asm("adc $0,%rbx;");
    asm("mov 0x08(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r12;");
    asm("adc %rdx,%r13;");
    asm("adc %rbx,%r14;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x10(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r13;");
    asm("adc %rdx,%r14;");
    asm("adc %rbx,%r15;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x18(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r14;");
    asm("adc %rdx,%r15;");
    asm("adc %rbx,%r8;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x20(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r15;");
    asm("adc %rdx,%r8;");
    asm("adc %rbx,%r9;");
    asm("mov 0x28(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r8;");
    asm("adc %rdx,%r9;");
    // i5
    asm("mov 0x28(%rsi),%rcx;");
    asm("mov $0,%rbx;");
    asm("mov     (%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r12;");
    asm("adc %rdx,%r13;");
    asm("adc $0,%r14;");
    asm("adc $0,%rbx;");
    asm("mov 0x08(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r13;");
    asm("adc %rdx,%r14;");
    asm("adc %rbx,%r15;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x10(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r14;");
    asm("adc %rdx,%r15;");
    asm("adc %rbx,%r8;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x18(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r15;");
    asm("adc %rdx,%r8;");
    asm("adc %rbx,%r9;");
    asm("mov $0,%rbx;");
    asm("adc $0,%rbx;");
    asm("mov 0x20(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r8;");
    asm("adc %rdx,%r9;");
    asm("adc $0,%rbx;");
    asm("mov 0x28(%rdi),%rax;");
    asm("mul %rcx;");
    asm("add %rax,%r9;");
    asm("adc %rdx,%rbx;");

    asm("pop %rsi;");
    asm("pop %rdi;");
    asm("pop %rcx;");
    asm("push %rbx;");
    asm("push %r9;");

    // montgomery reduction
    // i0
    asm("mov %rcx,%rax;");
    asm("mulq (%rbp);");
    asm("mov %rax,%r9;");
    asm("mov $0,%rbx;");
    // j0
    asm("mov -0x08(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%rcx;");
    asm("adc %rdx,%rbx;");
    // j1
    asm("mov -0x10(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%rdi;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%rdi;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j2
    asm("mov -0x18(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%rsi;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%rsi;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j3
    asm("mov -0x20(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r10;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r10;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j4
    asm("mov -0x28(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r11;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r11;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j5
    asm("mov -0x30(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r12;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r12;");
    asm("adc %rdx,%r13;");
    asm("adc $0,%rcx;");
    // i1
    asm("mov %rdi,%rax;");
    asm("mulq (%rbp);");
    asm("mov %rax,%r9;");
    asm("mov $0,%rbx;");
    // j0
    asm("mov -0x08(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%rdi;");
    asm("adc %rdx,%rbx;");
    // j1
    asm("mov -0x10(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%rsi;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%rsi;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j2
    asm("mov -0x18(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r10;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r10;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j3
    asm("mov -0x20(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r11;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r11;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j4
    asm("mov -0x28(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r12;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r12;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j5
    asm("mov -0x30(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r13;");
    asm("adc %rdx,%rcx;");
    asm("add %rbx,%r13;");
    asm("adc %rcx,%r14;");
    asm("mov $0,%rcx;");
    asm("adc $0,%rcx;");
    // i2
    asm("mov %rsi,%rax;");
    asm("mulq (%rbp);");
    asm("mov %rax,%r9;");
    asm("mov $0,%rbx;");
    // j0
    asm("mov -0x08(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%rsi;");
    asm("adc %rdx,%rbx;");
    // j1
    asm("mov -0x10(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r10;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r10;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j2
    asm("mov -0x18(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r11;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r11;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j3
    asm("mov -0x20(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r12;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r12;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j4
    asm("mov -0x28(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r13;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r13;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j5
    asm("mov -0x30(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r14;");
    asm("adc %rdx,%rcx;");
    asm("add %rbx,%r14;");
    asm("adc %rcx,%r15;");
    asm("mov $0,%rcx;");
    asm("adc $0,%rcx;");
    // i3
    asm("mov %r10,%rax;");
    asm("mulq (%rbp);");
    asm("mov %rax,%r9;");
    asm("mov $0,%rbx;");
    // j0
    asm("mov -0x08(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r10;");
    asm("adc %rdx,%rbx;");
    // j1
    asm("mov -0x10(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r11;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r11;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j2
    asm("mov -0x18(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r12;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r12;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j3
    asm("mov -0x20(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r13;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r13;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j4
    asm("mov -0x28(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r14;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r14;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j5
    asm("mov -0x30(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r15;");
    asm("adc %rdx,%rcx;");
    asm("add %rbx,%r15;");
    asm("adc %rcx,%r8;");
    asm("mov $0,%rcx;");
    asm("adc $0,%rcx;");
    // i4
    asm("mov %r11,%rax;");
    asm("mulq (%rbp);");
    asm("mov %rax,%r9;");
    asm("mov $0,%rbx;");
    // j0
    asm("mov -0x08(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r11;");
    asm("adc %rdx,%rbx;");
    // j1
    asm("mov -0x10(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r12;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r12;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j2
    asm("mov -0x18(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r13;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r13;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j3
    asm("mov -0x20(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r14;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r14;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j4
    asm("mov -0x28(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r15;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r15;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j5
    asm("mov -0x30(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r8;");
    asm("adc %rdx,%rcx;");
    asm("add %rbx,%r8;");
    asm("pop %rdi;");
    asm("adc %rcx,%rdi;");
    asm("mov $0,%rcx;");
    asm("adc $0,%rcx;");
    // i5
    asm("mov %r12,%rax;");
    asm("mulq (%rbp);");
    asm("mov %rax,%r9;");
    asm("mov $0,%rbx;");
    // j0
    asm("mov -0x08(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r12;");
    asm("adc %rdx,%rbx;");
    // j1
    asm("mov -0x10(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r13;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r13;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j2
    asm("mov -0x18(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r14;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r14;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j3
    asm("mov -0x20(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r15;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r15;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j4
    asm("mov -0x28(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%r8;");
    asm("adc $0,%rdx;");
    asm("add %rbx,%r8;");
    asm("mov $0,%rbx;");
    asm("adc %rdx,%rbx;");
    // j5
    asm("mov -0x30(%rbp),%rax;");
    asm("mul %r9;");
    asm("add %rax,%rdi;");
    asm("adc %rdx,%rcx;");
    asm("add %rbx,%rdi;");
    asm("pop %rsi;");
    asm("adc %rsi,%rcx;");

    // modular reduction
    asm("mov %r13,%r10;");
    asm("sub -0x08(%rbp),%r10;");
    asm("mov %r14,%r11;");
    asm("sbb -0x10(%rbp),%r11;");
    asm("mov %r15,%r12;");
    asm("sbb -0x18(%rbp),%r12;");
    asm("mov %r8,%rax;");
    asm("sbb -0x20(%rbp),%rax;");
    asm("mov %rdi,%rbx;");
    asm("sbb -0x28(%rbp),%rbx;");
    asm("mov %rcx,%r9;");
    asm("sbb -0x30(%rbp),%r9;");

    // out
    asm("pop %rsi;");
    asm("cmovae %r10,%r13;");
    asm("mov %r13,    (%rsi);");
    asm("cmovae %r11,%r14;");
    asm("mov %r14,0x08(%rsi);");
    asm("cmovae %r12,%r15;");
    asm("mov %r15,0x10(%rsi);");
    asm("cmovae %rax,%r8;");
    asm("mov %r8,0x18(%rsi);");
    asm("cmovae %rbx,%rdi;");
    asm("mov %rdi,0x20(%rsi);");
    asm("cmovae %r9,%rcx;");
    asm("mov %rcx,0x28(%rsi);");

    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");

    asm("pop %rbx;");
    asm("pop %r12;");
    asm("pop %r13;");
    asm("pop %r14;");
    asm("pop %r15;");
    asm("pop %rbp;");
}
void __mul_ex(fp* z, const fp* x, const fp* y)
{
    // x86_64 calling convention (https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI):
    // z => %rdi (=> stack)
    // x => %rsi
    // y => %rdx (=> %rdi)
    // callee needs to restore registers r15, r14, r13, r12, rbx before returning
    asm("push %rbp;");
    asm("push %r15;");
    asm("push %r14;");
    asm("push %r13;");
    asm("push %r12;");
    asm("push %rbx;");

    // this pushes the constants (INP and MODULUS) used in this function to the stack
    // TODO: should be referenced from data section instead
    asm("mov $0x89f3fffcfffcfffd,%rax;");
    asm("push %rax;");
    asm("mov %rsp,%rbp;");
    asm("mov $0xb9feffffffffaaab,%rax;");
    asm("push %rax;");
    asm("mov $0x1eabfffeb153ffff,%rax;");
    asm("push %rax;");
    asm("mov $0x6730d2a0f6b0f624,%rax;");
    asm("push %rax;");
    asm("mov $0x64774b84f38512bf,%rax;");
    asm("push %rax;");
    asm("mov $0x4b1ba7b6434bacd7,%rax;");
    asm("push %rax;");
    asm("mov $0x1a0111ea397fe69a,%rax;");
    asm("push %rax;");

    asm("push %rdi;");                  // save z for later
    asm("mov %rdx,%rdi;");              // move y to %rdi to free up %rdx for use in mulx
    asm("xor %rax,%rax;");
    // i0
    asm("mov      (%rsi),%rdx;");       // x0
    asm("mulx     (%rdi),%rax,%rcx;");  // x0 * y0
    asm("push %rax;");
    asm("mulx 0x08(%rdi),%rax,%r8;");   // x0 * y1
    asm("adcx %rax,%rcx;");
    asm("mulx 0x10(%rdi),%rax,%r9;");   // x0 * y2
    asm("adcx %rax,%r8;");
    asm("mulx 0x18(%rdi),%rax,%r10;");  // x0 * y3
    asm("adcx %rax,%r9;");
    asm("mulx 0x20(%rdi),%rax,%r11;");  // x0 * y4
    asm("adcx %rax,%r10;");
    asm("mulx 0x28(%rdi),%rax,%r12;");  // x0 * y5
    asm("adcx %rax,%r11;");
    asm("adc  $0,%r12;");
    // i1
    asm("mov 0x08(%rsi),%rdx;");        // x1
    asm("xor %r13,%r13;");
    asm("mulx     (%rdi),%rax,%rbx;");  // x1 * y0
    asm("adox %rax,%rcx;");
    asm("adcx %rbx,%r8;");
    asm("push %rcx;");
    asm("mulx 0x08(%rdi),%rax,%rbx;");  // x1 * y1
    asm("adox %rax,%r8;");
    asm("adcx %rbx,%r9;");
    asm("mulx 0x10(%rdi),%rax,%rbx;");  // x1 * y2
    asm("adox %rax,%r9;");
    asm("adcx %rbx,%r10;");
    asm("mulx 0x18(%rdi),%rax,%rbx;");  // x1 * y3
    asm("adox %rax,%r10;");
    asm("adcx %rbx,%r11;");
    asm("mulx 0x20(%rdi),%rax,%rbx;");  // x1 * y4
    asm("adox %rax,%r11;");
    asm("adcx %rbx,%r12;");
    asm("mulx 0x28(%rdi),%rax,%rbx;");  // x1 * y5
    asm("adox %rax,%r12;");
    asm("adox %r13,%r13;");
    asm("adcx %rbx,%r13;");
    // i2
    asm("mov 0x10(%rsi),%rdx;");        // x2
    asm("xor %r14,%r14;");
    asm("mulx     (%rdi),%rax,%rbx;");  // x2 * y0
    asm("adox %rax,%r8;");
    asm("adcx %rbx,%r9;");
    asm("mulx 0x08(%rdi),%rax,%rbx;");  // x2 * y1
    asm("adox %rax,%r9;");
    asm("adcx %rbx,%r10;");
    asm("mulx 0x10(%rdi),%rax,%rbx;");  // x2 * y2
    asm("adox %rax,%r10;");
    asm("adcx %rbx,%r11;");
    asm("mulx 0x18(%rdi),%rax,%rbx;");  // x2 * y3
    asm("adox %rax,%r11;");
    asm("adcx %rbx,%r12;");
    asm("mulx 0x20(%rdi),%rax,%rbx;");  // x2 * y4
    asm("adox %rax,%r12;");
    asm("adcx %rbx,%r13;");
    asm("mulx 0x28(%rdi),%rax,%rbx;");  // x2 * y5
    asm("adox %rax,%r13;");
    asm("adox %r14,%r14;");
    asm("adcx %rbx,%r14;");
    // i3
    asm("mov 0x18(%rsi),%rdx;");        // x3
    asm("xor %r15,%r15;");
    asm("mulx     (%rdi),%rax,%rbx;");  // x3 * y0
    asm("adox %rax,%r9;");
    asm("adcx %rbx,%r10;");
    asm("mulx 0x08(%rdi),%rax,%rbx;");  // x3 * y1
    asm("adox %rax,%r10;");
    asm("adcx %rbx,%r11;");
    asm("mulx 0x10(%rdi),%rax,%rbx;");  // x3 * y2
    asm("adox %rax,%r11;");
    asm("adcx %rbx,%r12;");
    asm("mulx 0x18(%rdi),%rax,%rbx;");  // x3 * y3
    asm("adox %rax,%r12;");
    asm("adcx %rbx,%r13;");
    asm("mulx 0x20(%rdi),%rax,%rbx;");  // x3 * y4
    asm("adox %rax,%r13;");
    asm("adcx %rbx,%r14;");
    asm("mulx 0x28(%rdi),%rax,%rbx;");  // x3 * y5
    asm("adox %rax,%r14;");
    asm("adox %r15,%r15;");
    asm("adcx %rbx,%r15;");
    // i4
    asm("mov 0x20(%rsi),%rdx;");        // x4
    asm("xor %rcx,%rcx;");
    asm("mulx     (%rdi),%rax,%rbx;");  // x4 * y0
    asm("adox %rax,%r10;");
    asm("adcx %rbx,%r11;");
    asm("mulx 0x08(%rdi),%rax,%rbx;");  // x4 * y1
    asm("adox %rax,%r11;");
    asm("adcx %rbx,%r12;");
    asm("mulx 0x10(%rdi),%rax,%rbx;");  // x4 * y2
    asm("adox %rax,%r12;");
    asm("adcx %rbx,%r13;");
    asm("mulx 0x18(%rdi),%rax,%rbx;");  // x4 * y3
    asm("adox %rax,%r13;");
    asm("adcx %rbx,%r14;");
    asm("mulx 0x20(%rdi),%rax,%rbx;");  // x4 * y4
    asm("adox %rax,%r14;");
    asm("adcx %rbx,%r15;");
    asm("mulx 0x28(%rdi),%rax,%rbx;");  // x4 * y5
    asm("adox %rax,%r15;");
    asm("adox %rcx,%rcx;");
    asm("adcx %rbx,%rcx;");
    // i5
    asm("mov 0x28(%rsi),%rdx;");        // x5
    asm("xor %rsi,%rsi;");
    asm("mulx     (%rdi),%rax,%rbx;");  // x5 * y0
    asm("adox %rax,%r11;");
    asm("adcx %rbx,%r12;");
    asm("mulx 0x08(%rdi),%rax,%rbx;");  // x5 * y1
    asm("adox %rax,%r12;");
    asm("adcx %rbx,%r13;");
    asm("mulx 0x10(%rdi),%rax,%rbx;");  // x5 * y2
    asm("adox %rax,%r13;");
    asm("adcx %rbx,%r14;");
    asm("mulx 0x18(%rdi),%rax,%rbx;");  // x5 * y3
    asm("adox %rax,%r14;");
    asm("adcx %rbx,%r15;");
    asm("mulx 0x20(%rdi),%rax,%rbx;");  // x5 * y4
    asm("adox %rax,%r15;");
    asm("adcx %rbx,%rcx;");
    asm("mulx 0x28(%rdi),%rax,%rbx;");  // x5 * y5
    asm("adox %rax,%rcx;");
    asm("adox %rbx,%rsi;");
    asm("adc $0,%rsi;");

    asm("pop %rdi;");
    asm("pop %rbx;");
    asm("push %rsi;");

    // montgomery reduction
    asm("xor %rax,%rax;");
    // i0
    asm("mov %rbx, %rdx");
    asm("mulx      (%rbp),%rdx,%rsi;");
    asm("mulx -0x08(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); // j0
    asm("adox %rax,%rbx;");
    asm("adcx %rsi,%rdi;");
    asm("mulx -0x10(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); // j1
    asm("adox %rax,%rdi;");
    asm("adcx %rsi,%r8;");
    asm("mulx -0x18(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); // j2
    asm("adox %rax,%r8;");
    asm("adcx %rsi,%r9;");
    asm("mulx -0x20(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); // j3
    asm("adox %rax,%r9;");
    asm("adcx %rsi,%r10;");
    asm("mulx -0x28(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); // j4
    asm("adox %rax,%r10;");
    asm("adcx %rsi,%r11;");
    asm("mulx -0x30(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); // j5
    asm("adox %rax,%r11;");
    asm("adcx %rsi,%r12;");
    asm("adox %rbx,%r12;");
    asm("adcx %rbx,%rbx;");
    asm("mov $0,%rax;");
    asm("adox %rax,%rbx;");
    asm("xor %rax,%rax;");
    // i1
    asm("mov %rdi,%rdx;");
    asm("mulx      (%rbp),%rdx,%rsi;");
    asm("mulx -0x08(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); // j0
    asm("adox %rax,%rdi;");
    asm("adcx %rsi,%r8;");
    asm("mulx -0x10(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); // j1
    asm("adox %rax,%r8;");
    asm("adcx %rsi,%r9;");
    asm("mulx -0x18(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); // j2
    asm("adox %rax,%r9;");
    asm("adcx %rsi,%r10;");
    asm("mulx -0x20(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); // j3
    asm("adox %rax,%r10;");
    asm("adcx %rsi,%r11;");
    asm("mulx -0x28(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); // j4
    asm("adox %rax,%r11;");
    asm("adcx %rsi,%r12;");
    asm("mulx -0x30(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); // j5
    asm("adox %rax,%r12;");
    asm("adcx %rsi,%r13;");
    asm("adox %rbx,%r13;");
    asm("adcx %rdi,%rdi;");
    asm("mov $0,%rax;");
    asm("adox %rax,%rdi;");
    asm("xor %rax,%rax;");
    // i2
    asm("mov %r8,%rdx;");
    asm("mulx      (%rbp),%rdx,%rsi;");
    asm("mulx -0x08(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); // j0
    asm("adox %rax,%r8;");
    asm("adcx %rsi,%r9;");
    asm("mulx -0x10(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); // j1
    asm("adox %rax,%r9;");
    asm("adcx %rsi,%r10;");
    asm("mulx -0x18(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); // j2
    asm("adox %rax,%r10;");
    asm("adcx %rsi,%r11;");
    asm("mulx -0x20(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); // j3
    asm("adox %rax,%r11;");
    asm("adcx %rsi,%r12;");
    asm("mulx -0x28(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); // j4
    asm("adox %rax,%r12;");
    asm("adcx %rsi,%r13;");
    asm("mulx -0x30(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); // j5
    asm("adox %rax,%r13;");
    asm("adcx %rsi,%r14;");
    asm("adox %rdi,%r14;");
    asm("adcx %r8,%r8;");
    asm("mov $0,%rax;");
    asm("adox %rax,%r8;");
    asm("xor %rax,%rax;");
    // i3
    asm("mov %r9,%rdx;");
    asm("mulx      (%rbp),%rdx,%rsi;");
    asm("mulx -0x08(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); // j0
    asm("adox %rax,%r9;");
    asm("adcx %rsi,%r10;");
    asm("mulx -0x10(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); // j1
    asm("adox %rax,%r10;");
    asm("adcx %rsi,%r11;");
    asm("mulx -0x18(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); // j2
    asm("adox %rax,%r11;");
    asm("adcx %rsi,%r12;");
    asm("mulx -0x20(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); // j3
    asm("adox %rax,%r12;");
    asm("adcx %rsi,%r13;");
    asm("mulx -0x28(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); // j4
    asm("adox %rax,%r13;");
    asm("adcx %rsi,%r14;");
    asm("mulx -0x30(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); // j5
    asm("adox %rax,%r14;");
    asm("adcx %rsi,%r15;");
    asm("adox %r8,%r15;");
    asm("adcx %r9,%r9;");
    asm("mov $0,%rax;");
    asm("adox %rax,%r9;");
    asm("xor %rax,%rax;");
    // i4
    asm("mov %r10,%rdx;");
    asm("mulx      (%rbp),%rdx,%rsi;");
    asm("mulx -0x08(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); // j0
    asm("adox %rax,%r10;");
    asm("adcx %rsi,%r11;");
    asm("mulx -0x10(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); // j1
    asm("adox %rax,%r11;");
    asm("adcx %rsi,%r12;");
    asm("mulx -0x18(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); // j2
    asm("adox %rax,%r12;");
    asm("adcx %rsi,%r13;");
    asm("mulx -0x20(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); // j3
    asm("adox %rax,%r13;");
    asm("adcx %rsi,%r14;");
    asm("mulx -0x28(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); // j4
    asm("adox %rax,%r14;");
    asm("adcx %rsi,%r15;");
    asm("mulx -0x30(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); // j5
    asm("adox %rax,%r15;");
    asm("adcx %rsi,%rcx;");
    asm("adox %r9,%rcx;");
    asm("adcx %r10,%r10;");
    asm("mov $0,%rax;");
    asm("adox %rax,%r10;");
    asm("xor %rax,%rax;");
    // i5
    asm("mov %r11,%rdx;");
    asm("mulx      (%rbp),%rdx,%rsi;");
    asm("mulx -0x08(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); // j0
    asm("adox %rax,%r11;");
    asm("adcx %rsi,%r12;");
    asm("mulx -0x10(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); // j1
    asm("adox %rax,%r12;");
    asm("adcx %rsi,%r13;");
    asm("mulx -0x18(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); // j2
    asm("adox %rax,%r13;");
    asm("adcx %rsi,%r14;");
    asm("mulx -0x20(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); // j3
    asm("adox %rax,%r14;");
    asm("adcx %rsi,%r15;");
    asm("mulx -0x28(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); // j4
    asm("adox %rax,%r15;");
    asm("adcx %rsi,%rcx;");
    asm("mulx -0x30(%rbp),%rax,%rsi;");
    //asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); // j5
    asm("adox %rax,%rcx;");
    asm("pop %rbx;");
    asm("adcx %rsi,%rbx;");
    asm("adox %r10,%rbx;");
    // modular reduction
    asm("mov %r12,%rax;");
    asm("sub -0x08(%rbp),%rax;");
    //asm("sub %0,%%rax;" : : "m" (fp::MODULUS.d[0]));
    asm("mov %r13,%rsi;");
    asm("sbb -0x10(%rbp),%rsi;");
    //asm("sbb %0,%%rsi;" : : "m" (fp::MODULUS.d[1]));
    asm("mov %r14,%rdi;");
    asm("sbb -0x18(%rbp),%rdi;");
    //asm("sbb %0,%%rdi;" : : "m" (fp::MODULUS.d[2]));
    asm("mov %r15,%r8;");
    asm("sbb -0x20(%rbp),%r8;");
    //asm("sbb %0,%%r8;" : : "m" (fp::MODULUS.d[3]));
    asm("mov %rcx,%r9;");
    asm("sbb -0x28(%rbp),%r9;");
    //asm("sbb %0,%%r9;" : : "m" (fp::MODULUS.d[4]));
    asm("mov %rbx,%r10;");
    asm("sbb -0x30(%rbp),%r10;");
    //asm("sbb %0,%%r10;" : : "m" (fp::MODULUS.d[5]));
    // out
    asm("pop %r11;");
    asm("cmovae %rax,%r12;");
    asm("mov %r12,(%r11);");
    asm("cmovae %rsi,%r13;");
    asm("mov %r13,0x8(%r11);");
    asm("cmovae %rdi,%r14;");
    asm("mov %r14,0x10(%r11);");
    asm("cmovae %r8,%r15;");
    asm("mov %r15,0x18(%r11);");
    asm("cmovae %r9,%rcx");
    asm("mov %rcx,0x20(%r11);");
    asm("cmovae %r10,%rbx;");
    asm("mov %rbx,0x28(%r11);");

    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");
    asm("pop %rax;");

    asm("pop %rbx;");
    asm("pop %r12;");
    asm("pop %r13;");
    asm("pop %r14;");
    asm("pop %r15;");
    asm("pop %rbp;");
}

typedef void (*blsmul_func_t)(fp*, const fp*, const fp*);

static bool cpu_has_bmi2_and_adx() {
    // borrowed from: https://github.com/Mysticial/FeatureDetector/blob/master/src/x86/cpu_x86.cpp
    int32_t info[4];
    __cpuid_count(0, 0, info[0], info[1], info[2], info[3]);
    int nIds = info[0];
    if(nIds >= 0x00000007) {
        __cpuid_count(0x00000007, 0, info[0], info[1], info[2], info[3]);
        if((info[1] & (1 <<  8)) && (info[1] & (1 << 19))) // BMI2 && ADX
            return true;
    }
    return false;
}

#ifdef __ELF__
extern "C" char** _dl_argv;

extern "C" blsmul_func_t __attribute__((no_sanitize_address)) resolve_blsmul() {
    int argc = *(int*)(_dl_argv - 1);
    char** my_environ = (char**)(_dl_argv + argc + 1);
    while(*my_environ != nullptr) {
       const char disable_str[] = "BLS_DISABLE_BMI2";
        if(strncmp(*my_environ++, disable_str, strlen(disable_str)) == 0)
           return __multiply;
    }

    if(cpu_has_bmi2_and_adx())
       return __mul_ex;
    return __multiply;
}

void _multiply(fp*, const fp*, const fp*) __attribute__((ifunc("resolve_blsmul")));
#else
blsmul_func_t _multiply = __multiply;

struct bls_mul_init {
   bls_mul_init() {
      if(cpu_has_bmi2_and_adx())
         _multiply = __mul_ex;
   }
};
static bls_mul_init the_bls_mul_init;

#endif //__ELF__
#else
void _multiply(fp* z, const fp* x, const fp* y)
{
    array<uint64_t, 6> t;
    array<uint64_t, 3> c;
    {
        // round 0
        uint64_t v = x->d[0];
        tie(c[1], c[0]) = Mul64(v, y->d[0]);
        uint64_t m = c[0] * fp::INP;
        c[2] = madd0(m, fp::MODULUS.d[0], c[0]);
        tie(c[1], c[0]) = madd1(v, y->d[1], c[1]);
        tie(c[2], t[0]) = madd2(m, fp::MODULUS.d[1], c[2], c[0]);
        tie(c[1], c[0]) = madd1(v, y->d[2], c[1]);
        tie(c[2], t[1]) = madd2(m, fp::MODULUS.d[2], c[2], c[0]);
        tie(c[1], c[0]) = madd1(v, y->d[3], c[1]);
        tie(c[2], t[2]) = madd2(m, fp::MODULUS.d[3], c[2], c[0]);
        tie(c[1], c[0]) = madd1(v, y->d[4], c[1]);
        tie(c[2], t[3]) = madd2(m, fp::MODULUS.d[4], c[2], c[0]);
        tie(c[1], c[0]) = madd1(v, y->d[5], c[1]);
        tie(t[5], t[4]) = madd3(m, fp::MODULUS.d[5], c[0], c[2], c[1]);
    }
    {
        // round 1
        uint64_t v = x->d[1];
        tie(c[1], c[0]) = madd1(v, y->d[0], t[0]);
        uint64_t m = c[0] * fp::INP;
        c[2] = madd0(m, fp::MODULUS.d[0], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[1], c[1], t[1]);
        tie(c[2], t[0]) = madd2(m, fp::MODULUS.d[1], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[2], c[1], t[2]);
        tie(c[2], t[1]) = madd2(m, fp::MODULUS.d[2], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[3], c[1], t[3]);
        tie(c[2], t[2]) = madd2(m, fp::MODULUS.d[3], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[4], c[1], t[4]);
        tie(c[2], t[3]) = madd2(m, fp::MODULUS.d[4], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[5], c[1], t[5]);
        tie(t[5], t[4]) = madd3(m, fp::MODULUS.d[5], c[0], c[2], c[1]);
    }
    {
        // round 2
        uint64_t v = x->d[2];
        tie(c[1], c[0]) = madd1(v, y->d[0], t[0]);
        uint64_t m = c[0] * fp::INP;
        c[2] = madd0(m, fp::MODULUS.d[0], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[1], c[1], t[1]);
        tie(c[2], t[0]) = madd2(m, fp::MODULUS.d[1], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[2], c[1], t[2]);
        tie(c[2], t[1]) = madd2(m, fp::MODULUS.d[2], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[3], c[1], t[3]);
        tie(c[2], t[2]) = madd2(m, fp::MODULUS.d[3], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[4], c[1], t[4]);
        tie(c[2], t[3]) = madd2(m, fp::MODULUS.d[4], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[5], c[1], t[5]);
        tie(t[5], t[4]) = madd3(m, fp::MODULUS.d[5], c[0], c[2], c[1]);
    }
    {
        // round 3
        uint64_t v = x->d[3];
        tie(c[1], c[0]) = madd1(v, y->d[0], t[0]);
        uint64_t m = c[0] * fp::INP;
        c[2] = madd0(m, fp::MODULUS.d[0], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[1], c[1], t[1]);
        tie(c[2], t[0]) = madd2(m, fp::MODULUS.d[1], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[2], c[1], t[2]);
        tie(c[2], t[1]) = madd2(m, fp::MODULUS.d[2], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[3], c[1], t[3]);
        tie(c[2], t[2]) = madd2(m, fp::MODULUS.d[3], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[4], c[1], t[4]);
        tie(c[2], t[3]) = madd2(m, fp::MODULUS.d[4], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[5], c[1], t[5]);
        tie(t[5], t[4]) = madd3(m, fp::MODULUS.d[5], c[0], c[2], c[1]);
    }
    {
        // round 4
        uint64_t v = x->d[4];
        tie(c[1], c[0]) = madd1(v, y->d[0], t[0]);
        uint64_t m = c[0] * fp::INP;
        c[2] = madd0(m, fp::MODULUS.d[0], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[1], c[1], t[1]);
        tie(c[2], t[0]) = madd2(m, fp::MODULUS.d[1], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[2], c[1], t[2]);
        tie(c[2], t[1]) = madd2(m, fp::MODULUS.d[2], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[3], c[1], t[3]);
        tie(c[2], t[2]) = madd2(m, fp::MODULUS.d[3], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[4], c[1], t[4]);
        tie(c[2], t[3]) = madd2(m, fp::MODULUS.d[4], c[2], c[0]);
        tie(c[1], c[0]) = madd2(v, y->d[5], c[1], t[5]);
        tie(t[5], t[4]) = madd3(m, fp::MODULUS.d[5], c[0], c[2], c[1]);
    }
    {
        // round 5
        uint64_t v = x->d[5];
        tie(c[1], c[0]) = madd1(v, y->d[0], t[0]);
        uint64_t m = c[0] * fp::INP;
        c[2] = madd0(m, fp::MODULUS.d[0], c[0]);
        tie(c[1], c[0])    = madd2(v, y->d[1], c[1], t[1]);
        tie(c[2], z->d[0]) = madd2(m, fp::MODULUS.d[1], c[2], c[0]);
        tie(c[1], c[0])    = madd2(v, y->d[2], c[1], t[2]);
        tie(c[2], z->d[1]) = madd2(m, fp::MODULUS.d[2], c[2], c[0]);
        tie(c[1], c[0])    = madd2(v, y->d[3], c[1], t[3]);
        tie(c[2], z->d[2]) = madd2(m, fp::MODULUS.d[3], c[2], c[0]);
        tie(c[1], c[0])    = madd2(v, y->d[4], c[1], t[4]);
        tie(c[2], z->d[3]) = madd2(m, fp::MODULUS.d[4], c[2], c[0]);
        tie(c[1], c[0])    = madd2(v, y->d[5], c[1], t[5]);
        tie(z->d[5], z->d[4]) = madd3(m, fp::MODULUS.d[5], c[0], c[2], c[1]);
    }

    // if z > q --> z -= q
    // note: this is NOT constant time
    if(!(z->d[5] < fp::MODULUS.d[5] || (z->d[5] == fp::MODULUS.d[5] && (z->d[4] < fp::MODULUS.d[4] || (z->d[4] == fp::MODULUS.d[4] && (z->d[3] < fp::MODULUS.d[3] || (z->d[3] == fp::MODULUS.d[3] && (z->d[2] < fp::MODULUS.d[2] || (z->d[2] == fp::MODULUS.d[2] && (z->d[1] < fp::MODULUS.d[1] || (z->d[1] == fp::MODULUS.d[1] && (z->d[0] < fp::MODULUS.d[0]))))))))))))
    {
        uint64_t b, _;
        tie(z->d[0], b) = Sub64(z->d[0], fp::MODULUS.d[0], 0);
        tie(z->d[1], b) = Sub64(z->d[1], fp::MODULUS.d[1], b);
        tie(z->d[2], b) = Sub64(z->d[2], fp::MODULUS.d[2], b);
        tie(z->d[3], b) = Sub64(z->d[3], fp::MODULUS.d[3], b);
        tie(z->d[4], b) = Sub64(z->d[4], fp::MODULUS.d[4], b);
        tie(z->d[5], _) = Sub64(z->d[5], fp::MODULUS.d[5], b);
    }
}
#endif

#ifdef __x86_64__
void _square(fp* z, const fp* x)
{
    #ifdef __clang__
    // The clang compiler completely optimizes out the _square() function and inlines __multiply() wherever
    // it occurs. However, for some reason the compiler forgets that it has to move the third
    // parameter ('y') of __multiply() into %rdx according to the calling convention. The first two
    // parameters, 'z' (%rdi) and 'x' (%rsi), are set properly because they are the exact same as for
    // __square(). But the third parameter (which sould be 'x' as well) is somehow ignored by the
    // clang compiler. So we need to help out by moving it into %rdx before calling __multiply().
    // This is probably a bug in clang!
    asm("mov %rsi,%rdx;");
    #endif
    __multiply(z, x, x);
}
#else
void _square(fp* z, const fp* x)
{
    array<uint64_t, 6> p;
    uint64_t u, v;
    {
        // round 0
        tie(u, p[0]) = Mul64(x->d[0], x->d[0]);
        uint64_t m = p[0] * fp::INP;
        uint64_t C = madd0(m, fp::MODULUS.d[0], p[0]);
        uint64_t t, _;
        tie(t, u, v) = madd1sb(x->d[0], x->d[1], u);
        tie(C, p[0]) = madd2(m, fp::MODULUS.d[1], v, C);
        tie(t, u, v) = madd1s(x->d[0], x->d[2], t, u);
        tie(C, p[1]) = madd2(m, fp::MODULUS.d[2], v, C);
        tie(t, u, v) = madd1s(x->d[0], x->d[3], t, u);
        tie(C, p[2]) = madd2(m, fp::MODULUS.d[3], v, C);
        tie(t, u, v) = madd1s(x->d[0], x->d[4], t, u);
        tie(C, p[3]) = madd2(m, fp::MODULUS.d[4], v, C);
        tie(_, u, v) = madd1s(x->d[0], x->d[5], t, u);
        tie(p[5], p[4]) = madd3(m, fp::MODULUS.d[5], v, C, u);
    }
    {
        // round 1
        uint64_t m = p[0] * fp::INP;
        uint64_t C = madd0(m, fp::MODULUS.d[0], p[0]);
        tie(u, v) = madd1(x->d[1], x->d[1], p[1]);
        tie(C, p[0]) = madd2(m, fp::MODULUS.d[1], v, C);
        uint64_t t, _;
        tie(t, u, v) = madd2sb(x->d[1], x->d[2], p[2], u);
        tie(C, p[1]) = madd2(m, fp::MODULUS.d[2], v, C);
        tie(t, u, v) = madd2s(x->d[1], x->d[3], p[3], t, u);
        tie(C, p[2]) = madd2(m, fp::MODULUS.d[3], v, C);
        tie(t, u, v) = madd2s(x->d[1], x->d[4], p[4], t, u);
        tie(C, p[3]) = madd2(m, fp::MODULUS.d[4], v, C);
        tie(_, u, v) = madd2s(x->d[1], x->d[5], p[5], t, u);
        tie(p[5], p[4]) = madd3(m, fp::MODULUS.d[5], v, C, u);
    }
    {
        // round 2
        uint64_t m = p[0] * fp::INP;
        uint64_t C = madd0(m, fp::MODULUS.d[0], p[0]);
        tie(C, p[0]) = madd2(m, fp::MODULUS.d[1], p[1], C);
        tie(u, v) = madd1(x->d[2], x->d[2], p[2]);
        tie(C, p[1]) = madd2(m, fp::MODULUS.d[2], v, C);
        uint64_t t, _;
        tie(t, u, v) = madd2sb(x->d[2], x->d[3], p[3], u);
        tie(C, p[2]) = madd2(m, fp::MODULUS.d[3], v, C);
        tie(t, u, v) = madd2s(x->d[2], x->d[4], p[4], t, u);
        tie(C, p[3]) = madd2(m, fp::MODULUS.d[4], v, C);
        tie(_, u, v) = madd2s(x->d[2], x->d[5], p[5], t, u);
        tie(p[5], p[4]) = madd3(m, fp::MODULUS.d[5], v, C, u);
    }
    {
        // round 3
        uint64_t m = p[0] * fp::INP;
        uint64_t C = madd0(m, fp::MODULUS.d[0], p[0]);
        tie(C, p[0]) = madd2(m, fp::MODULUS.d[1], p[1], C);
        tie(C, p[1]) = madd2(m, fp::MODULUS.d[2], p[2], C);
        tie(u, v) = madd1(x->d[3], x->d[3], p[3]);
        tie(C, p[2]) = madd2(m, fp::MODULUS.d[3], v, C);
        uint64_t t, _;
        tie(t, u, v) = madd2sb(x->d[3], x->d[4], p[4], u);
        tie(C, p[3]) = madd2(m, fp::MODULUS.d[4], v, C);
        tie(_, u, v) = madd2s(x->d[3], x->d[5], p[5], t, u);
        tie(p[5], p[4]) = madd3(m, fp::MODULUS.d[5], v, C, u);
    }
    {
        // round 4
        uint64_t m = p[0] * fp::INP;
        uint64_t C = madd0(m, fp::MODULUS.d[0], p[0]);
        tie(C, p[0]) = madd2(m, fp::MODULUS.d[1], p[1], C);
        tie(C, p[1]) = madd2(m, fp::MODULUS.d[2], p[2], C);
        tie(C, p[2]) = madd2(m, fp::MODULUS.d[3], p[3], C);
        tie(u, v) = madd1(x->d[4], x->d[4], p[4]);
        tie(C, p[3]) = madd2(m, fp::MODULUS.d[4], v, C);
        uint64_t _;
        tie(_, u, v) = madd2sb(x->d[4], x->d[5], p[5], u);
        tie(p[5], p[4]) = madd3(m, fp::MODULUS.d[5], v, C, u);
    }
    {
        // round 5
        uint64_t m = p[0] * fp::INP;
        uint64_t C = madd0(m, fp::MODULUS.d[0], p[0]);
        tie(C, z->d[0]) = madd2(m, fp::MODULUS.d[1], p[1], C);
        tie(C, z->d[1]) = madd2(m, fp::MODULUS.d[2], p[2], C);
        tie(C, z->d[2]) = madd2(m, fp::MODULUS.d[3], p[3], C);
        tie(C, z->d[3]) = madd2(m, fp::MODULUS.d[4], p[4], C);
        tie(u, v) = madd1(x->d[5], x->d[5], p[5]);
        tie(z->d[5], z->d[4]) = madd3(m, fp::MODULUS.d[5], v, C, u);
    }

    // if z > q --> z -= q
    // note: this is NOT constant time
    if(!(z->d[5] < fp::MODULUS.d[5] || (z->d[5] == fp::MODULUS.d[5] && (z->d[4] < fp::MODULUS.d[4] || (z->d[4] == fp::MODULUS.d[4] && (z->d[3] < fp::MODULUS.d[3] || (z->d[3] == fp::MODULUS.d[3] && (z->d[2] < fp::MODULUS.d[2] || (z->d[2] == fp::MODULUS.d[2] && (z->d[1] < fp::MODULUS.d[1] || (z->d[1] == fp::MODULUS.d[1] && (z->d[0] < fp::MODULUS.d[0]))))))))))))
    {
        uint64_t b, _;
        tie(z->d[0], b) = Sub64(z->d[0], fp::MODULUS.d[0], 0);
        tie(z->d[1], b) = Sub64(z->d[1], fp::MODULUS.d[1], b);
        tie(z->d[2], b) = Sub64(z->d[2], fp::MODULUS.d[2], b);
        tie(z->d[3], b) = Sub64(z->d[3], fp::MODULUS.d[3], b);
        tie(z->d[4], b) = Sub64(z->d[4], fp::MODULUS.d[4], b);
        tie(z->d[5], _) = Sub64(z->d[5], fp::MODULUS.d[5], b);
    }
}
#endif

// Add64 returns the sum with carry of x, y and carry: sum = x + y + carry.
// The carry input must be 0 or 1; otherwise the behavior is undefined.
// The carryOut output is guaranteed to be 0 or 1.
//
// This function's execution time does not depend on the inputs.
tuple<uint64_t, uint64_t> Add64(
    const uint64_t& x,
    const uint64_t& y,
    const uint64_t& carry
)
{
    uint64_t sum = (x + y) + carry;
    uint64_t carryOut = (sum < x);
    return {sum, carryOut};
}

// Sub64 returns the difference of x, y and borrow: diff = x - y - borrow.
// The borrow input must be 0 or 1; otherwise the behavior is undefined.
// The borrowOut output is guaranteed to be 0 or 1.
//
// This function's execution time does not depend on the inputs.
tuple<uint64_t, uint64_t> Sub64(
    const uint64_t& x,
    const uint64_t& y,
    const uint64_t& borrow
)
{
    uint64_t diff = x - y - borrow;
    uint64_t borrowOut = (diff > x);
    return {diff, borrowOut};
}

// Mul64 returns the 128-bit product of x and y: (hi, lo) = x * y
// with the product bits' upper half returned in hi and the lower
// half returned in lo.
//
// This function's execution time does not depend on the inputs.
tuple<uint64_t, uint64_t> Mul64(
    const uint64_t& x,
    const uint64_t& y
)
{
    uint128_t result = static_cast<uint128_t>(x) * static_cast<uint128_t>(y);
    uint64_t lo = result;
    uint64_t hi = result >> 64;
    return {hi, lo};
}

tuple<uint64_t, uint64_t, uint64_t> madd(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& t,
    const uint64_t& u,
    const uint64_t& v
)
{
    uint64_t hi, lo, carry, _v, _u, _t, _;
    tie(hi, lo) = Mul64(a, b);
    tie(_v, carry) = Add64(lo, v, 0);
    tie(_u, carry) = Add64(hi, u, carry);
    tie(_t, _) = Add64(t, 0, carry);
    return {_t, _u, _v};
}

// madd0 hi = a*b + c (discards lo bits)
uint64_t madd0(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c
)
{
    uint64_t carry, hi, lo, _;
    tie(hi, lo) = Mul64(a, b);
    tie(_, carry) = Add64(lo, c, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    return hi;
}

// madd1 hi, lo = a*b + c
tuple<uint64_t, uint64_t> madd1(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c
)
{
    uint64_t carry, hi, lo, _;
    tie(hi, lo) = Mul64(a, b);
    tie(lo, carry) = Add64(lo, c, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    return {hi, lo};
}

// madd2 hi, lo = a*b + c + d
tuple<uint64_t, uint64_t> madd2(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c,
    const uint64_t& d
)
{
    uint64_t carry, hi, lo, _c, _;
    tie(hi, lo) = Mul64(a, b);
    tie(_c, carry) = Add64(c, d, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    tie(lo, carry) = Add64(lo, _c, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    return {hi, lo};
}

// madd2s superhi, hi, lo = 2*a*b + c + d + e
tuple<uint64_t, uint64_t, uint64_t> madd2s(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c,
    const uint64_t& d,
    const uint64_t& e
)
{
    uint64_t carry, sum, superhi, hi, lo, _;

    tie(hi, lo) = Mul64(a, b);
    tie(lo, carry) = Add64(lo, lo, 0);
    tie(hi, superhi) = Add64(hi, hi, carry);

    tie(sum, carry) = Add64(c, e, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    tie(lo, carry) = Add64(lo, sum, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    tie(hi, _) = Add64(hi, 0, d);
    return {superhi, hi, lo};
}

tuple<uint64_t, uint64_t, uint64_t> madd1s(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& d,
    const uint64_t& e
)
{
    uint64_t carry, superhi, hi, lo, _;

    tie(hi, lo) = Mul64(a, b);
    tie(lo, carry) = Add64(lo, lo, 0);
    tie(hi, superhi) = Add64(hi, hi, carry);
    tie(lo, carry) = Add64(lo, e, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    tie(hi, _) = Add64(hi, 0, d);
    return {superhi, hi, lo};
}

tuple<uint64_t, uint64_t, uint64_t> madd2sb(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c,
    const uint64_t& e
)
{
    uint64_t carry, sum, superhi, hi, lo, _;

    tie(hi, lo) = Mul64(a, b);
    tie(lo, carry) = Add64(lo, lo, 0);
    tie(hi, superhi) = Add64(hi, hi, carry);

    tie(sum, carry) = Add64(c, e, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    tie(lo, carry) = Add64(lo, sum, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    return {superhi, hi, lo};
}

tuple<uint64_t, uint64_t, uint64_t> madd1sb(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& e
)
{
    uint64_t carry, superhi, hi, lo, _;

    tie(hi, lo) = Mul64(a, b);
    tie(lo, carry) = Add64(lo, lo, 0);
    tie(hi, superhi) = Add64(hi, hi, carry);
    tie(lo, carry) = Add64(lo, e, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    return {superhi, hi, lo};
}

tuple<uint64_t, uint64_t> madd3(
    const uint64_t& a,
    const uint64_t& b,
    const uint64_t& c,
    const uint64_t& d,
    const uint64_t& e
)
{
    uint64_t carry, hi, lo, _c, _;
    tie(hi, lo) = Mul64(a, b);
    tie(_c, carry) = Add64(c, d, 0);
    tie(hi, _) = Add64(hi, 0, carry);
    tie(lo, carry) = Add64(lo, _c, 0);
    tie(hi, _) = Add64(hi, e, carry);
    return {hi, lo};
}

} // namespace bls12_381
