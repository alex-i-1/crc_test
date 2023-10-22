#pragma once

// by Alexander Ilyin
// belongs at Drag Watcher project
// from 2023-01-20 it doesn't belong anywhere, but is just between my common lib files

// *I find these types rather old; u8, u16, u32 - are more convenient
#if 0
// define unsigned types
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned __int64 uint64;

// define signed types
typedef __int64 int64;
typedef signed __int64 sint64;
#endif

// "unary minus operator applied to unsigned type, result still unsigned", rises on some MSVC higher than 6.0
#pragma warning(disable:4146)
// C4244 is more disturbing and forcing you write extra code, than helping
// have to fight with this on a proper level
#pragma warning(disable:4244)
// appears on a newer compilers; on MSVC 2008 it is a complain about 64 -> 32 bit
// somewhere in between of MSVC 2008 and 2015 it also started to complain about u32 -> u8
#pragma warning(disable:4267)
// *"warning C4305: '=' : truncation from 'const unsigned int' to 'bool'" I think it still might be useful
// even a conversions to bool which I am making are intentional, and such a conversion appears erroneous very very rare
#pragma warning(disable:4800)
// "warning C4838: conversion from 'int' to 'const u32' requires a narrowing conversion" when initializing 'u32' with '-1' on MSVC 2015 (but not yet when it is on MSVC 2008)
#pragma warning(disable:4838)

// *here first I tried to define types like 'i1' for 8 bit, but appears it is not good idea (for example 's1' makes a collide between a type and a var which we prefer to use)
// *with types like '__int8', these typedefs are looking nice, but later appear problems of incompatibility
// a pointer to such type is not compatible with lets say 'char', and 'std::cout' doesn't know what to do with them either

// define unsigned types
#if 0
typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;
#endif
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#if defined(_MSC_VER)
typedef unsigned __int64 u64;
#endif
#if defined(__GNUC__)
typedef unsigned long long u64;
#endif
// '__int128' basicly is not supported on x86
//typedef unsigned __int128 u128;

// define signed types
#if 0
typedef signed __int8 s8;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;
//typedef signed __int128 s128;
#endif
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
#if defined(_MSC_VER)
typedef signed __int64 s64;
#endif
#if defined(__GNUC__)
typedef signed long long s64;
#endif
//typedef signed __int128 s128;

// testing; maybe these will appear convenient in use too
typedef float fl32;
typedef double fl64;

// for MSVC 6.0
//#ifndef wchar_t
#if defined(_MSC_VER) && _MSC_VER <= 1200
typedef u16 wchar_t;
//typedef ushort wchar_t;
#endif

#define PTRBYTESNUM (sizeof(void*))
#define PTRBITSNUM (PTRBYTESNUM * 8)

// address-sized type
// the first version was "aszd" - address-sized, but now I think the ending "ed" is superfluous
// no idea, why I made these types incompatible with 'u32'-like (for example '__int16' is a different type than 'short'); perhaps to show the incompatibility intentionally ...
#if defined(_M_I86)
//typedef u16 asz;
typedef unsigned __int16 asz;
#endif
#if defined(i386)
typedef u32 asz;
#endif
#if defined(__i386)
typedef u32 asz;
#endif
#if defined(__i386__)
typedef u32 asz;
#endif
// **also is defined on 16 bit Watcom compiler
#if defined(_M_IX86) && ! defined(_M_I86)
typedef unsigned __int32 asz;
#endif
#if defined(__amd64__)
typedef u64 asz;
#endif
#if defined(__x86_64__6)
typedef u64 asz;
#endif
#if defined(_M_X64)
typedef u64 asz;
#endif
//#if
//#error unsupported address size
//#endif


// *no define for it in SDK or DDK (GTI_SECONDS only)
#define TI_MICROSECOND ((u64)10)
#define TI_MILLISECOND (1000 * TI_MICROSECOND)
#define TI_SECOND (1000 * TI_MILLISECOND)
#define TI_MINUTE (60 * TI_SECOND)
#define TI_HOUR (60 * TI_MINUTE)
#define TI_DAY (24 * TI_HOUR)


// 2023-05-22: moved here from "type.h" of WellForm
// 2023-05-22: name 'masked' isn't very proper, but sensitively/sensibly better than 'conditional_xxx' or 'maybe<T>'
// 2023-05-22: there is 'tribool' out there which is very similar; we though are different in the way that we don't provide you a way to use this value with a math or logical operators directly (like 'if (mbool) ...' and 'if (! mbool) ...'); instead you have to write an explicit code which is separately accessing the masking bool and the value
// we also don't provide a way to undefine it with '=' operator: you instead have to call the explicit 'mval::Undefine()'
template<typename T> struct mval
{
// is in use / is present
//	bool on;
	bool is;
// value
	T v;

// *this 'Clear' is unclear
//	void Clear ()
	void Undefine ()
	{
		is = 0;
		v = 0;
	}
// *it would be better to overload the '=' ... or maybe worse
#if 0
	void Set (T val)
	{
		is = 1;
		v = val;
	}
#endif
// *not really necessary actually, because a comparisons between 'is' and 'v' normally are being necessary to do separately
// 2023-05-22: not really necessary because of other reason: it isn't clear what in particular that could be for
#if 0
	bool operator == (mval<T>& right)
	{
		return (this->is == right.is) & (this->v == right.v);
	}
	bool operator != (mval<T>& right)
	{
		return ! (*this == right);
	}
#endif
// let assign but not take
	void operator = (const T& val)
	{
		this->is = 1;
		this->v = val;
	}
};

typedef mval<bool> mbool;
typedef mval<u32> mu32;

