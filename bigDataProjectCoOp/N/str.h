
/*
============================================================================
File:      str.h
Project:   Data Science Systems
Author:    Carlos Ordonez
Contents:  string classes and functions
Usage:     general purpose, C style strings, fixed size
           preferred over <string> for disk-resident strings
           fast: direct local access
           robust: no memory leaks, no new, no delete
           use it instead of char *, compatible with printf()
           stack allocation by default
           if dynamic string with size beyond 256 is needed use <string>
Tested:    OS: Windows, Linux
           compiler: g++, Clang pending
           It requires C++; not compatible with C anymore
           try using functions here instead of implementing new ones
Changes:
2021.02.06 started
2021.02.08 trace.h dependence eliminated
2021.11.07 moved s[] to Str
           all functions pushed up
===========================================================================
*/


/* C */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/timeb.h>
#include <sys/types.h>
/** C++ 11 */
#include <cmath>
#include <typeinfo>
#include <ctime>
#include <string>
using namespace std;



#if !defined(STR)
#define STR

#include "foundation.h"
//#include "trace.h"  // dependence eliminated

//======================================================
// stored in stack by default
// functional: in/out strings; parent class does not have s
struct Str {
 uint16    size;  // 
 uint16    length;  //
 char     *s;
 // actual string s[] is in each child class
 //string16  name;  // overhead?

           Str(char *p_s=0,uint16 p_s_size=0);
           //Str(char *p_s=0,uint16 p_s_size=0) { s=p_s; s[0]=0; length=0; size=p_s_size; }
 void      copy(const char *s_src);
 void      copy(Str &src);
 void      concat(const char *s_src);
 void      append(char c);
 void      operator +=(char c) { append(c); } // append one char s+='a';
 void      view();
 // deprecated
 //void      copy(char *s_dst,uint16 s_dst_size,const char *s_src);
 //void      concat(char *s_dst,const char *s_src);
 //void      append(char *s_dst,char c);
 //void      view(const char *s_src);
 void      test();
};

//-------------------------------
struct Str32: Str {
 //string32  s;  // C string
 string32  s32;  // C string
 
           Str32():Str(s32,sizeof(s32)) {}
           Str32(const char *s_src):Str(s32,sizeof(s32)) { copy(s_src); }
 //void    copy(const char *s_src) {  Str::copy(s,size,s_src); }
 //void    copy(Str32 &src ) {  Str::copy(s,size,src.s); } // can copy from smaller strings  
 //void    append(char c) { Str::append(s,c); }
 //void    operator +=(char c) { append(c); } // append one char s+='a';
 //void    view();
}; 

struct Str256: Str {
 string256  s256;  // C string
 
            Str256():Str(s256,sizeof(s256)) {}
            Str256(const char *s_src):Str(s256,sizeof(s256)) { copy(s_src); }
};

struct Str1024: Str {
 string1024  s1024;  // C string
 
            Str1024():Str(s1024,sizeof(s1024)) {}
            Str1024(const char *s_src):Str(s1024,sizeof(s1024)) { copy(s_src); }
};

//------------------------------------------
// delete later


// fixed: discarded
char *new_string08();
char *new_string16();
// capacity=256
char *new_string(uint08 p_capacity);
// cap=64k
// almost== new char[p_capacity]
char *new_String(uint16 p_capacity);
//char *new_char(uint16 p_capacity);

// strcap()
uint16 String_capacity(const char *s);
uint08 string_capacity(const char *s);

// strcpy_safe
// impossible to distinguish between String and string with arguments:
//  1st letter, S or s
void strcpy_safe(char *dst,const char *src);  // short: string08,string16
void Strcpy_safe(char *dst,const char *src);  // large: object


//----------------------------------------
// alternative #3 : not good, requires + 2 every call to access s

// pack capacity at front byte
// str_safe[0]==capacity
// str_safe[1]==length
// actual string strsafe+2;
// cons: requires reworking all string.h calls with ptr+2
//typedef char str_safe[max_len_id];
#define str_capacity(s) s[0]   // confusing
#define str_length(s) s[1]
#define str_strng(s) s+2
// strcpy(str_safe+2,"test");

//-------------------------------
// alternative #4: discarded
// String: pack capacity after null => +3 bytes to have effective max_len_id
// string: pack capacity after null => +2 bytes to have effective max_len_id
// pros: not a new class!, compatible with string.h, little extra space
// con: does not detect const strings
//typedef char *string_safe;  // not required

// reserve +1 byte for null and +2 bytes for capacity
// max=65535
uint16 String_size(uint16 &p_capacity);

// reserve +1 byte for null and +1 bytes for capacity
// max=255
uint08 string_size(uint08 &p_capacity);

// uint16: compiler automatically makes range check
void String_set_capacity(char *s,uint16 p_capacity);

// uint08: compiler automatically makes range check
void string_set_capacity(char *s,uint08 p_capacity);
//----------------------------------
/*
alternative #5
pros: well-defined in OO
cons:new data type,
     requires changing all calls to use .s and pass object instead of char *
*/
class StringLarge {
//class string_sys {
public:
  uint16 capacity; // size?
  uint16 length;
  char *s;  // s[0]==len?

  //string_sys(const char *param) {  ("capacity=100;s=XYZ;")
  // DO NOT define constructor with value string as this clashes with param
public:
  StringLarge(uint16 p_capacity=len8bits);
 ~StringLarge();
  void copy(const char *src);
  StringLarge operator+(const StringLarge &); //pending
};


//-----------------------------------


#endif // header guard
