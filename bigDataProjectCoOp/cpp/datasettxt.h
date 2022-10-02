/*
============================================================================
File:      datasettxt.h
Author:    Carlos Ordonez
Project:   Analytics
Contents:  basic I/O functions
            TxtDense to read numeric files with 'd' numeric dimensions
            TxtItemset: binary files in transaction format (vector)
            it accepts numbers separated by commas, tabs or spaces
            if there is garbage it is skipped
Started:   1997
Date:      2000.03.07
Updated:   
2000.06.07 functionality to read row
2000.10.27 bug corrected, valueString[] too short
2002.10.27 problem with empty transactions
           problem with items out of range
           they are skipped
           pending: make it self-contained,
           pass size in constructor
2002.11.16 no external dependnencies but utility.h
2002.11.20 readRow() does accept empty transactions (no items)
           if an item is 0 or negative it is skipped
           skip lines with comments starting with '#'
           bug corrected: skip header, useful if exported from
           Teradata Queryman or SQL BTEQ tool
2003.04.29 uncomment line with 'return;' to get time
           per iteration without parsing
2003.10.01 parameter to write with a given separator
           this allows separating values by space or comma
2005.01.23 format to writeRow
2005.10.14 write header
2019.05.01 renamed: 
           io.h->dataset.h
2019.09.09 resume
           Matrix constructor with one string argument
           vector should be part of this class? yes
           skips header, or lines that start with letter
           TxtDense::vector maintained "online" as data set is scanned
2019.09.25 removed local sizes #define MAX_*
           now it uses global max_text_line_len
           Dataset: inheritance
            TxtSparse: done
            TxtItemset: done
           conflict with n=|V|
2019.09.28 TxtSparse next_row() done
           bug: test() should not access this->
2019.09.30 class TxtDense:Dataset
           constructor: open()
           destructor: close()
           disadvantage, interactive testing
2019.10.13 split .h .cpp
2019.10.23 swig has issues accesing Matrix *vector
2019.10.25 renamed to datasettxt.h
           two I/O branches
            txt: numbers separated by space/comma|csv
             bdf: binary data file
2020.02.09 datasetrecord defer typing completely to query evaluation
           treat all columns as string:
            pi works
            sigma works
            join works
            NULL works: can be manipulated with empty string
            GROUP BY works
           sum(A) does not work, unless most values can be converted to number
           'a'||'b' works
           date does not work
           similar: SQLLet: py+sqlite
                    q - Text as Data
                    quick import into DBMS
           nested queries->conversion to binary format on the fly
2020.02.21 TxtDense::example()
2020.02.22 getElement,assignElement functions disabled
2020.09.13 renamed to Txt*
           TxtDense matrix
           TxtSparseMatrix
           TxtTuple
           TxtItemset
           not useful yet: TxtFixstr
2020.10.01 CSV makes more sense as a standard format for all txt
2020.12.28 defaults inside class
           static const needed for int/char
           const string works
2021.01.22 renamed readln() -> skip_line() to avoid name conflict
2021.11.06 larger buffer string

Usage:
FILE fptext is opened in constructor by default and closed in destructor
To hide details of accessing each record.
Its main features include open(), eof(), numberOfRecords() 
readRow() writeRow()

Main input formats:
internal storage as array
 dense: x1 x2 .. xd | d real numbers, d fixed, dense matrices
 sparse: i j v | triples with 2 integers, one value, useful for sparse matrices
 itemset: i1 i2 .. ik | k integers (items), where k varies
 recfixstr: v1 v2 .. vd | all strings, key not required
 record: c1 c2 .. cd | d columns, datatype int32,real64,string16,string32
                       access by subscript j
                       column names can be repeated?
                       one of them may be the K (key)


 sparse: i>0 j>0 v>0 zeroes are eliminated (invalid input or id==0 or v==0) 

 Pending:
 tuple: a1 a2 .. ad | d attributes, type int32,real64,string16,string32
                      attribute names are unique
                      one of them may be the K (key)
 string separator, can it be const with default value?


 TxtItemset:
 Used to read variable size transactions, i.e. diff. from clustering
 Length of itemset stored in itemset[0]
 To hide details of accessing each record.


============================================================================

*/

#if !defined(DATASETTXT)
#define DATASETTXT

// Dependencies
#include "foundation.h"
#include "str.h"
#include "trace.h"
#include "param.h"
#include "matrixmem.h" // only new dependency
#include "textio.h" // only new dependency

// compile-time parameter
enum {SHORT_LINE, LARGE_LINE}; // Line_size;
#define LINE_SIZE LARGE_LINE


// text file I/O
class TxtDataset {
public:
 /* parameters */
 bool     invalid_parameters;
 // pending
 //Str32 file_name;
 //Str16 open_mode;  // read|write
 string32 file_name;
 string16 open_mode;  // read|write
 char header;     // y n
 char separator;  // , | space, anything except ; default=, for CSV
 string32 file_col_name; // file with column names, mostly for open_mode=write
 /* parameter defaults */
 const string08 open_mode_default="read";  // read|write
 static const char header_default='y';     // y n
 static const char separator_default=',';
 const string16 file_col_name_default="col_name.csv";

 /* internal */
 FILE *fptext;
 int32    num_bytes;
 // schema+raw text content
 Str32 *col_name;  // any id is fine .. of any type, but matrix all == same type
 //Str16 *col_name;  // any id is fine .. of any type, but matrix all == same type
 //string16 *col_name;  // any id is fine .. of any type, but matrix all == same type

 // CHOOSE I/O size 
 //string1024 valuesString;     // SHORT new==static size, safer
 string65535 valuesString;     // LARGE

/* 
// it does not work for variable declaration 
#if (LINE_SIZE==SHORT_LINE)
     string1024 valuesString;     // MEDIUM new, safer
#endif
#if (LINE_SIZE==LARGE_LINE)
       string32768 valuesString;     // LARGE new, safer
#endif
*/

 // sizes
 // int32 d;   // # of dimensions, # of columns, it was p before
 int32 num_columns;   //  creates ,many issues in subclasses
 int32 num_rows;   //  creates ,many issues in subclasses
 // irregular shape
 uint32 max_num_columns;   
 uint32 min_num_columns;   
 uint32 min_size;   
 uint32 max_size;   
 //int16 max_num_columns;   
 //int16 min_num_columns;   
 //int16 min_size;   
 //int16 max_size;   
 int32 n;  // pending: int64  n vectors, n transactions, n records, n samples..
 // subscripts
 int32 i;  //  pending: int64 point id, vector id, transaction id
 int32 j;  // dimension id, vertex #

 TxtDataset(const char *);
~TxtDataset();
 int32 eof();
 void open(const char*);
 void read_values(Str32 []);
 //void read_values(Str16 []);
 void set_column_name(Str32 []); // mem -> mem
 //void set_column_name(Str16 []); // mem -> mem
 void read_header();   // default, from file itself
 void write_header();
 void close();
 void skip_line();  // skip comments and header on 2nd pass
 void view(); 
 void compute_shape_size();  // full scan to determine shape, size, uniform
 void execute();  
};

//----------------------------------------------
// pending: TxtMatrixDense
class TxtDense: public TxtDataset {
//class CSVMatrixDense Vector: public TxtDataset {
//  FILE *fptext;
//  char file_name[max_len_id];
//  char open[max_len_id];  // read|write
//  char valuesString[max_text_line_len];     // compiler takes care of delete[]
public:
  Matrix *vector;
  // needs to be local variable
  // Matrix &x_i=*vector; // for elegant manipulation, requires initialization
  //  const Matrix &v=*vector; //illegal in C++ standard

public:
  TxtDense(const char *parameters); 
  // "file_name=x.csv;open_mode=read"  vector size computed from file, 2nd line
  // "file_name=out.csv;open_mode=write;d=3" for write() size must be given
  //TxtDense();
 ~TxtDense();
  int32 determine_vector_size();
  void  readRow();
  void  writeRow(char);
  // void  writeDimensionNames(int32);   // pending: delete header, pass d as parameter
 //  void  write_header();
  void  example();  // 
  void  test_Gamma();  // 
  void  test();  // main()-style
};


//---------------------------------------------------
// pending: TxtMatrixSparse
class TxtSparse:public TxtDataset {
//PENDING: class CSVMatrixSparse:public TxtDataset {
// inherited
// FILE *fptext;
// char file_name[max_len_id];
// char open[max_len_id];  // mode to open file: read|write
// char *valuesString;
// int32 n;   // conflict with E, n vertices

public:
 enum { UNDEFINED,MACHINE_LEARNING,GRAPH }; // ML: rectangular d!=n; graph: squared n*n
 int32 matrix_type;
 // sizes
 int32 num_rows;
 int32 num_columns;
 //int32 d;   // full scan required, 
 // I/O atom i,j,v
 // i,j come from TxtDataset
// int32 i;
 // sparse
 real64 v;     // needs to be here, to maintain state from previous i
 int32 i_prev; // to detect point boundaries, assumes rows are clustered by i
 int32 k;   // k entries>0, changes every i
 int32 i_max; // in a graph n=max(i_max,j_max)
 int32 j_max; // in a sparse matrix j_max becomes d in a dense matrix
 int32 k_max;   // guaranteed max size
 int32 *i_list;  // i subscript, redundant but fast access and easy to remember
 int32 *j_list;  // subscripts
 real64 *v_list; // values>0
 // dense
 //Matrix *vector; // after reading all values for same i, requirres knowing d

 TxtSparse(const char *params);
~TxtSparse();
 void view_ijv(int32 i,int32 j,real64 v);
 void view();
 void determine_sizes();   // k,d and n
 // void sort(const char *); // by i,j|j,i priority=in BDF
 void next_row();     // read row i completely
 void write_header();
 void write_row();
 int32 append_triple(int32 i,int32 j,real64 v);
 void copy_lists(int32 p_i_list[],int32 p_j_list[],real64 p_v_list[]);
 void test_copy();
 void test();
 void test_next_row();
};

//----------------------------------------------------
class TxtItemset:TxtDataset {

public:
 // I/O atom: one transaction, variable size
 int32 *t;       // transaction: array of integers t[0] has size
 int32 t_size; // maximum number of items, t_size<= maxItem
 int32 maxItem;  // dimensionality

public:
  TxtItemset(const char *);
  //TxtItemset(int32,int32);
 ~TxtItemset();
  // pending: determine maxItem in one scan
 // void openRead();
 // void openWrite();
  void view();
  void  readRow();  // requires size of array as parameter
  void  writeRow();
  void  copy_itemset(int32 *);
  int32 getn() { return n; }
  void  test();
};

//----------------------------------------------------
/* 
+:
   O(1) access
   easy programming
-:
   wasted space
*/
class TxtFixstr:TxtDataset {
public:
  // input
  // internal storage
  // int32 p;
  // string16 *col_name;  // moved to parent TxtDataSet

  int08    *col_datatype;
  Str32    *col_value;  // value[1] value[2] .. of any type
  //Str16    *col_value;  // value[1] value[2] .. of any type
  //string16 *col_value;  // value[1] value[2] .. of any type

  TxtFixstr(const char *);
 ~TxtFixstr();
  void view_col_array(const char *,Str32 []);
  //void view_col_array(const char *,Str16 []);
  // void view_col_array(const char *,string16 []);
  void view();
  void view_parameters();
  int32 determine_size();   // p and n
  //void read_header();    // here for now, hard to move up to parent
  void read_col_names();
  void read_col_values();
  void read_next();  
  void test();
};

//----------------------------------------------------
/*
+:
-:
*/
//enum  record_datatype { INT, REAL, CHAR, DATE, NUMERIC,TIMESTAMP};
enum datatype_code { datatype_undefined, datatype_int, datatype_real
                      , datatype_char,datatype_data, datatype_numeric,datatype_timestamp};
// record? repeated, subscript ok
// tuple? PK, unique, subscript, strictly relational
class TxtRecord:TxtDataset {
public:
  int08    *col_datatype_code; // to save space, enforce via enum
  string16 *col_datatype;  //  int, real, char(n), date
  string16 *col_value;  // value may clash, col_value[1] value[2] .. of any type

};

//-------------------------------------------------------
class TxtConverter {
public: 
  enum { NONE,DENSE2SPARSE,SPARSE2DENSE} choice_set;
  // parameters
  bool      invalid_parameters;
  int32     choice;
  string32  in_file_name;
  string32  out_file_name;
  //const string128  parameters="choice,in_file_name,out_file_name";
  //int08     in_file_format;
  //int08     out_file_format;

  TxtConverter(const char *params="");
 ~TxtConverter();
  void help();
  void view();
  void dense2sparse();
  void sparse2dense();
  void execute();

};


#endif
