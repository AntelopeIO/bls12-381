.file	"arithmetic.s"
.text

# void bls12_381::_add(fp* z, const fp* x, const fp* y)
.globl _ZN9bls12_3814_addEPNS_2fpEPKS0_S3_
.type _ZN9bls12_3814_addEPNS_2fpEPKS0_S3_, @function
_ZN9bls12_3814_addEPNS_2fpEPKS0_S3_:
   push %r15
   push %r14
   push %r13
   push %r12
   push %rbx
   mov     (%rsi),%r8
   mov 0x08(%rsi),%r9
   mov 0x10(%rsi),%r10
   mov 0x18(%rsi),%r11
   mov 0x20(%rsi),%r12
   mov 0x28(%rsi),%r13
   add     (%rdx),%r8
   adc 0x08(%rdx),%r9
   adc 0x10(%rdx),%r10
   adc 0x18(%rdx),%r11
   adc 0x20(%rdx),%r12
   adc 0x28(%rdx),%r13
   mov %r8,%r14
   mov %r9,%r15
   mov %r10,%rcx
   mov %r11,%rdx
   mov %r12,%rsi
   mov %r13,%rbx
   mov $0xb9feffffffffaaab,%rax
   sub %rax,%r14
   mov $0x1eabfffeb153ffff,%rax
   sbb %rax,%r15
   mov $0x6730d2a0f6b0f624,%rax
   sbb %rax,%rcx
   mov $0x64774b84f38512bf,%rax
   sbb %rax,%rdx
   mov $0x4b1ba7b6434bacd7,%rax
   sbb %rax,%rsi
   mov $0x1a0111ea397fe69a,%rax
   sbb %rax,%rbx
   cmovae %r14,%r8
   cmovae %r15,%r9
   cmovae %rcx,%r10
   cmovae %rdx,%r11
   cmovae %rsi,%r12
   cmovae %rbx,%r13
   mov %r8,     (%rdi)
   mov %r9, 0x08(%rdi)
   mov %r10,0x10(%rdi)
   mov %r11,0x18(%rdi)
   mov %r12,0x20(%rdi)
   mov %r13,0x28(%rdi)
   pop %rbx
   pop %r12
   pop %r13
   pop %r14
   pop %r15
   retq
.size	_ZN9bls12_3814_addEPNS_2fpEPKS0_S3_, .-_ZN9bls12_3814_addEPNS_2fpEPKS0_S3_

# void bls12_381::_ladd(fp* z, const fp* x, const fp* y)
.globl _ZN9bls12_3815_laddEPNS_2fpEPKS0_S3_
.type _ZN9bls12_3815_laddEPNS_2fpEPKS0_S3_, @function
_ZN9bls12_3815_laddEPNS_2fpEPKS0_S3_:
   mov     (%rsi),%r8
   mov 0x08(%rsi),%r9
   mov 0x10(%rsi),%r10
   mov 0x18(%rsi),%r11
   mov 0x20(%rsi),%rcx
   mov 0x28(%rsi),%rax
   add     (%rdx),%r8
   adc 0x08(%rdx),%r9
   adc 0x10(%rdx),%r10
   adc 0x18(%rdx),%r11
   adc 0x20(%rdx),%rcx
   adc 0x28(%rdx),%rax
   mov %r8,     (%rdi)
   mov %r9, 0x08(%rdi)
   mov %r10,0x10(%rdi)
   mov %r11,0x18(%rdi)
   mov %rcx,0x20(%rdi)
   mov %rax,0x28(%rdi)
   retq
.size	_ZN9bls12_3815_laddEPNS_2fpEPKS0_S3_, .-_ZN9bls12_3815_laddEPNS_2fpEPKS0_S3_

# void bls12_381::_double(fp* z, const fp* x)
.globl _ZN9bls12_3817_doubleEPNS_2fpEPKS0_
.type _ZN9bls12_3817_doubleEPNS_2fpEPKS0_, @function
_ZN9bls12_3817_doubleEPNS_2fpEPKS0_:
   push %r15
   push %r14
   push %r13
   push %r12
   push %rbx
   mov     (%rsi),%r8
   mov 0x08(%rsi),%r9
   mov 0x10(%rsi),%r10
   mov 0x18(%rsi),%r11
   mov 0x20(%rsi),%r12
   mov 0x28(%rsi),%r13
   add %r8,%r8
   adc %r9,%r9
   adc %r10,%r10
   adc %r11,%r11
   adc %r12,%r12
   adc %r13,%r13
   mov %r8,%r14
   mov %r9,%r15
   mov %r10,%rcx
   mov %r11,%rdx
   mov %r12,%rsi
   mov %r13,%rbx
   mov $0xb9feffffffffaaab,%rax
   sub %rax,%r14
   mov $0x1eabfffeb153ffff,%rax
   sbb %rax,%r15
   mov $0x6730d2a0f6b0f624,%rax
   sbb %rax,%rcx
   mov $0x64774b84f38512bf,%rax
   sbb %rax,%rdx
   mov $0x4b1ba7b6434bacd7,%rax
   sbb %rax,%rsi
   mov $0x1a0111ea397fe69a,%rax
   sbb %rax,%rbx
   cmovae %r14,%r8
   cmovae %r15,%r9
   cmovae %rcx,%r10
   cmovae %rdx,%r11
   cmovae %rsi,%r12
   cmovae %rbx,%r13
   mov %r8,     (%rdi)
   mov %r9, 0x08(%rdi)
   mov %r10,0x10(%rdi)
   mov %r11,0x18(%rdi)
   mov %r12,0x20(%rdi)
   mov %r13,0x28(%rdi)
   pop %rbx
   pop %r12
   pop %r13
   pop %r14
   pop %r15
   retq
.size	_ZN9bls12_3817_doubleEPNS_2fpEPKS0_, .-_ZN9bls12_3817_doubleEPNS_2fpEPKS0_

# void bls12_381::_ldouble(fp* z, const fp* x)
.globl _ZN9bls12_3818_ldoubleEPNS_2fpEPKS0_
.type _ZN9bls12_3818_ldoubleEPNS_2fpEPKS0_, @function
_ZN9bls12_3818_ldoubleEPNS_2fpEPKS0_:
   mov     (%rsi),%r8
   mov 0x08(%rsi),%r9
   mov 0x10(%rsi),%r10
   mov 0x18(%rsi),%r11
   mov 0x20(%rsi),%rcx
   mov 0x28(%rsi),%rax
   add %r8,%r8
   adc %r9,%r9
   adc %r10,%r10
   adc %r11,%r11
   adc %rcx,%rcx
   adc %rax,%rax
   mov %r8,     (%rdi)
   mov %r9, 0x08(%rdi)
   mov %r10,0x10(%rdi)
   mov %r11,0x18(%rdi)
   mov %rcx,0x20(%rdi)
   mov %rax,0x28(%rdi)
   retq
.size	_ZN9bls12_3818_ldoubleEPNS_2fpEPKS0_, .-_ZN9bls12_3818_ldoubleEPNS_2fpEPKS0_

# void bls12_381::_subtract(fp* z, const fp* x, const fp* y)
.globl _ZN9bls12_3819_subtractEPNS_2fpEPKS0_S3_
.type _ZN9bls12_3819_subtractEPNS_2fpEPKS0_S3_, @function
_ZN9bls12_3819_subtractEPNS_2fpEPKS0_S3_:
   push %r15
   push %r14
   push %r13
   push %r12
   push %rbx
   xor %rax,%rax
   mov     (%rsi),%r8
   mov 0x08(%rsi),%r9
   mov 0x10(%rsi),%r10
   mov 0x18(%rsi),%r11
   mov 0x20(%rsi),%r12
   mov 0x28(%rsi),%r13
   sub     (%rdx),%r8
   sbb 0x08(%rdx),%r9
   sbb 0x10(%rdx),%r10
   sbb 0x18(%rdx),%r11
   sbb 0x20(%rdx),%r12
   sbb 0x28(%rdx),%r13
   mov $0xb9feffffffffaaab,%r14
   mov $0x1eabfffeb153ffff,%r15
   mov $0x6730d2a0f6b0f624,%rcx
   mov $0x64774b84f38512bf,%rdx
   mov $0x4b1ba7b6434bacd7,%rsi
   mov $0x1a0111ea397fe69a,%rbx
   cmovae %rax,%r14
   cmovae %rax,%r15
   cmovae %rax,%rcx
   cmovae %rax,%rdx
   cmovae %rax,%rsi
   cmovae %rax,%rbx
   add %r14,%r8
   adc %r15,%r9
   adc %rcx,%r10
   adc %rdx,%r11
   adc %rsi,%r12
   adc %rbx,%r13
   mov %r8,     (%rdi)
   mov %r9, 0x08(%rdi)
   mov %r10,0x10(%rdi)
   mov %r11,0x18(%rdi)
   mov %r12,0x20(%rdi)
   mov %r13,0x28(%rdi)
   pop %rbx
   pop %r12
   pop %r13
   pop %r14
   pop %r15
   retq
.size	_ZN9bls12_3819_subtractEPNS_2fpEPKS0_S3_, .-_ZN9bls12_3819_subtractEPNS_2fpEPKS0_S3_

# void bls12_381::_lsubtract(fp* z, const fp* x, const fp* y)
.globl _ZN9bls12_38110_lsubtractEPNS_2fpEPKS0_S3_
.type _ZN9bls12_38110_lsubtractEPNS_2fpEPKS0_S3_, @function
_ZN9bls12_38110_lsubtractEPNS_2fpEPKS0_S3_:
   mov     (%rsi),%r8
   mov 0x08(%rsi),%r9
   mov 0x10(%rsi),%r10
   mov 0x18(%rsi),%r11
   mov 0x20(%rsi),%rcx
   mov 0x28(%rsi),%rax
   sub     (%rdx),%r8
   sbb 0x08(%rdx),%r9
   sbb 0x10(%rdx),%r10
   sbb 0x18(%rdx),%r11
   sbb 0x20(%rdx),%rcx
   sbb 0x28(%rdx),%rax
   mov %r8,     (%rdi)
   mov %r9, 0x08(%rdi)
   mov %r10,0x10(%rdi)
   mov %r11,0x18(%rdi)
   mov %rcx,0x20(%rdi)
   mov %rax,0x28(%rdi)
   retq
.size	_ZN9bls12_38110_lsubtractEPNS_2fpEPKS0_S3_, .-_ZN9bls12_38110_lsubtractEPNS_2fpEPKS0_S3_

# void bls12_381::__negate(fp* z, const fp* x)
.globl _ZN9bls12_3818__negateEPNS_2fpEPKS0_
.type _ZN9bls12_3818__negateEPNS_2fpEPKS0_, @function
_ZN9bls12_3818__negateEPNS_2fpEPKS0_:
   mov $0xb9feffffffffaaab,%r8
   mov $0x1eabfffeb153ffff,%r9
   mov $0x6730d2a0f6b0f624,%r10
   mov $0x64774b84f38512bf,%r11
   mov $0x4b1ba7b6434bacd7,%rcx
   mov $0x1a0111ea397fe69a,%rax
   sub     (%rsi),%r8
   sbb 0x08(%rsi),%r9
   sbb 0x10(%rsi),%r10
   sbb 0x18(%rsi),%r11
   sbb 0x20(%rsi),%rcx
   sbb 0x28(%rsi),%rax
   mov %r8,     (%rdi)
   mov %r9, 0x08(%rdi)
   mov %r10,0x10(%rdi)
   mov %r11,0x18(%rdi)
   mov %rcx,0x20(%rdi)
   mov %rax,0x28(%rdi)
   retq
.size	_ZN9bls12_3818__negateEPNS_2fpEPKS0_, .-_ZN9bls12_3818__negateEPNS_2fpEPKS0_

# void bls12_381::__multiply(fp* z, const fp* x, const fp* y)
.globl _ZN9bls12_38110__multiplyEPNS_2fpEPKS0_S3_
.type _ZN9bls12_38110__multiplyEPNS_2fpEPKS0_S3_, @function
_ZN9bls12_38110__multiplyEPNS_2fpEPKS0_S3_:
   push %rbp
   push %r15
   push %r14
   push %r13
   push %r12
   push %rbx

   # this pushes the constants (INP and MODULUS) used in this function to the stack
   # TODO: should be referenced from data section instead
   mov $0x89f3fffcfffcfffd,%rax
   push %rax
   mov %rsp,%rbp
   mov $0xb9feffffffffaaab,%rax
   push %rax
   mov $0x1eabfffeb153ffff,%rax
   push %rax
   mov $0x6730d2a0f6b0f624,%rax
   push %rax
   mov $0x64774b84f38512bf,%rax
   push %rax
   mov $0x4b1ba7b6434bacd7,%rax
   push %rax
   mov $0x1a0111ea397fe69a,%rax
   push %rax

   mov $0,%r9
   mov $0,%r10
   mov $0,%r11
   mov $0,%r12
   mov $0,%r13
   mov $0,%r14
   mov $0,%r15

   push %rdi                  # save z for later
   mov %rdx,%rdi              # move y to %rdi to free up %rdx for use in mul
   # i0
   mov     (%rsi),%rcx
   mov     (%rdi),%rax
   mul %rcx
   push %rax
   mov %rdx,%r8
   mov 0x08(%rdi),%rax
   mul %rcx
   add %rax,%r8
   adc %rdx,%r9
   mov 0x10(%rdi),%rax
   mul %rcx
   add %rax,%r9
   adc %rdx,%r10
   mov 0x18(%rdi),%rax
   mul %rcx
   add %rax,%r10
   adc %rdx,%r11
   mov 0x20(%rdi),%rax
   mul %rcx
   add %rax,%r11
   adc %rdx,%r12
   mov 0x28(%rdi),%rax
   mul %rcx
   add %rax,%r12
   adc %rdx,%r13
   # i1
   mov 0x08(%rsi),%rcx
   mov $0,%rbx
   mov     (%rdi),%rax
   mul %rcx
   add %rax,%r8
   adc %rdx,%r9
   adc $0,%r10
   adc $0,%rbx
   push %r8
   mov $0,%r8
   mov 0x08(%rdi),%rax
   mul %rcx
   add %rax,%r9
   adc %rdx,%r10
   adc %rbx,%r11
   mov $0,%rbx
   adc $0,%rbx
   mov 0x10(%rdi),%rax
   mul %rcx
   add %rax,%r10
   adc %rdx,%r11
   adc %rbx,%r12
   mov $0,%rbx
   adc $0,%rbx
   mov 0x18(%rdi),%rax
   mul %rcx
   add %rax,%r11
   adc %rdx,%r12
   adc %rbx,%r13
   mov $0,%rbx
   adc $0,%rbx
   mov 0x20(%rdi),%rax
   mul %rcx
   add %rax,%r12
   adc %rdx,%r13
   adc %rbx,%r14
   mov 0x28(%rdi),%rax
   mul %rcx
   add %rax,%r13
   adc %rdx,%r14
   # i2
   mov 0x10(%rsi),%rcx
   mov $0,%rbx
   mov     (%rdi),%rax
   mul %rcx
   add %rax,%r9
   adc %rdx,%r10
   adc $0,%r11
   adc $0,%rbx
   push %r9
   mov $0,%r9
   mov 0x08(%rdi),%rax
   mul %rcx
   add %rax,%r10
   adc %rdx,%r11
   adc %rbx,%r12
   mov $0,%rbx
   adc $0,%rbx
   mov 0x10(%rdi),%rax
   mul %rcx
   add %rax,%r11
   adc %rdx,%r12
   adc %rbx,%r13
   mov $0,%rbx
   adc $0,%rbx
   mov 0x18(%rdi),%rax
   mul %rcx
   add %rax,%r12
   adc %rdx,%r13
   adc %rbx,%r14
   mov $0,%rbx
   adc $0,%rbx
   mov 0x20(%rdi),%rax
   mul %rcx
   add %rax,%r13
   adc %rdx,%r14
   adc %rbx,%r15
   mov 0x28(%rdi),%rax
   mul %rcx
   add %rax,%r14
   adc %rdx,%r15
   # i3
   mov 0x18(%rsi),%rcx
   mov $0,%rbx
   mov     (%rdi),%rax
   mul %rcx
   add %rax,%r10
   adc %rdx,%r11
   adc $0,%r12
   adc $0,%rbx
   mov 0x08(%rdi),%rax
   mul %rcx
   add %rax,%r11
   adc %rdx,%r12
   adc %rbx,%r13
   mov $0,%rbx
   adc $0,%rbx
   mov 0x10(%rdi),%rax
   mul %rcx
   add %rax,%r12
   adc %rdx,%r13
   adc %rbx,%r14
   mov $0,%rbx
   adc $0,%rbx
   mov 0x18(%rdi),%rax
   mul %rcx
   add %rax,%r13
   adc %rdx,%r14
   adc %rbx,%r15
   mov $0,%rbx
   adc $0,%rbx
   mov 0x20(%rdi),%rax
   mul %rcx
   add %rax,%r14
   adc %rdx,%r15
   adc %rbx,%r8
   mov 0x28(%rdi),%rax
   mul %rcx
   add %rax,%r15
   adc %rdx,%r8
   # i4
   mov 0x20(%rsi),%rcx
   mov $0,%rbx
   mov     (%rdi),%rax
   mul %rcx
   add %rax,%r11
   adc %rdx,%r12
   adc $0,%r13
   adc $0,%rbx
   mov 0x08(%rdi),%rax
   mul %rcx
   add %rax,%r12
   adc %rdx,%r13
   adc %rbx,%r14
   mov $0,%rbx
   adc $0,%rbx
   mov 0x10(%rdi),%rax
   mul %rcx
   add %rax,%r13
   adc %rdx,%r14
   adc %rbx,%r15
   mov $0,%rbx
   adc $0,%rbx
   mov 0x18(%rdi),%rax
   mul %rcx
   add %rax,%r14
   adc %rdx,%r15
   adc %rbx,%r8
   mov $0,%rbx
   adc $0,%rbx
   mov 0x20(%rdi),%rax
   mul %rcx
   add %rax,%r15
   adc %rdx,%r8
   adc %rbx,%r9
   mov 0x28(%rdi),%rax
   mul %rcx
   add %rax,%r8
   adc %rdx,%r9
   # i5
   mov 0x28(%rsi),%rcx
   mov $0,%rbx
   mov     (%rdi),%rax
   mul %rcx
   add %rax,%r12
   adc %rdx,%r13
   adc $0,%r14
   adc $0,%rbx
   mov 0x08(%rdi),%rax
   mul %rcx
   add %rax,%r13
   adc %rdx,%r14
   adc %rbx,%r15
   mov $0,%rbx
   adc $0,%rbx
   mov 0x10(%rdi),%rax
   mul %rcx
   add %rax,%r14
   adc %rdx,%r15
   adc %rbx,%r8
   mov $0,%rbx
   adc $0,%rbx
   mov 0x18(%rdi),%rax
   mul %rcx
   add %rax,%r15
   adc %rdx,%r8
   adc %rbx,%r9
   mov $0,%rbx
   adc $0,%rbx
   mov 0x20(%rdi),%rax
   mul %rcx
   add %rax,%r8
   adc %rdx,%r9
   adc $0,%rbx
   mov 0x28(%rdi),%rax
   mul %rcx
   add %rax,%r9
   adc %rdx,%rbx

   pop %rsi
   pop %rdi
   pop %rcx
   push %rbx
   push %r9

   # montgomery reduction
   # i0
   mov %rcx,%rax
   mulq (%rbp)
   mov %rax,%r9
   mov $0,%rbx
   # j0
   mov -0x08(%rbp),%rax
   mul %r9
   add %rax,%rcx
   adc %rdx,%rbx
   # j1
   mov -0x10(%rbp),%rax
   mul %r9
   add %rax,%rdi
   adc $0,%rdx
   add %rbx,%rdi
   mov $0,%rbx
   adc %rdx,%rbx
   # j2
   mov -0x18(%rbp),%rax
   mul %r9
   add %rax,%rsi
   adc $0,%rdx
   add %rbx,%rsi
   mov $0,%rbx
   adc %rdx,%rbx
   # j3
   mov -0x20(%rbp),%rax
   mul %r9
   add %rax,%r10
   adc $0,%rdx
   add %rbx,%r10
   mov $0,%rbx
   adc %rdx,%rbx
   # j4
   mov -0x28(%rbp),%rax
   mul %r9
   add %rax,%r11
   adc $0,%rdx
   add %rbx,%r11
   mov $0,%rbx
   adc %rdx,%rbx
   # j5
   mov -0x30(%rbp),%rax
   mul %r9
   add %rax,%r12
   adc $0,%rdx
   add %rbx,%r12
   adc %rdx,%r13
   adc $0,%rcx
   # i1
   mov %rdi,%rax
   mulq (%rbp)
   mov %rax,%r9
   mov $0,%rbx
   # j0
   mov -0x08(%rbp),%rax
   mul %r9
   add %rax,%rdi
   adc %rdx,%rbx
   # j1
   mov -0x10(%rbp),%rax
   mul %r9
   add %rax,%rsi
   adc $0,%rdx
   add %rbx,%rsi
   mov $0,%rbx
   adc %rdx,%rbx
   # j2
   mov -0x18(%rbp),%rax
   mul %r9
   add %rax,%r10
   adc $0,%rdx
   add %rbx,%r10
   mov $0,%rbx
   adc %rdx,%rbx
   # j3
   mov -0x20(%rbp),%rax
   mul %r9
   add %rax,%r11
   adc $0,%rdx
   add %rbx,%r11
   mov $0,%rbx
   adc %rdx,%rbx
   # j4
   mov -0x28(%rbp),%rax
   mul %r9
   add %rax,%r12
   adc $0,%rdx
   add %rbx,%r12
   mov $0,%rbx
   adc %rdx,%rbx
   # j5
   mov -0x30(%rbp),%rax
   mul %r9
   add %rax,%r13
   adc %rdx,%rcx
   add %rbx,%r13
   adc %rcx,%r14
   mov $0,%rcx
   adc $0,%rcx
   # i2
   mov %rsi,%rax
   mulq (%rbp)
   mov %rax,%r9
   mov $0,%rbx
   # j0
   mov -0x08(%rbp),%rax
   mul %r9
   add %rax,%rsi
   adc %rdx,%rbx
   # j1
   mov -0x10(%rbp),%rax
   mul %r9
   add %rax,%r10
   adc $0,%rdx
   add %rbx,%r10
   mov $0,%rbx
   adc %rdx,%rbx
   # j2
   mov -0x18(%rbp),%rax
   mul %r9
   add %rax,%r11
   adc $0,%rdx
   add %rbx,%r11
   mov $0,%rbx
   adc %rdx,%rbx
   # j3
   mov -0x20(%rbp),%rax
   mul %r9
   add %rax,%r12
   adc $0,%rdx
   add %rbx,%r12
   mov $0,%rbx
   adc %rdx,%rbx
   # j4
   mov -0x28(%rbp),%rax
   mul %r9
   add %rax,%r13
   adc $0,%rdx
   add %rbx,%r13
   mov $0,%rbx
   adc %rdx,%rbx
   # j5
   mov -0x30(%rbp),%rax
   mul %r9
   add %rax,%r14
   adc %rdx,%rcx
   add %rbx,%r14
   adc %rcx,%r15
   mov $0,%rcx
   adc $0,%rcx
   # i3
   mov %r10,%rax
   mulq (%rbp)
   mov %rax,%r9
   mov $0,%rbx
   # j0
   mov -0x08(%rbp),%rax
   mul %r9
   add %rax,%r10
   adc %rdx,%rbx
   # j1
   mov -0x10(%rbp),%rax
   mul %r9
   add %rax,%r11
   adc $0,%rdx
   add %rbx,%r11
   mov $0,%rbx
   adc %rdx,%rbx
   # j2
   mov -0x18(%rbp),%rax
   mul %r9
   add %rax,%r12
   adc $0,%rdx
   add %rbx,%r12
   mov $0,%rbx
   adc %rdx,%rbx
   # j3
   mov -0x20(%rbp),%rax
   mul %r9
   add %rax,%r13
   adc $0,%rdx
   add %rbx,%r13
   mov $0,%rbx
   adc %rdx,%rbx
   # j4
   mov -0x28(%rbp),%rax
   mul %r9
   add %rax,%r14
   adc $0,%rdx
   add %rbx,%r14
   mov $0,%rbx
   adc %rdx,%rbx
   # j5
   mov -0x30(%rbp),%rax
   mul %r9
   add %rax,%r15
   adc %rdx,%rcx
   add %rbx,%r15
   adc %rcx,%r8
   mov $0,%rcx
   adc $0,%rcx
   # i4
   mov %r11,%rax
   mulq (%rbp)
   mov %rax,%r9
   mov $0,%rbx
   # j0
   mov -0x08(%rbp),%rax
   mul %r9
   add %rax,%r11
   adc %rdx,%rbx
   # j1
   mov -0x10(%rbp),%rax
   mul %r9
   add %rax,%r12
   adc $0,%rdx
   add %rbx,%r12
   mov $0,%rbx
   adc %rdx,%rbx
   # j2
   mov -0x18(%rbp),%rax
   mul %r9
   add %rax,%r13
   adc $0,%rdx
   add %rbx,%r13
   mov $0,%rbx
   adc %rdx,%rbx
   # j3
   mov -0x20(%rbp),%rax
   mul %r9
   add %rax,%r14
   adc $0,%rdx
   add %rbx,%r14
   mov $0,%rbx
   adc %rdx,%rbx
   # j4
   mov -0x28(%rbp),%rax
   mul %r9
   add %rax,%r15
   adc $0,%rdx
   add %rbx,%r15
   mov $0,%rbx
   adc %rdx,%rbx
   # j5
   mov -0x30(%rbp),%rax
   mul %r9
   add %rax,%r8
   adc %rdx,%rcx
   add %rbx,%r8
   pop %rdi
   adc %rcx,%rdi
   mov $0,%rcx
   adc $0,%rcx
   # i5
   mov %r12,%rax
   mulq (%rbp)
   mov %rax,%r9
   mov $0,%rbx
   # j0
   mov -0x08(%rbp),%rax
   mul %r9
   add %rax,%r12
   adc %rdx,%rbx
   # j1
   mov -0x10(%rbp),%rax
   mul %r9
   add %rax,%r13
   adc $0,%rdx
   add %rbx,%r13
   mov $0,%rbx
   adc %rdx,%rbx
   # j2
   mov -0x18(%rbp),%rax
   mul %r9
   add %rax,%r14
   adc $0,%rdx
   add %rbx,%r14
   mov $0,%rbx
   adc %rdx,%rbx
   # j3
   mov -0x20(%rbp),%rax
   mul %r9
   add %rax,%r15
   adc $0,%rdx
   add %rbx,%r15
   mov $0,%rbx
   adc %rdx,%rbx
   # j4
   mov -0x28(%rbp),%rax
   mul %r9
   add %rax,%r8
   adc $0,%rdx
   add %rbx,%r8
   mov $0,%rbx
   adc %rdx,%rbx
   # j5
   mov -0x30(%rbp),%rax
   mul %r9
   add %rax,%rdi
   adc %rdx,%rcx
   add %rbx,%rdi
   pop %rsi
   adc %rsi,%rcx

   # modular reduction
   mov %r13,%r10
   sub -0x08(%rbp),%r10
   mov %r14,%r11
   sbb -0x10(%rbp),%r11
   mov %r15,%r12
   sbb -0x18(%rbp),%r12
   mov %r8,%rax
   sbb -0x20(%rbp),%rax
   mov %rdi,%rbx
   sbb -0x28(%rbp),%rbx
   mov %rcx,%r9
   sbb -0x30(%rbp),%r9

   # out
   pop %rsi
   cmovae %r10,%r13
   mov %r13,    (%rsi)
   cmovae %r11,%r14
   mov %r14,0x08(%rsi)
   cmovae %r12,%r15
   mov %r15,0x10(%rsi)
   cmovae %rax,%r8
   mov %r8,0x18(%rsi)
   cmovae %rbx,%rdi
   mov %rdi,0x20(%rsi)
   cmovae %r9,%rcx
   mov %rcx,0x28(%rsi)

   pop %rax
   pop %rax
   pop %rax
   pop %rax
   pop %rax
   pop %rax
   pop %rax

   pop %rbx
   pop %r12
   pop %r13
   pop %r14
   pop %r15
   pop %rbp
   retq
.size	_ZN9bls12_38110__multiplyEPNS_2fpEPKS0_S3_, .-_ZN9bls12_38110__multiplyEPNS_2fpEPKS0_S3_

# void bls12_381::__mul_ex(fp* z, const fp* x, const fp* y)
.globl _ZN9bls12_3818__mul_exEPNS_2fpEPKS0_S3_
.type _ZN9bls12_3818__mul_exEPNS_2fpEPKS0_S3_, @function
_ZN9bls12_3818__mul_exEPNS_2fpEPKS0_S3_:
   push %rbp
   push %r15
   push %r14
   push %r13
   push %r12
   push %rbx

   # this pushes the constants (INP and MODULUS) used in this function to the stack
   # TODO: should be referenced from data section instead
   mov $0x89f3fffcfffcfffd,%rax
   push %rax
   mov %rsp,%rbp
   mov $0xb9feffffffffaaab,%rax
   push %rax
   mov $0x1eabfffeb153ffff,%rax
   push %rax
   mov $0x6730d2a0f6b0f624,%rax
   push %rax
   mov $0x64774b84f38512bf,%rax
   push %rax
   mov $0x4b1ba7b6434bacd7,%rax
   push %rax
   mov $0x1a0111ea397fe69a,%rax
   push %rax

   push %rdi                  # save z for later
   mov %rdx,%rdi              # move y to %rdi to free up %rdx for use in mulx
   xor %rax,%rax
   # i0
   mov      (%rsi),%rdx       # x0
   mulx     (%rdi),%rax,%rcx  # x0 * y0
   push %rax
   mulx 0x08(%rdi),%rax,%r8   # x0 * y1
   adcx %rax,%rcx
   mulx 0x10(%rdi),%rax,%r9   # x0 * y2
   adcx %rax,%r8
   mulx 0x18(%rdi),%rax,%r10  # x0 * y3
   adcx %rax,%r9
   mulx 0x20(%rdi),%rax,%r11  # x0 * y4
   adcx %rax,%r10
   mulx 0x28(%rdi),%rax,%r12  # x0 * y5
   adcx %rax,%r11
   adc  $0,%r12
   # i1
   mov 0x08(%rsi),%rdx        # x1
   xor %r13,%r13
   mulx     (%rdi),%rax,%rbx  # x1 * y0
   adox %rax,%rcx
   adcx %rbx,%r8
   push %rcx
   mulx 0x08(%rdi),%rax,%rbx  # x1 * y1
   adox %rax,%r8
   adcx %rbx,%r9
   mulx 0x10(%rdi),%rax,%rbx  # x1 * y2
   adox %rax,%r9
   adcx %rbx,%r10
   mulx 0x18(%rdi),%rax,%rbx  # x1 * y3
   adox %rax,%r10
   adcx %rbx,%r11
   mulx 0x20(%rdi),%rax,%rbx  # x1 * y4
   adox %rax,%r11
   adcx %rbx,%r12
   mulx 0x28(%rdi),%rax,%rbx  # x1 * y5
   adox %rax,%r12
   adox %r13,%r13
   adcx %rbx,%r13
   # i2
   mov 0x10(%rsi),%rdx        # x2
   xor %r14,%r14
   mulx     (%rdi),%rax,%rbx  # x2 * y0
   adox %rax,%r8
   adcx %rbx,%r9
   mulx 0x08(%rdi),%rax,%rbx  # x2 * y1
   adox %rax,%r9
   adcx %rbx,%r10
   mulx 0x10(%rdi),%rax,%rbx  # x2 * y2
   adox %rax,%r10
   adcx %rbx,%r11
   mulx 0x18(%rdi),%rax,%rbx  # x2 * y3
   adox %rax,%r11
   adcx %rbx,%r12
   mulx 0x20(%rdi),%rax,%rbx  # x2 * y4
   adox %rax,%r12
   adcx %rbx,%r13
   mulx 0x28(%rdi),%rax,%rbx  # x2 * y5
   adox %rax,%r13
   adox %r14,%r14
   adcx %rbx,%r14
   # i3
   mov 0x18(%rsi),%rdx        # x3
   xor %r15,%r15
   mulx     (%rdi),%rax,%rbx  # x3 * y0
   adox %rax,%r9
   adcx %rbx,%r10
   mulx 0x08(%rdi),%rax,%rbx  # x3 * y1
   adox %rax,%r10
   adcx %rbx,%r11
   mulx 0x10(%rdi),%rax,%rbx  # x3 * y2
   adox %rax,%r11
   adcx %rbx,%r12
   mulx 0x18(%rdi),%rax,%rbx  # x3 * y3
   adox %rax,%r12
   adcx %rbx,%r13
   mulx 0x20(%rdi),%rax,%rbx  # x3 * y4
   adox %rax,%r13
   adcx %rbx,%r14
   mulx 0x28(%rdi),%rax,%rbx  # x3 * y5
   adox %rax,%r14
   adox %r15,%r15
   adcx %rbx,%r15
   # i4
   mov 0x20(%rsi),%rdx        # x4
   xor %rcx,%rcx
   mulx     (%rdi),%rax,%rbx  # x4 * y0
   adox %rax,%r10
   adcx %rbx,%r11
   mulx 0x08(%rdi),%rax,%rbx  # x4 * y1
   adox %rax,%r11
   adcx %rbx,%r12
   mulx 0x10(%rdi),%rax,%rbx  # x4 * y2
   adox %rax,%r12
   adcx %rbx,%r13
   mulx 0x18(%rdi),%rax,%rbx  # x4 * y3
   adox %rax,%r13
   adcx %rbx,%r14
   mulx 0x20(%rdi),%rax,%rbx  # x4 * y4
   adox %rax,%r14
   adcx %rbx,%r15
   mulx 0x28(%rdi),%rax,%rbx  # x4 * y5
   adox %rax,%r15
   adox %rcx,%rcx
   adcx %rbx,%rcx
   # i5
   mov 0x28(%rsi),%rdx        # x5
   xor %rsi,%rsi
   mulx     (%rdi),%rax,%rbx  # x5 * y0
   adox %rax,%r11
   adcx %rbx,%r12
   mulx 0x08(%rdi),%rax,%rbx  # x5 * y1
   adox %rax,%r12
   adcx %rbx,%r13
   mulx 0x10(%rdi),%rax,%rbx  # x5 * y2
   adox %rax,%r13
   adcx %rbx,%r14
   mulx 0x18(%rdi),%rax,%rbx  # x5 * y3
   adox %rax,%r14
   adcx %rbx,%r15
   mulx 0x20(%rdi),%rax,%rbx  # x5 * y4
   adox %rax,%r15
   adcx %rbx,%rcx
   mulx 0x28(%rdi),%rax,%rbx  # x5 * y5
   adox %rax,%rcx
   adox %rbx,%rsi
   adc $0,%rsi

   pop %rdi
   pop %rbx
   push %rsi

   # montgomery reduction
   xor %rax,%rax
   # i0
   mov %rbx, %rdx
   mulx      (%rbp),%rdx,%rsi
   mulx -0x08(%rbp),%rax,%rsi
   #asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); # j0
   adox %rax,%rbx
   adcx %rsi,%rdi
   mulx -0x10(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); # j1
   adox %rax,%rdi
   adcx %rsi,%r8
   mulx -0x18(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); # j2
   adox %rax,%r8
   adcx %rsi,%r9
   mulx -0x20(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); # j3
   adox %rax,%r9
   adcx %rsi,%r10
   mulx -0x28(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); # j4
   adox %rax,%r10
   adcx %rsi,%r11
   mulx -0x30(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); # j5
   adox %rax,%r11
   adcx %rsi,%r12
   adox %rbx,%r12
   adcx %rbx,%rbx
   mov $0,%rax
   adox %rax,%rbx
   xor %rax,%rax
   # i1
   mov %rdi,%rdx
   mulx      (%rbp),%rdx,%rsi
   mulx -0x08(%rbp),%rax,%rsi
   #asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); # j0
   adox %rax,%rdi
   adcx %rsi,%r8
   mulx -0x10(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); # j1
   adox %rax,%r8
   adcx %rsi,%r9
   mulx -0x18(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); # j2
   adox %rax,%r9
   adcx %rsi,%r10
   mulx -0x20(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); # j3
   adox %rax,%r10
   adcx %rsi,%r11
   mulx -0x28(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); # j4
   adox %rax,%r11
   adcx %rsi,%r12
   mulx -0x30(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); # j5
   adox %rax,%r12
   adcx %rsi,%r13
   adox %rbx,%r13
   adcx %rdi,%rdi
   mov $0,%rax
   adox %rax,%rdi
   xor %rax,%rax
   # i2
   mov %r8,%rdx
   mulx      (%rbp),%rdx,%rsi
   mulx -0x08(%rbp),%rax,%rsi
   #asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); # j0
   adox %rax,%r8
   adcx %rsi,%r9
   mulx -0x10(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); # j1
   adox %rax,%r9
   adcx %rsi,%r10
   mulx -0x18(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); # j2
   adox %rax,%r10
   adcx %rsi,%r11
   mulx -0x20(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); # j3
   adox %rax,%r11
   adcx %rsi,%r12
   mulx -0x28(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); # j4
   adox %rax,%r12
   adcx %rsi,%r13
   mulx -0x30(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); # j5
   adox %rax,%r13
   adcx %rsi,%r14
   adox %rdi,%r14
   adcx %r8,%r8
   mov $0,%rax
   adox %rax,%r8
   xor %rax,%rax
   # i3
   mov %r9,%rdx
   mulx      (%rbp),%rdx,%rsi
   mulx -0x08(%rbp),%rax,%rsi
   #asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); # j0
   adox %rax,%r9
   adcx %rsi,%r10
   mulx -0x10(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); # j1
   adox %rax,%r10
   adcx %rsi,%r11
   mulx -0x18(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); # j2
   adox %rax,%r11
   adcx %rsi,%r12
   mulx -0x20(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); # j3
   adox %rax,%r12
   adcx %rsi,%r13
   mulx -0x28(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); # j4
   adox %rax,%r13
   adcx %rsi,%r14
   mulx -0x30(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); # j5
   adox %rax,%r14
   adcx %rsi,%r15
   adox %r8,%r15
   adcx %r9,%r9
   mov $0,%rax
   adox %rax,%r9
   xor %rax,%rax
   # i4
   mov %r10,%rdx
   mulx      (%rbp),%rdx,%rsi
   mulx -0x08(%rbp),%rax,%rsi
   #asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); # j0
   adox %rax,%r10
   adcx %rsi,%r11
   mulx -0x10(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); # j1
   adox %rax,%r11
   adcx %rsi,%r12
   mulx -0x18(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); # j2
   adox %rax,%r12
   adcx %rsi,%r13
   mulx -0x20(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); # j3
   adox %rax,%r13
   adcx %rsi,%r14
   mulx -0x28(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); # j4
   adox %rax,%r14
   adcx %rsi,%r15
   mulx -0x30(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); # j5
   adox %rax,%r15
   adcx %rsi,%rcx
   adox %r9,%rcx
   adcx %r10,%r10
   mov $0,%rax
   adox %rax,%r10
   xor %rax,%rax
   # i5
   mov %r11,%rdx
   mulx      (%rbp),%rdx,%rsi
   mulx -0x08(%rbp),%rax,%rsi
   #asm("mulx %0,%%rdx,%%rsi;" : : "m" (fp::INP));
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[0])); # j0
   adox %rax,%r11
   adcx %rsi,%r12
   mulx -0x10(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[1])); # j1
   adox %rax,%r12
   adcx %rsi,%r13
   mulx -0x18(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[2])); # j2
   adox %rax,%r13
   adcx %rsi,%r14
   mulx -0x20(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[3])); # j3
   adox %rax,%r14
   adcx %rsi,%r15
   mulx -0x28(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[4])); # j4
   adox %rax,%r15
   adcx %rsi,%rcx
   mulx -0x30(%rbp),%rax,%rsi
   #asm("mulx %0,%%rax,%%rsi;" : : "m" (fp::MODULUS.d[5])); # j5
   adox %rax,%rcx
   pop %rbx
   adcx %rsi,%rbx
   adox %r10,%rbx
   # modular reduction
   mov %r12,%rax
   sub -0x08(%rbp),%rax
   #asm("sub %0,%%rax;" : : "m" (fp::MODULUS.d[0]));
   mov %r13,%rsi
   sbb -0x10(%rbp),%rsi
   #asm("sbb %0,%%rsi;" : : "m" (fp::MODULUS.d[1]));
   mov %r14,%rdi
   sbb -0x18(%rbp),%rdi
   #asm("sbb %0,%%rdi;" : : "m" (fp::MODULUS.d[2]));
   mov %r15,%r8
   sbb -0x20(%rbp),%r8
   #asm("sbb %0,%%r8;" : : "m" (fp::MODULUS.d[3]));
   mov %rcx,%r9
   sbb -0x28(%rbp),%r9
   #asm("sbb %0,%%r9;" : : "m" (fp::MODULUS.d[4]));
   mov %rbx,%r10
   sbb -0x30(%rbp),%r10
   #asm("sbb %0,%%r10;" : : "m" (fp::MODULUS.d[5]));
   # out
   pop %r11
   cmovae %rax,%r12
   mov %r12,(%r11)
   cmovae %rsi,%r13
   mov %r13,0x8(%r11)
   cmovae %rdi,%r14
   mov %r14,0x10(%r11)
   cmovae %r8,%r15
   mov %r15,0x18(%r11)
   cmovae %r9,%rcx
   mov %rcx,0x20(%r11)
   cmovae %r10,%rbx
   mov %rbx,0x28(%r11)

   pop %rax
   pop %rax
   pop %rax
   pop %rax
   pop %rax
   pop %rax
   pop %rax

   pop %rbx
   pop %r12
   pop %r13
   pop %r14
   pop %r15
   pop %rbp
   retq
.size	_ZN9bls12_3818__mul_exEPNS_2fpEPKS0_S3_, .-_ZN9bls12_3818__mul_exEPNS_2fpEPKS0_S3_

.section	.note.GNU-stack,"",@progbits  # non executable stack
