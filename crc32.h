#pragma once

#include "types.h"
#include "crc_template.h"


extern t_crc<u32, 0x04C11DB7, 1> crc32;


void Crc32_InitVal ();
void Crc32_Gen (const void* data, u32 size);
void Crc32_GetVal (u32& val);

