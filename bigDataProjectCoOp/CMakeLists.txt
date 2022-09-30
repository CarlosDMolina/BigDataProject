cmake_minimum_required(VERSION 3.23)
project(bigDataProjectCoOp)

set(CMAKE_CXX_STANDARD 23)

add_executable(bigDataProjectCoOp csv.h csv.o foundation.h foundation.o param.o param.h str.o str.h trace.o trace.h textio.h textio.o main.cpp)
