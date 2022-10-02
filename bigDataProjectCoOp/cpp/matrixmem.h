
/*
============================================================================
 File:      matrixmem.h
 Project:   parallel data system  
 Author:    Carlos Ordonez
 Contents:  Dense matrix operations
 Updated:   
 1999.06.30 used Java matrix code as starting point
 2000.03.07
 2000.08.01 viewWithDimensionNames optimized
 2000.08.03 view() method only FORMAT added to:   par.h  pararem.h
 2000.08.19 bug with DimensionArray fixed, docum. updated
 2000.08.21 drand48() used instead
 2000.08.24 \n added in view() for long vectors like W
 2002.05.10 add and multiply to avoid float call to assignElement
 2002.10.25 merged EM version with KMB
 2005.05.06 changed type from 'real64' to 'float' to gain memory
 2006.06.03 bug: float was allocated, BUT model saved with real64
 2019.02.04 refactored 
  m->num_rows
  n->num_columns
  remove DebugMessage
 2019.02.05 pending: major column order, nan()
            matrix sizes names
            global trace
 2019.02.08 single constructor
 2019.04.25 
            exact size storage
            .hpp?
            alternatives to access each matrix entry
             A(1,1)
             A[addr(1,1)]
             A["1,1"]  not clean, slow, type conversion
 2019.09.13 view() changed to take TraceMessage as parameter
            view() can send output to stdout or file, any time
2019.09.17 bug with DimensionArray: dim_file_name
2019.09.18 bug: two constructors, one variable not initialized
            positional constructor eliminated
2019.09.22 preliminary usage of new string_sys => requires modifying parameter
2019.10.13 split into .h and .cpp
2019.11.20 added BLAS style functions
            get_entry() not range-checked
            pending: memcpy() to get row i
2020.02.29 DimensionArray should be eliminated: it forces reading from dim file
2022.09.17 dense
           sparse needed?
           pending: vector


 Instructions:
 -indexed from 1..num_rows,1..num_columns
 -Resut of operation is stored in the current object
  future: Euclidean, Squared?, Manahattan?
 -to display dimension names use INPUT_FILE prefix with .dat extension; 
  this class  automatically looks for this file. 
  If not found then dimension no. is shown. 
 
============================================================================

*/



#if !defined(MATRIXMEM)
#define MATRIXMEM

#include "foundation.h"
#include "str.h"
#include "trace.h"
#include "param.h"
#include "textio.h"



/* went to foundation */
//#ifndef MAXDIMENSION
// #define MAXDIMENSION 200
//#endif

//#ifndef DOUBLEFORMAT
// #define DOUBLEFORMAT "%8.3lf "
//#endif

// enum { 0=undefined,row_order,column_order };

//class Subscript { public: int32 i; }
//int32 operator, (Subscript,Subscript);

class Matrix {
public:          // external class: read-only
 Str32 name;
 int32 num_rows,num_columns;
 real64 *a;   // default: real64; a[] does not work; it was a[][]
 Str32 *column_name;  // more important
 //string16 *column_name;  // more important
 // new: pending
 // Str32 *row_name;
 // discarded
 //string16 *row_name;

 // Matrix &entry;
 //string32 name;
 // string32 file_name;
 Str32 num_rows_name,num_columns_name; // math: d, n, k
 //Str16 num_rows_name,num_columns_name; // math: d, n, k
 //deleted: string16 num_rows_name,num_columns_name; // d, n, k
 const char separator=',';

 // deprecate
 // current: bad, it requires "joining" with dim file, worse: i subscript deduced
 //char   dim_file_name[max_len_id];
 string32 dim_file_name;

 int32 storage_order;  // major row|major column
 real64 *row;   // one row for HPC-style multiply
 real64 *column;   // idem

private:        
 // BLAS style
 inline real64 get_entry(int32,int32);  // not range-checked
 real64 dot_product(real64 *,real64 *);

public:
 void allocate();
 Matrix(const char *);  // all in one string
~Matrix();
 void view(const char *choice="tm_stdout");
 void export2csv(const string32);
 void help();
 void set_column_name(Str32 in_column_name[],uint16);
 //void set_column_name(Str16 in_column_name[]);
 uint08   doubleCheck();
 uint08   valid_subscripts(int32,int32);
 real64 & operator() (int32 i,int32 j);
 void fill(real64);
 void zero();
 void identity();
 void setDiagonal(real64 value);
 int32 squared();
 real64 sum();
 void random(real64);
 real64 * get_row(int32); 
 real64 * get_column(int32); 
 void multiply(Matrix &,Matrix &); // BLAS style
 void product(Matrix &,Matrix &);
 void Gramian(Matrix &); // X X^T
 void Gamma(Matrix &); // Z Z^T, X fits in RAM
 // pending add_outerproduct_full? 
 void add_outerproduct(Matrix &); // add z_i*z_i^T, X does not fit in RAM, 
 void add_vector(Matrix &,int32); // add z_i, X does not fit in RAM, can solve L and Q?
 void add_outerproduct_diag(Matrix &,int32 j); // add z_i*z_i^T in column [j], X not fit in RAM
 //void Gamma(Dataset &); // Z Z^T, X does not fit in RAM, dependency:Dataset
 void copy(Matrix &);
 void test_portability(); // all functions
 void test_errors(); // all functions
 void test(); // all functions
// void export2csv();  // file_name given in constructor

 // kept for compatibility with old OO style
 int32   get_num_rows();
 int32   get_num_columns();
 char    *getName();
 // **deprecated, 
 // to remove later after removing dependencies
 // do not use these calls in new source code
 Matrix(int32,int32,char *);  // one by one, positional
 //void     fill_assign(real64);
 //void     assignElement(int32 i,int32 j,real64 value);
 //real64   getElement(int32 i,int32 j);
};


#endif

