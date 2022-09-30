
/*
============================================================================
 File:      foundation.h
 Project:   Data Science Systems
 Author:    Carlos Ordonez
 Contents:  data types, basic functions, micro classes: used everywhere
            size: Make sure data type has the righ # of bits, regardless of CPU and compiler
            trace: to track progress, always
            randomNormalized(): to avoid portability problems, or forgetting seeding
 Usage:     #include "utility.h" at the top right after last #include <>
            it includes:
            -portable types regardless of platform
 Tested:    OS: Windows, Linux
            compiler: g++, Clang pending
 Notes:     ONLY include portable code. Test in several platforms before
	    adding a function here
	    It requires C++; not compatible with C anymore
            try using functions here instead of implementing new ones
 Changes:
1999.05.15 created, merging several header files
2000.06.07 right trailing spaces trimmed
           recognizes 'e' '+' '-' for scientific format
2000.08.20 problems with rand() size of int in randomNormalized()
       in forge to normalize divide by 15
		       in danube to normalize divide by 31
2000.08.21 problem with random no. generator...does not reach 1
		       drand48() used instead
2000.09.18 bug corrected in next_integer to
		       read associations as sets of ints delimited by { }
2000.09.20 Timer
2001.06.21 typedef gives problems for printf
       arguments need to be casted
2001.06.22 real8 real4 added
2001.06.22 portability problems solved for randomNormalized()
                               debug.h deleted
2001.06.26 byte order
2002.10.20 bug in display timer...%lf
                       also use fabs to get elapsed time
                      iostream.h is deprecated
2002.11.20 bug corrected in next_double()
                      program was not updating 'i' when the line
                      did not contain a single number
2003.08.28 timer start has \n
2003.09.01 bug: readln does not check length available of
                      target string; would need top have 3rd param
2005.01.21 debug message
                      mem/disk storage
                      changed processor types to INTEL16,INTEL32,SUN32
2005.01.22 DebugMessage and Time are now global vars to
                      avoid creating them in every class
2005.01.26 testing with TC3
               bug: wrong size printed in memory meter
             tc3 gives only 64k in a malloc call, 'new' gives no NULL
2005.03.03 tests with bcc32
2005.04.09 EXPORT_EXTENSION
2005.10.03 PI constant
2006.02.10 log constants
2014.02.28 notify no output to screen
2014.03.01 remove warning with BYTE_ORDER
2014.03.02 eliminated int2 and warning with unsigned int
           program should be called with ; separating parameters
2015.09.10 migrate to g++ 64 bit compiler
           C++ 11
2018.06.04 Cygwin did not work on XP
2019.01.07 Mingw
           pending: 
              iostream?
              namespace
              random()
           trace.log in csv format
2019.01.08 space () for control statements
2019.01.11 clean non printable char
           pending: char *,string..prefer len @s[0]
2019.01.13 namespace? con: it would need :: or 'using'  to get tm_stdout.
2019.01.17 two output streams
           stdout
           trace
           subtle error with timer: reset because
           TraceMessage was passed by value
2019.01.18 Timer fixed
2019.01.21 TraceMessage instead of FILE * as view() argument
2019.02.05 tuned constants
           REAL64FORMAT
           initializeRandom()
2019.09.09 restarted data set, it depends on readln()
           pending: subtitute readln() with faster fgets() function
2019.09.12 next_int changed int32
           timestamp
           current function name, macro at compile run-time
2019.09.22 string_sys improving strcpy_safe (cumbersome to call)
           at [0]: unnatural, requires changing all calls
           str: set capacity after null (uint16==2 bytes)
                done: strcpy_safe(s1,s2)
                pending: strcat_safe(s1,s2),strapp_safe(s,char)
                fine: strlen,strcmp,strup,strlwr
2019.09.25 REALD64FORMAT
           text line input max length
2019.10.02 split into .h and .cpp, following Makefile convention
2019.10.15 warning() cut
           enum message type for TraceMessage
2019.10.27 two safe strings:
            String: 64kb
            string: 255b
2020.01.07 text file separator: defauilt , for csv
2020.02.10 pending: 'n' 'y' as data type
2020.02.11 fixd bug: next_uint32()
2020.02.13 next_int32() next_real64()
2020.02.17 uint32 eliminated from code
2020.10.01 STRING format
2020.10.04 missing real64
2021.01.22 namespace std
2021.02.06 str(s)  s+2 ==> str_string
            move all Str,Str08,Str16 code to str.h
            C safe string functions are here
2021.11.06 long string32768
2022.09.11 clash with byte in C++ 17
2022.09.12 isidsymbol()
===========================================================================
*/


/* C */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>  // new
// #include <stdint.h> // clash?
//#include <sys/types.h>
//#include <sys/timeb.h> // deprecated
/** C++ 11, 17 */
#include <cmath>
#include <typeinfo>
#include <ctime>
#include <string>
using namespace std;


#if !defined(FOUNDATION)
#define FOUNDATION

//========================= data types ==============================
// choose from #define below
#define GNU64   // default
//#define CPP11
//#define BCC32
//#define GNU


// more explcit than cstdint
#ifdef GNU64
// Integers, in # of bits
  typedef unsigned char       uint08;
  typedef unsigned short      uint16;
  typedef unsigned int        uint32;
  typedef unsigned long       uint64;
  typedef char                int08;
  typedef short               int16;
  typedef int                 int32;
  typedef long int            int64;
  typedef double              real64;    // float|double|long double
  typedef unsigned char       byte08;    // 8 bits, to avoid clash C++ 17 byte
//  typedef float          real32;  // remove it, not allowed

#endif

#ifdef CPP11
#include <cstdint>
  typedef std::int8_t         int08;
  typedef std::int16_t        int16;
  typedef std::int32_t        int32;
  typedef std::int64_t        int64;
  typedef std::uint8_t        uint08;
  typedef std::uint16_t       uint16;
  typedef std::uint32_t       uint32;
  typedef std::uint64_t       uint64;
  typedef double              real64;    // float|double|long double
  typedef unsigned char       byte08;   // 8 bits, to avoid clash C++ 17 byte
#endif


// deprecated
#ifdef GNU32
  typedef unsigned char  uint8;
  typedef unsigned short uint16;
  typedef unsigned long  uint32;
  typedef short          int16;
  typedef int            int32;
  typedef double         real64;    // float|double|long double
  typedef float          real32;
#endif


// backward compatibility
#ifdef BCC32
  typedef unsigned char   uint08;
  typedef unsigned short  uint16;
  typedef unsigned int    uint32;
  typedef unsigned long long int  uint64;
  typedef int             int32;
  typedef short           int16;
  typedef int             int32;
  typedef long long int   int64;
  typedef double          real64;    // float|double|long double
 //  const int32 *nullptr=0; type clash
  #define nullptr 0
  typedef unsigned char   byte08;
#endif

//--------------------------------------------------------------
// strings compatible with C and for secondary storage
// dual: \0 terminated, or length given by data type (1 extra byte)
// 1 byte for null, optional 1 byte for capacity, 1 byte for string length
  typedef char    string08[8];  // default for grid, 7 byte dictionary for long words?
  typedef char    string16[16]; // default SSO

  typedef char    string32[32];    // identifiers, names
  typedef char    string64[64];    // values
  typedef char    string128[128];  // values of parameters
  typedef char    string256[256];  // csv line low d
  typedef char    string1024[1024]; // csv line medium d
  /* they should be discuraged and (i j v) instead */
  typedef char    string65535[65535]; // csv line high d 64kb==16 bits , avoid sign problem with 16 bits
  // overflow uint08 capacity typedef char    string256[256];



// direct I/O, memcpy
#define BYTE_ORDER_32       0x44332211  // most significangt digit highest value
#define BYTE_ORDER_64       0x0102030405060708

//------------------- constants -------------------------
// DELETE later
// this should not be passed as arguments because they are not tied to data type
//const uint16 max_text_line_len=1024; // text I/O, including dataset*
//const uint32 max_len_value=24;  // for values uint32,real64,str column values

//const uint32 max_len_id=32;  // for identifiers, dimnension names, column names

const uint32 len4=4;  // one word 32 bits
const uint32 len8=8;  // one word 64 bits
const uint32 width_terminal=80; // terminal width
const uint32 len256=256;
const uint32 len8bits=256;
const uint32 len16bits=65535;
const uint32 oneKB=1024;
const uint32 oneMB=1048576;
const uint64 oneGB=(uint64)1048576*1048576;
const uint64 thirtytwogb=(uint64)32*1048576*1048576; // default 32 bits?

//#define BILLION 1000000000L // C
const uint64 BILLION=1000000000;

// const char default_separator=',';

const real64 real64_undefined=nan("-1");
const real64 real64_missing=nan("-2");
//-------------------------- FORMAT ----------------------------------
#define REAL64FORMAT         "%10.3lf"     // value
#define COLNAMEREAL64FORMAT  "%10s"        // header
#define REAL64FORMATEXP      "%10.3e"
#define INT64FORMAT          "%10l"
#define UINT64FORMAT         "%10lu"
#define COLNAMEINT64FORMAT   "%10s"       // header
#define INT32FORMAT          "%10i"
#define UINT32FORMAT         "%10u"
#define COLNAMENUMBERFORMAT  "%10s"
#define COLNAMESTRINGFORMAT  "%-10s"
#define STRING16FORMAT       "%-16s"
#define STRING32FORMAT       "%-32s"

//===========================================================
// C language

// pure C style alternative: pass size as argument everywhere
// discarded
// no new data types
// dst_max bytes preallocated in dst
void strcpy_safe(char *dst,const char *src,int32 dst_max);
//   char *null_end;    only GNU
//   null_end=(char *)memcpy(dst,src,nbytes); // only GNU

bool isidsymbol(char c);

void string_init(char *,uint16);

uint08 string_length(char *s,int64 s_size);

void set_string_length(char *s,int64 s_size,int16 s_length);

void string_copy(char *dst,const int32 dst_size,const char *src);

// robust to Windows format \n
void str_remove_spaces(char *s);

void create_aux_filename(const string32,const string32,string32 );

void str_typecheck(string08);

void test_str();


#endif // header guard
