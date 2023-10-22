// by Alexander Ilyin
// 

#include "crc64_plain.h"
#include <stdlib.h>
#include "macros.h"


// 2023-09-30: well, this table method needs to be explained
// taking a window of 8 bit, it is noticable, that there is 1 tap (or more) which breaks bits into "before" and "after" it
// the left side is close to be expelled, and is not being affected by it, but there are also bits to the right - those yet will have to pass this tap, and their value will depend on that
// note also that some bits will not be expelled yet, but still pass this tap before the 8 bits will be cycled through, - those bits have no importance for us right now
// lets return to those bits which have to pass the tap, and yet they also belong to this 8 bit pack, means they will get expelled
// this might look very complicated, and we need to have a simple explanation (since the method is simple):
// this table / these tables do not take a 8 bits of input data, nor the 8 bits to be expelled
// this table instead represents an image of what bits do invert when having these "8 bits on the brink of the register" and these "8 bits of input data"
// these two are simply XOR-ed, as it occurs in normal processing
// one more explanation: tables are taking 8 bits in that their state that they were at the moment they were on their "places", not when each of them was on the last bit; tables are taking, and giving the value which to XOR-apply to the register
// once more: you are giving the tables exactly same 8 bits as those which were giving when they were created (thus they are giving back exactly what is needed)
// 2023-10-02: one more thing to note: by this tabled algorithm, 8 bits are expelled (and processed), and the newly inserted 8 bits are just set to "0"; then the table value is XOR-ed with the reg, - only the table value gives those 8 bits in the low end of the register

#define CRC64_POLYNOMIAL 0X42F0E1EBA9EA3693

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

// an example for template-initialized array is taken from:
// https://stackoverflow.com/a/2228298
// and here:
// https://stackoverflow.com/a/37447199
// and note also one more similar method which though I don't use (because a two-int-argumented template will not work on MSVC 6.0, which I, oddly enough, yet use in this project):
// https://stackoverflow.com/a/3499919

// one more less or more easy-to-read example of CRC implementation is here:
// https://bitbucket.org/spandeygit/crc32v/src/master/tablegen_8bit.c
// function 'Compute_CRC32_Simple()'
// I found it in this shabby video :p :
// https://www.youtube.com/watch?v=q1l_rpQQaC8
// Table Based CRC32 - Introduction

// most significant bit first
// takes value, returns reg (align your data to the top, because in recurse we will not know how much to shift it to align)
//// takes reg and returns reg (align your data to the top, because in recurse we will have all 64 bits in this reg)
// that's odd; I hadn't think this is possible/allowed to be used in templates
// well, that is possible, but will it be compile-time processed?
// 2023-10-01: not one of these 'const' is making the code shorter neither in Debug or Release
//const u64 crc64_msbf_table_val (const u32 val, const u32 n_bits)
//const u64 crc64_msbf_table_val (const u64 reg, const u32 val, const u32 n_bits)
//const u64 crc64_msbf_table_val (const u64 val, const u32 n_bits)
const u64 crc64_msbf_table_val (const u64 ix_shifted, const u32 n_bits)
{
	const u64 reg = n_bits > 1 ? crc64_msbf_table_val (ix_shifted, n_bits - 1) : ix_shifted;
	const u64 v1 = CRC64_POLYNOMIAL * ((reg >> 63) & 1);
	return (reg << 1) ^ v1;
}

// 2023-10-02: adding 'const' to variables does not change anything a bit, - the '[]' operator is still of 256 comparisons
template <u32 I>
struct t_crc64_table
{
	const t_crc64_table<I - 1> _rest;
	const u64 _x;
	t_crc64_table()
		:
		_x(crc64_msbf_table_val ((u64)I << 56, 8))
	{}
#if 0
//	const u64 operator[](u32 const& i) const
	const u64& operator[](u32 const& i) const
	{ return (i == I ?  _x : _rest[i]); }
#endif
};

template <>
struct t_crc64_table<0>
{
	const u64 _x;
	t_crc64_table()
		:
		_x(crc64_msbf_table_val (0, 8))
	{}
#if 0
//	const u64 operator[](u32 const& i) const
	const u64& operator[](u32 const& i) const
	{ return _x; }
#endif
};


// only a small piece of second to create this table an unoptimal way (means can just forget about optimizing it)
// in Release there is no recursive call, but 256 comparisons with jumps
t_crc64_table<0xff> crc64_table_struct;
// that's a fragile code a bit :/
//const u64* crc64_table = &crc64_table_struct[0];
const u64* crc64_table = (u64*)&crc64_table_struct;


u64 Crc64Plain_Calc (const void* data, u32 len)
{
	volatile u64 tab_val;
	u64 crc;

// test
#if 0
// doesn't work :/ "error C2102: '&' requires l-value"
// works if '[]' operator returns a reference
	ptr = &crc64_table[0];
	ptr = ptr;
#endif

// test
#if 0
	tab_val = crc64_table[0x18];
// 42f0e1eba9ea3693
	tab_val = crc64_msbf_table_val ((u64)0x01 << 56, 8);
// 85e1c3d753d46d26
	tab_val = crc64_msbf_table_val ((u64)0x02 << 56, 8);
// f45bb4758c645c51
	tab_val = crc64_msbf_table_val ((u64)0x18 << 56, 8);
// f6fae5c07d3274cd
	tab_val = crc64_msbf_table_val ((u64)0x80 << 56, 8);
// 9afce626ce85b507
	tab_val = crc64_msbf_table_val ((u64)0xff << 56, 8);
	tab_val = tab_val;
	exit(0);
#endif
	crc = 0;
//	crc = 0xffffffffffffffff;
	FORI0(len)
	{
		u8 b;
		u8 crc_expelled_part;
		b = ((u8*)data)[i];
		crc_expelled_part = crc >> 56;
		tab_val = crc64_table[crc_expelled_part ^ b];
		crc <<= 8;
		crc ^= tab_val;
	}
//	crc ^= 0xffffffffffffffff;
	crc = crc;
//	exit(0);

	return crc;
}

