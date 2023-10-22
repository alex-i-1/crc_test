// by Alexander Ilyin
// 

#include "crc32.h"


// either use this exposed structure, or plain C functions provided below otherwise
t_crc<u32, 0x04C11DB7, 1> crc32(0xffffffff, 1, 1, 0xffffffff, 0xcbf43926, 0xdebb20e3, "CRC-32/ISO-HDLC");


void Crc32_InitVal ()
{
	crc32.test ();
	crc32.init_val ();
}

void Crc32_Gen (const void* data, u32 size)
{
	crc32.gen (data, size);
}

void Crc32_GetVal (u32& val)
{
	crc32.get_val (val);
}

