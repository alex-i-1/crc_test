// by Alexander Ilyin
// 

#include "crc16.h"
//#include <stdlib.h>
//#include "macros.h"


// either use this exposed structure, or plain C functions provided below otherwise
// CRC-16, as in IBM PC floppy disks
t_crc<u16, 0X1021, 0> crc16(0xFFFF, 0, 0, 0, 0x29b1, 0x0000, "CRC-16/IBM-3740");


void Crc16_InitVal ()
{
	crc16.test ();
	crc16.init_val ();
}

void Crc16_Gen (const void* data, u32 size)
{
	crc16.gen (data, size);
}

void Crc16_GetVal (u16& val)
{
	crc16.get_val (val);
}

