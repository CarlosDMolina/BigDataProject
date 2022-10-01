/*str.h
        str.o/*
============================================================================
File:      csv.h
Author:    Carlos Ordonez
Project:   Data Science Systems
Contents:  fast CSV
           binary mode

Main input formats:
csv
column names by default
data types optional

header format
 col_name datatype,col_name datatype,..
 datatype= int16,int32,real64,str16,str32

row format: all rows should have fixed_row_size bytes + \n
 v1 v2 .. vd \n
 v1 v2 .. vd \nA
 ..

block format
 row1 row2 .. rowm  where m==block_num_rows


Started:   2021.02.09
Updated:   
2022.09.01 compute_shape_size(): determine fixed row size for block-based I/O
2022.09.07 block-based I/O
============================================================================

*/

#if !defined(CSV)
#define CSV

// Dependencies
#include "foundation.h"
#include "str.h"
#include "trace.h"
#include "param.h"
//#include "matrixmem.h" // only new dependency
//#include "textio.h" // only new dependency

// text file I/O
class CSVfile {
public:
 /* parameters */
 bool     invalid_parameters;
 string32 file_name;
 string16 open_mode;  // read|write
 // pending
 //Str32 file_name;
 //Str16 open_mode;    // read|write
 char     header;          // y n
 char     separator;       // , | space, anything except ; default=, for CSVfile
 int32    MBs;    // megabytes for buffer <=1024  1GB
 string32 file_col_name; // file with column names, mostly for open_mode=write
 /* parameter defaults */
 const string08 open_mode_default="read";  // read|write r|w?
 static const char header_default='y';     // y n
 static const char separator_default=',';
 static const char eol_default='\n';
 static const int32 block_num_rows_default=1;  // pending: uint32?
 const string16 file_col_name_default="col_name.csv";
 const uint32 MBs_default=2;    // megabytes for buffer <=1024  1GB

 /* internal */
 FILE    *fpbin;
 // schema+raw text content
 // header
 Str32  *col_name; // any id is fine .. of any type, but matrix all == same type 
 uint08 *col_type; // no more than 10, enum{}
 Str32  *col_value; // enough to store names, large numbers, timestamp
 uint32 *separator_pos; // can fit very wide matrices; d>64k
 //string16 *col_name;  // any id is fine .. of any type, but matrix all == same type
 byte08 *rows_buffer;      // 1MB, 2MB=2*onemb kMBs=k*onemb max=4GB
 uint32 rows_buffer_size;  // all 32 bit ints
 // row size can be inferred for i+1
 uint32 *row_offset;       // row_offset[i] address of row i. row_offest[0]=header? 
 uint32 num_columns;   
 uint32 num_rows;      
 uint32 row_size;  // can vary
 // determine if regular or irregular shape
 uint32 avg_num_columns;   
 uint32 min_num_columns;   
 uint32 max_num_columns;   
 uint32 min_row_size;    // # bytes
 uint32 max_row_size;   
 // fixed shape==block-based I/O
 uint32 header_size;
 uint32 data_offset;
 uint32 fixed_row_size; 
 uint32 num_blocks; 
 int32  block_num_rows; // unspanned
 uint32 block_size; // # bytes==fixed_row_size*num_rows
 Str256 *block_row;

 CSVfile(const char *);
~CSVfile();
 // external
 void view(); 
 void test();  
 void execute();  
 // internal for now
 void   compute_shape_size();  // full scan to determine shape, size, uniform
 bool   eof();
 void   open(const char*);
 void   close();
 void   read_values(Str32 []);
 //void read_values(Str16 []);
 // header
 void set_header();
 void print_header();
 // void   set_column_name(Str32 []); // mem -> mem
 // void   read_header();   // default, from file itself
 // void   write_header();  // dump column names from array
 // row-based I/O
 //void   skip_row();  // skip comments and header on 2nd pass
 void   seek_row(uint32);
 void   read_row();
 void   view_row();
 // block-based I/O
 void allocate_block(); // header and block
 uint64 offset(uint32);
 void set_block_parameters();
 uint64 offset_block(uint32 i);
 void seek_block(uint32);
 void next_block();
 void read_block();
 void view_block();
 void sort_block(uint32);
};

#endif

