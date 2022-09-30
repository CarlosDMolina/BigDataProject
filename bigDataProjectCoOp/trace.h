/*
============================================================================
File:      trace.h
Project:   Data Systems
Author:    Carlos Ordonez
Contents:  trace file, for debugging and viewing
Usage:     diverse classes: trace, measure time, check portability
Tested:    OS: Windows, Linux
            compiler: g++, Clang pending
Notes:     ONLY include portable code. Test in several platforms before
           adding a function here
           It requires C++; not compatible with C anymore
           try using functions here instead of implementing new ones
Changes:
2021.02.06 moved to separate file
2022.09.11 byte08
===========================================================================
*/



#if !defined(TRACE)
#define TRACE

#include "foundation.h"

// TRACE
// trace always goes to a file
// buffered. Therefore, flush() may be required
// strings must end with \n
// strings must be 80 chars max
// should not be used to print data, like records, matrices, long text

const string32  TRACE_FILE="trace.log";
//const char   TRACE_FILE[max_len_id]="trace.log";
const char   trace_ext[10]=".log";
const int    max_msg_len=200;
const int    max_idname_len=100;      // name of function, variable, class
const int    max_idname_len_print=20; // name of function, variable, class

// C++ 11
enum TraceMessageCode {TRACE_PROGRESS,TRACE_WARNING,TRACE_ERROR,TRACE_OUTPUT};

class TraceMessage {
public:    // only message is writable, rest read-only
  FILE *fptext;
  string64 file_name;
  string256 message;   /* passed as argument to TraceMessage everywhere */
  string08 trace;
  string16 message_type;
  // deprecated
  // old: parameterized length, but unsafe in calls
  //char file_name[max_idname_len];
  // char message[max_msg_len+1];    /* old: passed as argument to TraceMessage everywhere */
  //char trace[trace_len];  // on/off trace_flag
  // new: fixed lengths

public:
  TraceMessage(const char *p_file_name=TRACE_FILE);
 ~TraceMessage();
  void set_trace(char *p_trace);
  void print(const char *); 
  void printf(const char *); 
  void print();
  void print_error(const char *function_name,const char *error_message);
  void print(TraceMessageCode message_code,const char *function_name,const char *message="");
  void print_time(const char *function_name="function_name?"); //char *);
  void print_variable(const char *context_function  /* type safe */
                     ,const char *var_name
                     ,const char *var_type
                     ,const byte08 *var_value);
  void view();
  // pending: follow standard parameter constructor syntax 
  //    "file_name=trace.log;trace=off"
  //TraceMessage(const char *p_file_name);
};
// tm_file,tm_stdout((char *)"stdout");  /* observe it is declared here => global, always available, enforces serial behavior */


// pending: typeof
#define debug_int32(s,x)  sprintf(s,"%s=%i",#x,x)
//#define debug_int32(s,x)  sprintf(s,"%s=%u",#x,x)
#define debug_int64(s,x)  sprintf(s,"%s=%li",#x,x)
//#define debug_int64(s,x)  sprintf(s,"%s=%llu",#x,x)
#define debug_real64(s,x)  sprintf(s,"%s=%12.5lf",#x,x)
#define debug_string(s,x)  sprintf(s,"%s=%s",#x,x)


//------------------- string C++ classes

// TIMER 
//
// Notes:
// include the following files:
//    #include <sys/types.h>
//    #include <sys/timeb.h>
//    #include <sys/time.h>
// and link the math library: gcc prog.c -lm

/* returns elapsed time in secs in end call, exact up to msecs */
class Timer {
public:
 //struct timeb time_rec; // deprecated
 struct timespec start_time;
 struct timespec end_time;
 real64 secs;
 string64  calling_function;
 TraceMessage *tm; // pending: needs pointer

public:
  //Timer();
  Timer(TraceMessage *p_tm,const char *p_calling_function); // environment
 ~Timer();
  void start();
  double end();
};


//------------------ RAM memory used 
class MemoryMeter {
public:
  uint32   memoryUsedBytes;
  uint32   availableMemory;
  MemoryMeter();
  void inc(uint32 M);
  void dec(uint32 M);
  void computeAvailableMemory();
};

//----------------------------random numbers-------------------------------
void
initializeRandomNormalized();

double randomNormalized();

void
testRandom();

//---------------------------Portability----------------------------------

// requires TraceMessage
void
checkPortability();

void
test_portability();
//---------------------------- TEST EVERYTHING-----------------------------

void
test_Timer();


//=========================== GLOBAL VARIABLES =========================


#if !defined(TRACE_CPP)   // .h alone
//#if !defined(FOUNDATION_CPP)   // .h alone
extern TraceMessage tm_file,tm_stdout;
extern MemoryMeter  defaultMemoryMeter;
//extern Timer timer; // better as local variable
#else                          // .cpp pulls .h
TraceMessage tm_file,tm_stdout((char *)"stdout");
MemoryMeter  defaultMemoryMeter;
//Timer timer; // better as local variable
#endif


#endif
