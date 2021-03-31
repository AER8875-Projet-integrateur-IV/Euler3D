#include <iostream>
#include <math.h>
#include <string>

#include "parameters.h"
#include "setup.h"
#include "utils.h"

using namespace std;


// We could do one for only square matrices
// https://www.geeksforgeeks.org/c-program-multiply-two-matrices/
float **matrixMultiplication(float **mat1, float **mat2, int m1, int m2, int n1, int n2) {
	float **res;
	for (int i = 0; i < m1; i++) {
		for (int j = 0; j < n2; j++) {
			res[i][j] = 0;
			for (int x = 0; x < m2; x++) {
				*(*(res + i) + j) += *(*(mat1 + i) + x) * *(*(mat2 + x) + j);
			}
		}
	}
	return res;
}

float *rowMultiplication(float *mat1, float *mat2, int size) {
	float *res;
	res = new float[size];
	for (int i = 0; i < size; i++) {
		res[i] = mat1[i] * mat2[i];
	}
	return res;
}

float **invMatrix(float **mat) {
	// Sweep method
	int aa, bb;
	float **D;
	D = new float *[3];
	for (int i = 0; i < 3; i++)
		D[i] = new float[3];

	for (int l = 0; l < 3; l++) {
		aa = 1.0 / D[l][l];
		D[l][l] = 1.0;

		for (int m = 0; m < 3; m++)
			D[l][m] = D[l][m] * aa;

		for (int m = 0; m < 3; m++) {
			if (m != l) {
				bb = D[m][l];
				D[m][l] = 0.0;
				for (int n = 0; n < 3; n++)
					D[m][n] = D[m][n] - bb * D[l][n];
			}
		}
	}

	return D;
}

int minmod(int x, int y, int b) {
	int ans = sign(x) * max(0, min(abs(x), sign(x) * y * b));

	return ans;
}

int sign(int x) {
	if (x < 0) return -1;
	else if (x == 0)
		return 0;
	else
		return 1;
}

float **qftoQc(float **qf) {
	float **lo_Qc;
	lo_Qc = new float *[nx];
	for (int i = 0; i < nx; i++)
		lo_Qc[i] = new float[3];

	for (int i = 0; i < nx; i++) {
		for (int j = 0; j < 3; j++) {
			if (j == 0) lo_Qc[i][j] = qf[i][1];
			else if (j == 1)
				lo_Qc[i][j] = qf[i][1] * qf[i][0];
			else if (j == 2)
				lo_Qc[i][j] = (qf[i][2] / (g - 1) + 1.0 / 2.0 * qf[i][1] * (pow(qf[i][0], 2)));
		}
	}
	return lo_Qc;
}

float **Qctoqf(float **Qc) {
	float **lo_qf;
	lo_qf = new float *[nx];
	for (int i = 0; i < nx; i++)
		lo_qf[i] = new float[3];

	for (int i = 0; i < nx; i++) {
		for (int j = 0; j < 3; j++) {
			if (j == 0) lo_qf[i][j] = Qc[i][1] / Qc[i][0];
			else if (j == 1)
				lo_qf[i][j] = Qc[i][0];
			else if (j == 2)
				lo_qf[i][j] = (g - 1) * (Qc[i][2] - 1.0 / 2.0 * Qc[i][0] * ((Qc[i][1] / pow(Qc[i][0], 2))));
		}
	}
	return lo_qf;
}