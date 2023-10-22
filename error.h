#pragma once
// 2022-07-19: lets finally do a clean version of this file
// 1) the error subroutines aren't necessary anymore, thus we don't need "error.cpp" file
// 2) '__asm int 3' doesn't work on 64 bit platform; '__debugbreak();' works on 64 bit platform; hence, let use it instead
// *'__debugbreak' is not present in MSVC 6.0 (File version 12.0.9782.0, Product version 6.00.9782.0), but is present in MSVC 7.0 (File version 13.0.9466.0, Product version 7.00.9466)
// *'SIGTRAP' isn't present even in MSVC 2008 (9.0)
// *'DebugBreak()' is a real function, and we wouldn't want it
// 2022-09-19: this file currently belongs at "fft" project (not really belongs, but the mostly recent version should be obtained from there)
// 2023-01-31: became a library

#include "types.h"
// 'STRINGIZE()'
//#include "macros.h"
#if defined(__GNUC__)
//#include <signal.h>
#include <debugapi.h>
#endif


#if 0
#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)
#endif
#if 0
#define STRING2(x) #x
#define STRING(x) STRING2(x)
#pragma message(STRING(_MSC_VER))
//#error STRING(_MSC_VER)
#endif

#ifdef _MSC_VER
//// higher than MSVC 6.0
//#if _MSC_VER > 1200
// not lower than MSVC 7.0
#if _MSC_VER >= 1300
#define DEBUG_BREAK __debugbreak();
#else
#define DEBUG_BREAK __asm int 3
#endif
#elif defined(__GNUC__)
// don't know if it isn't supported by any old GCC
// '__builtin_trap()' simply executes an invalid instruction, - that's not for breakpoints
//#define DEBUG_BREAK __builtin_trap();
// Linux
//#define DEBUG_BREAK raise(SIGTRAP);
// MinGW
// no actual difference: IDE doesn't catch breakpoints anyway
#define DEBUG_BREAK DebugBreak();
#else
#error not implemented for this compiler
#endif
// newer Clang has this (if '__has_builtin(__builtin_debugtrap)')
// #define DEBUG_BREAK __builtin_debugtrap();

// 2023-01-31: error throwing macro became so common, so I am redoing 'E' to it: 'ET' now will be just 'E', and 'E' becomes 'EP' (error, with printing)
// 2023-01-31: (irrelevant to errors) also got a thought, that for the ': ;' which are occurring at the end of a scope sometimes, is better not choose between moving ';' onto next line or not moving, but write ';;' on next line instead
// 2023-02-05: yet no clear conception on 'E()' and 'EP()'; for a while simply use 'E()' when it is sensitive, and use 'EP()' when error must be printed (the error function will do 'exit()' and not hit a breakpoint)

// error throw
//#define ET do { DEBUG_BREAK } while (0)
#define E do { DEBUG_BREAK } while (0)
// assertion
#define A(_assertion) do { if (! (_assertion)) E; } while (0)

// *MSVC 6.0 does not know '__FUNCTION__'
// instead of writing two kind of functions, it easier can be hacked
// '#if defined(__FUNCTION__)' always fails on GCC; though who cares (?) - on MSVC it works properly
// also, if on GCC define '__FUNCTION__', then it will overload the regular one
// '__FUNCTION__' is introduced in MSVC 2002 :  http://msdn.microsoft.com/en-us/library/2byy0fh6%28v=vs.80%29.aspx
//#if _MSC_VER < 1300
#if ! defined(__FUNCTION__)
#define __FUNCTION__ "---"
#endif

//void _Error (bool do_exit, const char* format, ...);
//void _ErrorPrint (const char* format, ...);
void _ErrorPrint (const char* text_pre, const char* format, ...);
//void _ErrorPrint (bool is_assertion, const char* text_pre, const char* format, ...);

//#define EP(_format, ...) do { _ErrorPrint("File \"" __FILE__ "\", line " STRINGIZE(__LINE__) ", function '" __FUNCTION__ "'\r\n", _format, __VA_ARGS__) } while (0)
//#define EP(_format, ...) _ErrorPrint("File \"" __FILE__ "\", line " STRINGIZE(__LINE__) ", function '" __FUNCTION__ "'\r\n", _format, __VA_ARGS__)

