/*
============================================================================
 File:      textio.h
 Author:    Carlos Ordonez
 Contents:  isolated C functions
Changes:
2021.02.06 started
===========================================================================
*/




#if !defined(TEXTIO)
#define TEXTIO

#include "foundation.h"
#include "str.h"
#include "trace.h"
#include "param.h"


//------------------------------C TEXT I/O------------------------------------

/* scan until \n or EOF 
P: fptext is open
Q: str_text contains read string
   returns string length
*/
uint16 readln(FILE *fptext,char *str,uint32 str_size); 
//uint16 readln(FILE *fptext,char *str,uint16 str_size); 

/* Skips any separators like commas, spaces, tabs   */
int32
next_int32(char *str_text,uint16 *ii);

/* deprecated
uint32
next_uint32(char *str_text,uint16 *ii);
*/

/**
scan for the next double number in the string
locate first digit or sign 
then read as many characters until no more digits, decimal points
'+','-' or 'e'  are found
This function effectively skips ANY separators like spaces, commas, tabs,
  [ ] ( ) 
*/
real64
next_real64(char *str_text,uint16 *ii);

/*
Used to read next available identifier or word in the input stream
it now recognizes underscore '_'
*/
void
next_identifier(char *destination,char *str_text,uint16 *ii);

/*
Used to read next string having all characters but spaces, tabs and commas
  in the input stream
separated by spaces or commas!
valid examples:
 weight<100  1
 if_peak_heart_rate>86]then[1]   0

invalid examples:
 weight < 100     1
 if peak heart rate > 86  0
*/
void
next_description(char *destination,char *str_text,uint16 *ii);

#endif

