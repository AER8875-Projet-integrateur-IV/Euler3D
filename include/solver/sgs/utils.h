#ifndef UTILS_H
#define UTILS_H

#include <string>

using namespace std;

float **matrixMultiplication(float **mat1, float **mat2, int m1, int m2, int n1, int n2);
float *rowMultiplication(float *mat1, float *mat2, int size);
float **invMatrix(float **mat);

// Flow velocity
int minmod(int x, int y, int b);
int sign(int x);

// Conversions
float **qftoQc(float **qf);
float **Qctoqf(float **Qc);

// Output
void output_q(string f_name);

#endif
