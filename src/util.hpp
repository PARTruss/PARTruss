#pragma once

#include <iostream>
#include <iomanip>
#define MAXMTXPRINT 20

using namespace std;

template <typename t>
void printMtx ( t* A, int n, int width ) {
  int max = ( n < MAXMTXPRINT ) ? n : MAXMTXPRINT;
  for ( int i = 0; i < max; i++ ) {
    for ( int j = 0; j < max; j++ ) {
      cout << setfill ( ' ' ) << defaultfloat << setw ( width ) << setprecision ( 1 ) << A[n * i + j];
    }
    cout << endl;
  }
}
