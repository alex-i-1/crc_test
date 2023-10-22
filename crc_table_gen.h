#pragma once

#include "types.h"


// "warning C4305: 'initializing' : truncation from 'const u32' to 'const bool'"
#pragma warning(disable:4305)
// for CRC-16 (can simply correct 1 line of code instead of disabling this warning)
#pragma warning(disable:4309)

// even though this file is short, it yet needs a map
// 1) template which generates a single entry in the table in the MSBF order
// 2a) a bits reversing template
// 2) template which generates a single entry in the table in the LSBF order
// 3) template of the whole table

// MSVC 6.0 doesn't grasp that (for the 'template <typename T, u64 POLYNOMIAL> struct t_crc_table<T, POLYNOMIAL, 0>' template it says "error C2989: 't_crc_table<T,POLYNOMIAL,0>' : template class has already been defined as a non-template class")
// (there are a templates' bugs in MSVC 6.0 when dealing with integer types in their parameters, and this error possibly is one of them)
// MSVC 2002 (1300) doesn't grasp these tempates too; MSVC 2003 (1310) does
#if (defined(_MSC_VER) && _MSC_VER < 1310)
#error "don't use MSVC below 2003"
#endif
// https://stackoverflow.com/a/18513666
// 'constexpr' requires MSVC 2015
// '__cplusplus' for MSVC 2015 however stays the same 199711
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
//#if __cplusplus >= 201103L
#define CONSTEXPR constexpr
#else
// *not a real replacement for 'constexpr', but just a fake substitute which satisfies this sort of compilers
#define CONSTEXPR const
#endif

// we will need a structure instead of a function, because a templates for functions do not support (or maybe allow) a partial specification
// "error: non-class, non-variable partial specialization ‘t_crc_msbf_table_val’ is not allowed"
// tested on C++14, C++17, C++20
// MSVC 2015 in this case gives: "error C2768: 't_crc_msbf_table_val': illegal use of explicit template arguments"
// if '<N>' -> '(n_bits)', then C++20 says "error: ‘n_bits’ is not a constant expression"
// and one more reason to use a structure instead of a function is that C++11 doesn't support 'constexpr' declarations inside a function body (C++14, C++17 and C++20 do allow)
// *'N' is tested for 8 bits, but should work with 16 too
//// *'N' is tested for 8 bits, but should work with 16 too (is hardcoded to 8 bits though)
template <typename T, T POLYNOMIAL, T VAL, u32 N>
//template <typename T, T POLYNOMIAL, u32 VAL, u32 N>
struct t_crc_msbf_table_val
{
	static CONSTEXPR u32 _size_n_bits = sizeof(T) * 8;
//// use this line if want to un-hardcode it from 8 bits
// MSVS 2002 with no SP: "fatal error C1001: INTERNAL COMPILER ERROR" (3.50GB memory)
	static CONSTEXPR T reg = N > 1 ? t_crc_msbf_table_val<T, POLYNOMIAL, VAL, N - 1>::val : VAL;
// this way it is more nice looking when used, but it then is hardcoded to 8 bit
//	static CONSTEXPR T reg = N > 1 ? t_crc_msbf_table_val<T, POLYNOMIAL, VAL, N - 1>::val : ((T)VAL << (_size_n_bits - 8));
	static CONSTEXPR T v1 = POLYNOMIAL * ((reg >> (_size_n_bits - 1)) & 1);
	static CONSTEXPR T val = (reg << 1) ^ v1;
};
// end here
template <typename T, T POLYNOMIAL, T VAL>
//template <typename T, T POLYNOMIAL, u32 VAL>
struct t_crc_msbf_table_val<T, POLYNOMIAL, VAL, 0>
{
// *not used, but compiler asks for
	static CONSTEXPR T val = 0;
};

// +++ reflected +++
template <typename T, T VAL, s32 N>
struct t_reverse_bits
{
	static CONSTEXPR u32 _size_n_bits = sizeof(T) * 8;
// shift one bit up, to its reversed position
	static CONSTEXPR bool bit_1 = VAL & ((T)1 << N);
	static CONSTEXPR T this_reverse_part_1 = (T)bit_1 << (_size_n_bits - N - 1);
// shift one bit down, to its reversed position
	static CONSTEXPR bool bit_2 = VAL & ((T)1 << (_size_n_bits - N - 1));
	static CONSTEXPR T this_reverse_part_2 = (T)bit_2 << N;
// recurse
// *this condition doesn't help to exclude the recurse stopping template specification
//	static CONSTEXPR T further_depth_swapped_bits = N < (_size_n_bits / 2) ? t_reverse_bits<T, VAL, N + 1>::val : 0;
	static CONSTEXPR T further_depth_swapped_bits = t_reverse_bits<T, VAL, N - 1>::val;
	static CONSTEXPR T val = this_reverse_part_1 | this_reverse_part_2 | further_depth_swapped_bits;
};
// end here
// (I think) it will be more convenient to go from 0 and up to the middle, and after that simply stub with 0
template <typename T, T VAL>
// this doesn't work :/ : "non-type parameter of a partial specialization must be a simple identifier"
//struct t_reverse_bits<T, VAL, sizeof(T) * 8 / 2>
struct t_reverse_bits<T, VAL, -1>
// this gives overflow warnings or errors
//struct t_reverse_bits<T, VAL, 100>
{
// *even if will not be used, compiler would asks for it anyway
	static CONSTEXPR T val = 0;
};
// a recursive macros might be possible ( https://stackoverflow.com/a/12540675 ), but they are very compilcated, - way less readable than a recursive template
#if 0
//**
#define REVERSE_BIT(_val, _pos)   (0)
#define REVERSE_BITS_RECURSE(_val, _pos)   (REVERSE_BIT(_val, (_pos)) | ((_pos) ? REVERSE_BITS_RECURSE(_val, (_pos) - 1) : 0))
#define REVERSE_BITS(_val)   REVERSE_BITS_RECURSE(_val, sizeof(_val) * 8 / 2)
#endif

// *this template could ask (require) for reversed polynomial, instead of reverting it inside of here
// 2023-10-13: yup, giving here an unreverted polynomial (and reverting it here) was a big mistake: 1) compilers compile it slow, 2) GCC works, but MSVC 2008 and 2015 do complain about .bsc having too many references
template <typename T, T POLYNOMIAL_R, u32 VAL, u32 N>
struct t_crc_lsbf_table_val
{
	static CONSTEXPR T reg = N > 1 ? t_crc_lsbf_table_val<T, POLYNOMIAL_R, VAL, N - 1>::val : VAL;
	static CONSTEXPR u32 _size_n_bits = sizeof(T) * 8;
	static CONSTEXPR T v1 = POLYNOMIAL_R * (reg & 1);
	static CONSTEXPR T val = (reg >> 1) ^ v1;
};
// end here
template <typename T, T POLYNOMIAL_R, u32 VAL>
struct t_crc_lsbf_table_val<T, POLYNOMIAL_R, VAL, 0>
{
// *not used, but compiler asks for
	static CONSTEXPR T val = 0;
};
// --- reflected ---

// about the reflection is the best written here: https://stackoverflow.com/a/75951866
//template <typename T, u64 POLYNOMIAL, u32 N>
//template <typename T, u64 POLYNOMIAL, bool REVERSE, u32 N = 0xff>
template <typename T, u64 POLYNOMIAL, bool REVERSE, u32 N = 0>
struct t_crc_table
{
//	const t_crc_table<T, POLYNOMIAL, N - 1> _rest;
//	const t_crc_table<T, POLYNOMIAL, REVERSE, N - 1> _rest;
	const T _x;
	const t_crc_table<T, POLYNOMIAL, REVERSE, N + 1> _rest;
	static CONSTEXPR u32 _size_n_bits = sizeof(T) * 8;
// *can't get rid of '_size_n_bits / 2', because template neither accepts it as argument value, nor can increment only up to it (and then stop without a specific template definition managing where to stop)
	static CONSTEXPR T POLYNOMIAL_R = t_reverse_bits<T, POLYNOMIAL, _size_n_bits / 2>::val;
// *if 'REVERSE', both - the values are going in some reversed-like mixed order (that indices in the table would correspond the data processed), and the table value is reversed
	t_crc_table() :
//		_x(REVERSE ? t_crc_lsbf_table_val<T, POLYNOMIAL, N, 8>::val : t_crc_msbf_table_val<T, POLYNOMIAL, N, 8>::val)
		_x(REVERSE ? t_crc_lsbf_table_val<T, POLYNOMIAL_R, N, 8>::val : t_crc_msbf_table_val<T, POLYNOMIAL, (T)N << (_size_n_bits - 8), 8>::val)
	{}
};

// end here
//template <typename T, u64 POLYNOMIAL>
template <typename T, u64 POLYNOMIAL, bool REVERSE>
struct t_crc_table<T, POLYNOMIAL, REVERSE, 0x100>
{
// MSVC doesn't need this, but Mingw64 complains about that (reports an error "uninitialized blabla...")
#if defined(__GNUC__)
	t_crc_table()
	{}
#endif
};


