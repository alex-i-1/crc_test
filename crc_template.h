#pragma once

#include "types.h"
#include "crc_table_gen.h"
#include "macros.h"
#include "error.h"


// look this for the list of setup values (and maybe it will give a clue on how to use this structure template)
// https://reveng.sourceforge.io/crc-catalogue/all.htm
// it offers the further components for CRCs: width, poly, init, refin, refout, xorout, check, residue, name
// where "check" and "residue" aren't that necessary
// "name" is optional

// here you can set any value for 'REVERSE' parameter (and the template should work anyway), but for a better algorithm to be in use, choose 'REVERSE' equal to 'refin' and 'refout'
// this will reverse the table and its content, this way allowing not to reverse the data being on input

// there is only single CRC-12/UMTS which has different 'refin' and 'refout'
// (thanks to the one who noted it here https://stackoverflow.com/a/75951866 )
// in this case set 'REVERSE' equal to 'refin' (if you want the algo to be quicker)

// *don't give a data straightforward here
// give only a copy of it, because we are modifying it
static void ReverseBits (u8& b)
{
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
}

static void ReverseBits (u16& val)
{
	ReverseBits (((u8*)&val)[0]);
	ReverseBits (((u8*)&val)[1]);
	val = (val & 0xFF00) >> 8 | (val & 0x00FF) << 8;
}

static void ReverseBits (u32& val)
{
	ReverseBits (((u16*)&val)[0]);
	ReverseBits (((u16*)&val)[1]);
	val = (val & 0xFFFF0000) >> 16 | (val & 0x0000FFFF) << 16;
}

static void ReverseBits (u64& val)
{
	ReverseBits (((u32*)&val)[0]);
	ReverseBits (((u32*)&val)[1]);
	val = (val & 0xFFFFFFFF00000000) >> 32 | (val & 0x00000000FFFFFFFF) << 32;
}

//template <typename T, u64 POLYNOMIAL>
template <typename T, u64 POLYNOMIAL, bool REVERSE>
struct t_crc
{
// +++ settings +++
	const u32 n_bits;
	const T poly;
	const T init;
	const bool refin;
	const bool refout;
	const T xorout;
// "123456789"
	const T check;
	const T residue;
	const char* const name;
	const t_crc_table<T, POLYNOMIAL, REVERSE> crc32_table_struct;
	const T* const table;
// --- settings ---
	T crc;

	t_crc (T init_, bool refin_, bool refout_, T xorout_, T check_, T residue_, const char* name_)
		:
		n_bits(sizeof(T) * 8),
		poly(POLYNOMIAL),
		init(init_),
		refin(refin_),
		refout(refout_),
		xorout(xorout_),
		check(check_),
		residue(residue_),
		name(name_),
		table((T*)&crc32_table_struct)
	{
		init_val ();
	}
	void init_val ()
	{
		crc = init;
	}
// use this only for CRC-16 and above (would actually work for CRC-8 too, but a piece of irrelevant extra code will yet be in use)
// Debug isn't inlining it even with 'inline'; Release is inlining it even without 'inline'
//	void gen_byte (u8& b)
	void gen_byte (u8 b)
//	inline void gen_byte (u8 b)
	{
// reverse if the order doesn't accord to the tables
		if (refin ^ REVERSE)
//			reverse_byte (b);
			ReverseBits (b);
		if (REVERSE)
// shifting down (right)
		{
			b ^= crc;
			crc >>= 8;
		}
		else
// shifting up (left)
		{
			b ^= crc >> (n_bits - 8);
			crc <<= 8;
		}
		crc ^= table[b];
	}
// what bad here we've got when compiling with MSVC 2015, Release
// 1 extra jump (plus 1 extra op), which lags the mostly (the one which is going around 'ReverseBits()')
// 1 extra op which is putting the value to 'crc' (the optimizer really missed this one)
	void gen (const void* data, u32 len)
	{
		FORI0(len)
		{
			u8& b = ((u8*)data)[i];
			gen_byte (b);
		}
	}
	void get_val (T& val)
	{
// specific xorout have these CRCs: CRC-8/I-432-1, CRC-15/MPT1327, CRC-16/DECT-R
// "... after the optional reflection"
		val = crc;
// reverse if the algorithm with tables isn't giving the result reversed already
		if (refout ^ REVERSE)
			ReverseBits (val);
		val ^= xorout;
	}
	void test ()
	{
		T val;
		init_val ();
		gen ("123456789", 9);
		get_val (val);
		A(val == check);
	}
};

