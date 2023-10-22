// by Alexander Ilyin

#include "crc_by_1_bit.h"
//#include <windows.h>
//#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
//#include <math.h>
//#include <io.h>
#include "types.h"
#include "macros.h"
#include "error.h"
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
#include "crc64.h"
#include "crc32.h"


// "'|=' : unsafe mix of type 'unsigned char' and type 'bool' in operation"
#pragma warning(disable:4805)

// nasty hack to exclude 'vector<bool>' specialization from use (which seems is introduced in C++03)
// careful! this might break some of your code! (like some zero-test might fail)
#define bool u8

//#define CRC_LENGTH (32)
// '[0]' is the bit closest to the output
// *this rather is something like "polynomial map" than the polynomial itself
//bool crc_polynomial_map[0100] = {0};
vector<bool> crc_polynomial_map;
// '[0]' is the bit closest to the output
//bool crc_register[0100] = {0};
vector<bool> crc_register;
//vector<u8> crc_register;
u32 crc_size = 0;
// reflect the input (MSB normally goes first, but if this is set, then LSB will go first)
bool refin = 0;
// reflect the output (the register's top bit will appear in bit 0 then)
bool refout = 0;
bool inv_out = 0;
// *it is also possible that this has to be 1 always, and that just some algos are broken
// on the other hand this looks like is tied at the initial value
// 2023-09-29 results: this must be only 1
bool mix_input_first = 0;
// *not sure though why zero padding is sometimes ommitted and sometimes isn't
// this looks like is necessary when the bit which went out from the register isn't mixed with the input bit: the value then has zeroes if the input is shorter than CRC
// 2023-09-29 results: this must be only 0 (the current version of https://en.wikipedia.org/wiki/Cyclic_redundancy_check page is wrong about necessity of zero pad bits of CRC size)
bool zero_pad = 0;


void BinStrToBinVec (vector<bool>& vec_out, string str_in)
{
	A(vec_out.empty());
	while (! str_in.empty())
	{
		u8 b;
		b = str_in[0];
		b -= '0';
// either '0' or '1'
		A(b < 2);
		vec_out.push_back(b);
		str_in.erase(0, 1);
	}
}

void BinVecToBinStr (string& str_out, vector<bool>& vec_in)
{
	A(vec_in.size());
	A(str_out.empty());
// **foreach
	FORI0(vec_in.size())
	{
		str_out += '0' + vec_in[i];
	}
}

// initialize CRC polynomial and length
void CRC_InitPolyBin (string poly)
{
// we must not be initialized yet
	A(! crc_size);
	BinStrToBinVec (crc_polynomial_map, poly);
	crc_size = crc_polynomial_map.size();
}

void CRC_InitRegSolid (bool bits_val)
{
// we have to be initialized
	A(crc_size);
	crc_register.resize(crc_size);
	FORI0(crc_size)
	{
		crc_register[i] = bits_val;
	}
}

void CRC_Reg_Invert ()
{
	FOREACH(it, crc_register)
	{
		(*it) ^= 1;
// this still doesn't work
#if 0
		vector<bool>::reference ref = *it;
		ref ^= 1;
#endif
// this works, but next time I will need an input (a XOR) too
//		*it = ! *it;
	}
}

void CRC_Reg_Reflect ()
{
// we have to be initialized
	A(crc_size);
	reverse(crc_register.begin(), crc_register.end());
}

// fixed to high bit being at the left
void CRC_Reg_ToString_Bin (string& out)
{
	BinVecToBinStr (out, crc_register);
}

// fixed to high bit being at the left
void CRC_Reg_ToString_Hex (string& out)
{
	u8 b;
// we have to be initialized
	A(crc_size);
	A(out.empty());
	b = 0;
	FORI0(crc_size)
	{
		b |= crc_register[i];
// every last bit in a hexadecade convert the accumulated bits into a digit
		if (! ((crc_size - (i + 1)) & 0x03))
		{
			if (b < 10)
			{
				out += '0' + b;
			}
			else
			{
				out += 'A' + b - 10;
			}
			b = 0;
		}
		else
		{
			b <<= 1;
		}
	}
}

void CRC_ProcessBit (bool bit_in)
{
	bool bit_out;

// this bit is being moved out from the shift register
	bit_out = crc_register[0];
	if (mix_input_first)
		bit_out ^= bit_in;
// do the shift
	FORI0(crc_size - 1)
	{
// XOR with 'bit_out' if the map allows
		crc_register[i] = crc_register[i + 1] ^ (bit_out & crc_polynomial_map[i]);
	}
// *this one (the lowest bit of polynomial map) is always 1 (it is written as "+ 1" in the polynomial)
	A(crc_polynomial_map[crc_size - 1]);
	crc_register[crc_size - 1] = bit_out & crc_polynomial_map[crc_size - 1];
	if (! mix_input_first)
		crc_register[crc_size - 1] ^= bit_in;
}

// '[0]' is the bit closest to the output / is the one to be taken/processed first
void CRC_ProcessMessage_Vec (vector<bool>& msg)
{
// we have to be initialized
	A(crc_size);
	FOREACH(it, msg)
	{
		CRC_ProcessBit (*it);
	}
}

void CRC_ProcessMessage_Bin (string in)
{
	vector<bool> vec_msg;
	BinStrToBinVec (vec_msg, in);
// *incorrectly implemented: need to revert every byte
	if (refin)
	{
		reverse(vec_msg.begin(), vec_msg.end());
	}
	CRC_ProcessMessage_Vec (vec_msg);
}

// most significant bit first
void CRC_ProcessMessage_MSBF_Bytes (string in)
{
	string msg_bin;
	FOREACH(it, in)
	{
		u8 b;
		char c;
		b = *it;
		FORI0(8)
		{
			c = b >> 7;
			c += '0';
			msg_bin += c;
			b <<= 1;
		}
	}
	CRC_ProcessMessage_Bin (msg_bin);
}

//bool CrcBy1Bit_Test ()
void CrcBy1Bit_Test ()
//u64 CrcBy1Bit_Test ()
//void CrcBy1Bit_Test (string& s_out)
{
// CRC-8, CRC-8/DVB-S2
#if 0
// 0xD5, 0xAB
// 8, 7, 6, 4, 2, 0
	CRC_InitPolyBin ("11010101");
	CRC_InitRegSolid (0);
	refin = 0;
	refout = 0;
	inv_out = 0;
// works both ways
#if 1
	mix_input_first = 0;
	zero_pad = 1;
#else
	mix_input_first = 1;
	zero_pad = 0;
#endif
#endif

// CRC-32, CRC-32/ISO-HDLC
#if 0
// 0x04C11DB7, 0xEDB88320
// 32, 26, 23, 22, 16, 12, 11, 10, 8, 7, 5, 4, 2, 1, 0
	CRC_InitPolyBin ("00000100110000010001110110110111");
	CRC_InitRegSolid (1);
	refin = 1;
	refout = 1;
	inv_out = 1;
// works only this way
	mix_input_first = 1;
// works only this way
	zero_pad = 0;
#endif

// CRC-32/BZIP2
#if 0
// 0x04C11DB7, 0xEDB88320
// 32, 26, 23, 22, 16, 12, 11, 10, 8, 7, 5, 4, 2, 1, 0
	CRC_InitPolyBin ("00000100110000010001110110110111");
	CRC_InitRegSolid (1);
	refin = 0;
	refout = 0;
	inv_out = 1;
// works only this way
	mix_input_first = 1;
// works only this way
	zero_pad = 0;
#endif

// CRC-32/MPEG-2
#if 0
// 0x04C11DB7, 0xEDB88320
// 32, 26, 23, 22, 16, 12, 11, 10, 8, 7, 5, 4, 2, 1, 0
	CRC_InitPolyBin ("00000100110000010001110110110111");
	CRC_InitRegSolid (1);
	refin = 0;
	refout = 0;
	inv_out = 0;
// works only this way
	mix_input_first = 1;
// works only this way
	zero_pad = 0;
#endif

// CRC-32 POSIX
#if 0
// 0x04C11DB7, 0xEDB88320
// 32, 26, 23, 22, 16, 12, 11, 10, 8, 7, 5, 4, 2, 1, 0
	CRC_InitPolyBin ("00000100110000010001110110110111");
	CRC_InitRegSolid (0);
	refin = 0;
	refout = 0;
	inv_out = 1;
// works both ways
#if 1
	mix_input_first = 0;
	zero_pad = 1;
#else
	mix_input_first = 1;
	zero_pad = 0;
#endif
#endif

// CRC-64/ECMA-128
#if 1
// 0X42F0E1EBA9EA3693
// 62, 57, 55, 54, 53, 52, 47, 46, 45, 40, 39, 38, 37, 35, 33, 32, 31, 29, 27, 24, 23, 22, 21, 19, 17, 13, 12, 10, 9, 7, 4, 1, 0
	CRC_InitPolyBin ("0100001011110000111000011110101110101001111010100011011010010011");
	CRC_InitRegSolid (0);
	refin = 0;
	refout = 0;
	inv_out = 0;
// works both ways
#if 1
	mix_input_first = 0;
	zero_pad = 1;
#else
	mix_input_first = 1;
	zero_pad = 0;
#endif
#endif

// "ABC"
// high bit to the front
// CRC-8 takes this: 79
//	CRC_ProcessMessage_Bin ("010000010100001001000011");
// low bit to the front
//	CRC_ProcessMessage_Bin ("100000100100001011000010");
// "A"
// low bit to the front
//	CRC_ProcessMessage_Bin ("10000010");
// high bit to the front
// CRC-8 takes this: 48
// CRC-32/POSIX takes this: CFB8923F
// CRC-64/ECMA-128: 98F5E3FE438617BC
//	CRC_ProcessMessage_Bin ("01000001");
//	CRC_ProcessMessage_Bin ("01000001000000000000000000000000");
//	CRC_ProcessMessage_Bin ("00000000000000000000000001000001");
//	CRC_ProcessMessage_MSBF_Bytes ("A");
//	CRC_ProcessMessage_MSBF_Bytes ("ABCDEFGHI");
	CRC_ProcessMessage_MSBF_Bytes ("123456789");
	if (zero_pad)
	{
// test pad: 3 bytes
//		CRC_ProcessMessage_Bin ("000000000000000000000000");
// pad bits; the amount is equal to the size of the CRC
//		CRC_ProcessMessage_Bin ("00000000");
		FORI0(crc_size)
		{
			CRC_ProcessMessage_Bin ("0");
//			CRC_ProcessMessage_Bin ("1");
		}
	}
	if (inv_out)
		CRC_Reg_Invert ();
	if (refout)
		CRC_Reg_Reflect ();

	string out_hex;
	string out_bin;
	CRC_Reg_ToString_Hex (out_hex);
	CRC_Reg_ToString_Bin (out_bin);
	printf ("CRC is: %s, %s", out_hex.c_str(), out_bin.c_str());
	printf ("\n");

// catch a particular strings (using a breakpoints which are set by assertions)
// "A"
#if 0
	A(out_hex != "D3D99E8B");
	A(out_hex != "81B02D8B");
	A(out_hex != "2C266174");
	A(out_hex != "7E4FD274");
	A(out_hex != "CFB8923F");
	A(out_hex != "98F5E3FE438617BC");
#endif
// "ABCDEFGHI"
#if 1
	A(out_hex != "C3C9F2011CD365D4");
	A(out_hex != "C835C6078C03E797");
#endif

// no big need to return anything, because our caller will not know what to do with that :)
//	s_out = out_hex;

//	return 1;
	return;
}

