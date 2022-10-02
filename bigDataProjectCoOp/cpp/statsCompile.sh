#!/bin/bash

g++ -std=c++17 matrixmem.h matrixmem.o datasettxt.h datasettxt.o csv.h csv.o stats_carlos.cpp foundation.h foundation.o param.h param.o str.h str.o textio.h textio.o trace.h trace.o -o ../bin/stats

