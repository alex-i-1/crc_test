// by Alexander Ilyin
// 

#include "crc64_plain_2.h"
#include <stdlib.h>
#include "macros.h"


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

// *am lazy to copy/paste the value generating template here
extern const u64 crc64_msbf_table_val (const u64 ix_shifted, const u32 n_bits);

// 2023-10-02: adding 'const' to variables does not change anything a bit, - the '[]' operator is still of 256 comparisons
//template <u32 N_BITS_TOP, u64 BASE>
template <typename T, u32 N_BITS_TOP, T BASE>
//struct t_crc64_table
struct t_crc64_table_recurse
{
	static CONSTEXPR u32 BIT_TO_TOGGLE_NOW = N_BITS_TOP - 1;
	static CONSTEXPR u32 N_BITS_TOP_NEXT = BIT_TO_TOGGLE_NOW;
	static CONSTEXPR T BASE_OFFSET_1 = 1 << BIT_TO_TOGGLE_NOW;
	static CONSTEXPR T BASE_0 = BASE;
	static CONSTEXPR T BASE_1 = BASE | BASE_OFFSET_1;
// the content is at the beginning (these values come first); simply take a pointer to an instance of this structure and use it as a table
	const t_crc64_table_recurse<T, N_BITS_TOP_NEXT, BASE_0> _part_0;
	const t_crc64_table_recurse<T, N_BITS_TOP_NEXT, BASE_1> _part_1;
// GCC would complain about absense of this constructor
	t_crc64_table_recurse()
	{}
#if 0
//	const u64 operator[](u32 const& i) const
	const u64& operator[](u32 const& i) const
	{ return (.....); }
#endif
};

// on 0 level stop recurse and generate the value itself
template <typename T, T BASE>
struct t_crc64_table_recurse<T, 0, BASE>
{
	static CONSTEXPR u32 n_bits_ix = sizeof(BASE) * 8;
//	static CONSTEXPR u32 n_bits_val = sizeof(u64) * 8;
	static CONSTEXPR u64 ix_shifted = (u64)BASE << (64 - n_bits_ix);
	const u64 _x;
	t_crc64_table_recurse()
		:
		_x(crc64_msbf_table_val (ix_shifted, n_bits_ix))
	{}
#if 0
//	const u64 operator[](u32 const& i) const
	const u64& operator[](u32 const& i) const
	{ return _x; }
#endif
};

// the recursing template requires too many arguments; our purpose is to fill in those and insulate a user from bothering with that
template <typename T>
struct t_crc64_table
{
	static CONSTEXPR u32 n_bits = sizeof(T) * 8;
	t_crc64_table_recurse<T, n_bits, 0> _crc64_table_recurse;
};


// only a small piece of second to create this table an unoptimal way (means can just forget about optimizing it)
// in Release there is no recursive call, but 256 comparisons with jumps
t_crc64_table<u8> crc64_table_struct_2;
// that's a fragile code a bit :/
//const u64* crc64_table = &crc64_table_struct[0];
const u64* crc64_table_2 = (u64*)&crc64_table_struct_2;


u64 Crc64Plain2_Calc (const void* data, u32 len)
{
	volatile u64 tab_val;
	u64 crc;

// test
#if 0
// doesn't work :/ "error C2102: '&' requires l-value"
// works if '[]' operator returns a reference
	ptr = &crc64_table_2[0];
	ptr = ptr;
#endif

// test
#if 0
// f45bb4758c645c51
	tab_val = crc64_table_2[0x18];
	tab_val = tab_val;
	exit(0);
#endif
	crc = 0;
	FORI0(len)
	{
		u8 b;
		u8 crc_expelled_part;
		b = ((u8*)data)[i];
		crc_expelled_part = crc >> 56;
		tab_val = crc64_table_2[crc_expelled_part ^ b];
		crc <<= 8;
		crc ^= tab_val;
	}
	crc = crc;

	return crc;
}

