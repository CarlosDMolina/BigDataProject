/*
============================================================================
 File:      param.h
 Project:   Data Mining
 Author:    Carlos Ordonez
 Contents:  a class to parse parameters for all modules
 Doc:
            all parameters in one string separated by ;
            all values are stoed as strings  andf they are cast after call

            syntax:
              <parameter1>=<value1>;<parameter2>=<value2>;...
              ...
 Started:   2002.11.16
 Updated:   2002.11.20 testing
            2005.03.05 more useful to have a string assigning values
                       because name of parameter is known at call time
            2005.03.06 all parameters are returned in their type or a default
            2006.06.03 checked for mem leaks: OK
2008.05.09 pending: add method to handle additional parameters, when needed
2008.05.11 search last value
            get_string->search_string
           2nd parameter avoids external if() when parameter not set;
            parameter remains the same.
2014.03.02 eliminated int2, to int32
           in Linux program should be called with \; separating parameters
           in Linux ; is a symbol that truncates the command
2019.01.09 revise to C++ 11
           read from file
           pending: sort by name->bin search
           pending: determine array size
           pending: linked list?
2019.01.12 error: delete[] needed with arraY
           file=<string> parameter
2019.01.15 pending: initialize parameter values, validate parameters
           basically defining a data type
           set containment
           integer: list of values or range
                    {0,1}
                    {1,2}
                    [0,1)
           real:    range
                    [0,1.5)
           string:  set of strings
                    {no,yes}
                    {off,on}  
                    {red,green,value}
                    file name must have extension
2019.01.16 parameters from parametersString                    
           double->real64
2019.02.08 default value for numbers == 0
           fixed int32, real64
2019.09.12 pending: set_value() for each data type
                    simplifies call, improves maintenance
           static arrays handled by the compiler
2019.10.13 split .h .cpp
2019.10.29 swig: default values here, not in .cpp
2020.12.26 changed to fixed size strings
           arrays size inside class

Example of Call:
myPar ParameterArray("inputfile=example.dat;k=5;support=0.01;confidence=0.50");

myPar.set_value();
one ParameterArray per object
<=100 parameters, avg=5, max=10 (so far)

===========================================================================
*/


#if !defined(PARAM)
#define PARAM

#include "foundation.h"
#include "str.h"
#include "trace.h"
#include "textio.h"

class Parameter {
public:            // to avoid accessors
  string32   name;   // handled by the compiler
  string32   valueString;   // number, file name, object name
  Str32      value;
  //char   name[max_len_id];   // handled by the compiler
  //char   valueString[max_len_id]; // could be shorter
  // old: unbounded
  //char   *name;
  //char   *valueString;
  //int32  maxParameterLength;
    
public:
  Parameter();
 ~Parameter();
};



// C: old
//#define MAXPARAMSTRLEN 80 // one line in text file, 
//#define MAXPARAMETERS  64 // for array

class ParameterArray {
public:
  Parameter *parameter;      // array of dimensions <= MAXDIMENSION
  static const uint16 max_parameters=100; // <=100 parameters, avg=5, max=10 (so far)
  int32      capacity; //  <= MAXPARAMETERS
  int32      size;           // actual number of used entries->count
  //string256     parametersString; // size <= capacity
  string1024    parametersString; // size <= capacity
  string16      trace;

  // old
  // char      *parametersString;
  //  char      *str_text;
  // char      trace[len8];      // on/off local debugging

public:
  ParameterArray(const char *); // with parameters, including file name
 ~ParameterArray();
  void    parseValues();  // constructor?
  char   *get_parameter_name(int32 i);
  // string32 search_string(const char *); //compile error
  string32 & search_parameter(const char *); // search parameter, return value as string

  //string32  get_value(const char *param_name);

  // string C++ style OOP
  void set_value(Str32& destination,const char *param_name,const char *default_value);

  // string C style
  // push all manipulation here, can upgrade to <string> later
  void    set_value(char * destination,int32 destination_size
                   ,const char *param_name,const char *default_value="undefined"); 
  // deprecated because sizeof() of destination string is lost
  // void    set_value(char * destination,const char *param_name  
  //                  ,const char *default_value="undefined");

  // char
  void    set_value(char & destination,const char *param_name
                   ,const char default_value='?');
  // int32
  void    set_value(int32 &destination,const char *param_name
                   ,int32 default_value);
  // real64
  void    set_value(real64 &destination,const char *param_name
                   ,real64 default_value);
  // pure signature
  //void    set_value(char *,const char *,const char *);
  //void    set_value(int32 &,const char *,int32);
  //void    set_value(real64 &,const char *,real64);
  void    view(const char *choice="tm_stdout");
  void    test();
};

#endif // PARAM

