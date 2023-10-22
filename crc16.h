#pragma once

#include "types.h"
#include "crc_template.h"


extern t_crc<u16, 0X1021, 0> crc16;


void Crc16_InitVal ();
void Crc16_Gen (const void* data, u32 len);
void Crc16_GetVal (u16& val);

