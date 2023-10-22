// by Alexander Ilyin

#include <windows.h>
//#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
//#include <math.h>
#include <io.h>
#include "types.h"
#include "macros.h"
#include "error.h"
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
#include "crc64_plain.h"
#include "crc64_plain_2.h"
#include "crc64.h"
#include "crc32.h"
#include "crc_by_1_bit.h"


// what (which versions) do we have here?
// "crc_by_1_bit.h" - the first wave:
// binary/text processing only
// "crc64_plain.h" - the second wave:
// well, no real C++; especially if tables will be pre-created (specifically here though they are created at run time)
// "crc64.h" - the third wave:
// C++ templates


int main (int argc, char* argv[])
{
//	bool r;
	u64 filetime_beg;
	u64 filetime_end;
	u64 bytes_read;

	GetSystemTimeAsFileTime ((FILETIME*)&filetime_beg);

// MSVC 6.0: 1
// MSVC 2008: 199711, 0x00030c1f
// MSVC 2015: 199711, 0x00030c1f
	u32 cpp_ver;
	cpp_ver = __cplusplus;
	cpp_ver = cpp_ver;

#if 0
	u32 test;
	test = Compute_CRC32_Simple ("A", 1);
	test = test;
	exit(0);
#endif

#if 1
	{
	string s;
	u32 val;
	s = "123456789";
	Crc32_InitVal ();
	Crc32_Gen (s.c_str(), s.length());
	Crc32_GetVal (val);
	val = val;
	}
#endif

#if 1
	{
	string s;
	u64 val;
	s = "123456789";
	Crc64_InitVal ();
	Crc64_Gen (s.c_str(), s.length());
	Crc64_GetVal (val);
	val = val;
	}
#endif

#if 1
	{
	crc32.test();
	cpp_ver = cpp_ver;
	}
#endif

#if 1
	{
	crc64.test();
	cpp_ver = cpp_ver;
	}
#endif

#if 1
	{
	FILE* f;
	int f_no;
	s64 flen;
	const char* filename;
	u32 crc_val;
//#define BUF_SIZE (0x100000)
	void* buf;
	u32 buf_size;
	int i;

	filename = "crc64_plain.cpp";
//	filename = "D:\\vbox\\en_windows_xp_professional_n_with_service_pack_3_x86_cd_x14-92445.iso";
//	filename = "D:\\common-flat.vmdk";
// below 256kB the speeds significantly fall for MSVC 2008 + Windows 10.0.10240
	buf_size = 0x400000;

	buf = malloc (buf_size);
	if (! buf)
	{
		printf ("failed to allocate %08x bytes", buf_size);
		printf ("\n");
		goto after_file_crc_gen;
	}
//	GetSystemTimeAsFileTime ((FILETIME*)&filetime_beg);
	f = fopen (filename, "rb");
	if (! f)
	{
		printf ("failed to open file \"%s\"", filename);
		printf ("\n");
		goto after_file_crc_gen;
	}
	f_no = _fileno (f);
	if (f_no == -1)
	{
		printf ("'_fileno()' failed for file \"%s\"", filename);
		printf ("\n");
		goto after_file_crc_gen;
	}
	flen = _filelengthi64 (f_no);
	if (flen == -1)
	{
		printf ("'_filelengthi64()' failed for file \"%s\"", filename);
		printf ("\n");
		goto after_file_crc_gen;
	}
	bytes_read = 0;
	crc32.init_val();
	while (1)
	{
		u32 n;
// MSVC 2008: no difference in speed between placing the size into the second or third argument
		n = fread (buf, 1, buf_size, f);
//		n = fread (buf, buf_size, 1, f);
		A(n <= buf_size);
		bytes_read += n;
		i = ferror (f);
		if (i)
		{
			printf ("error when reading file \"%s\"", filename);
			printf ("\n");
			goto after_file_crc_gen;
		}
// Intel G1840 CPU, byte by byte, MSVC 2008: Debug - 30.71 MB/s, Release - 176.86 MB/s
// Intel G1840 CPU, byte by byte, MSVC 2015: Debug - 30.40 MB/s, Release - 185.25 MB/s
		crc32.gen(buf, n);
//		crc32.gen(buf, buf_size);
//		crc32.gen2(buf, n);
		if (n < buf_size)
//		if (feof (f))
			break;
	}
	i = feof (f);
	if (! i)
	{
		printf ("no EOF when reading file \"%s\"", filename);
		printf ("\n");
		goto after_file_crc_gen;
	}
	if (bytes_read != flen)
	{
		printf ("file \"%s\" read length %llu is not equal to initial length %llu", filename, bytes_read, flen);
		printf ("\n");
		goto after_file_crc_gen;
	}
	crc32.get_val(crc_val);
//	GetSystemTimeAsFileTime ((FILETIME*)&filetime_end);
	free (buf);
	printf ("file \"%s\" CRC is %08x", filename, crc_val);
	printf ("\n");
after_file_crc_gen:
	cpp_ver = cpp_ver;
	}
#endif

	{
// "A" - 98F5E3FE438617BC (some algos of the same polynomial though insist that it is C835C6078C03E797; those have init=1, xorout=1, refin=1, refout=1)
// "AB" - A8A66E34CEF9D895
// "ABCDEFGHI" - C3C9F2011CD365D4
	string s;
	u64 crc_64;
//	s = "A";
//	s = "AB";
//	s = "ABCDEFGHI";
	s = "123456789";
	crc_64 = Crc64Plain_Calc (s.c_str(), s.length());
	crc_64 = crc_64;
	}

	{
	string s;
	u64 crc_64;
// "ABCDEFGHI" - C3C9F2011CD365D4
//	s = "ABCDEFGHI";
	s = "123456789";
	crc_64 = Crc64Plain2_Calc (s.c_str(), s.length());
	crc_64 = crc_64;
	}

//	string s_crc;
//	r = CrcBy1Bit_Test ();
	CrcBy1Bit_Test ();
//	CrcBy1Bit_Test (s_crc);
//	s_crc = s_crc;

	GetSystemTimeAsFileTime ((FILETIME*)&filetime_end);
	double d;
	d = filetime_end - filetime_beg;
	d /= TI_SECOND;
	printf ("done in %f seconds", d);
	printf ("\n");
	d = bytes_read / d;
	d /= 1024;
	d /= 1024;
	printf ("%f MB/s", d);
	printf ("\n");

// flush the character input
	while (_kbhit ())
	{
		_getch ();
	}
	printf ("press any key to exit");
	printf ("\n");
	while (1)
	{
		if (_kbhit ())
			break;
		Sleep (50);
	}

	return 0;
}

