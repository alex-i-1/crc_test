// by Alexander Ilyin
// 

#include "crc64.h"
//#include <stdlib.h>
//#include "macros.h"


// either use this exposed structure, or plain C functions provided below otherwise
// *here 'REVERSE' is intentionally wrong, to see if algorithm correction works properly; set it to 0 for normal use
t_crc<u64, 0X42F0E1EBA9EA3693, 1> crc64(0, 0, 0, 0, 0x6c40df5f0b497347, 0x0000000000000000, "CRC-64/ECMA-182");


void Crc64_InitVal ()
{
	crc64.test ();
	crc64.init_val ();
}

void Crc64_Gen (const void* data, u32 size)
{
	crc64.gen (data, size);
}

void Crc64_GetVal (u64& val)
{
	crc64.get_val (val);
}

