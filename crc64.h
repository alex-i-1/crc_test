#pragma once

#include "types.h"
#include "crc_template.h"


extern t_crc<u64, 0X42F0E1EBA9EA3693, 1> crc64;


void Crc64_InitVal ();
void Crc64_Gen (const void* data, u32 len);
void Crc64_GetVal (u64& val);

