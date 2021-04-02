#include <iostream>
#include <math.h>
#include <string>

#include "solver/sgs/cal_for_lusgs.h"
#include "solver/sgs/eigenValues.h"
#include "solver/sgs/parameters.h"
#include "solver/sgs/utils.h"

using namespace std;


Matrices cal_for_lusgs(float **lQc, float **lqf) {
	float **matrix_plus, **matrix_minus, **I, *beta_sigma, **temp;

	matrix_plus = new float *[nx];
	matrix_minus = new float *[nx];
	I = new float *[3];
	temp = new float *[3];
	for (int i = 0; i < nx; i++) {
		matrix_plus[i] = new float[3];
		matrix_minus[i] = new float[3];
	}
	for (int i = 0; i < 3; i++) {
		I[i] = new float[3];
		temp[i] = new float[3];
	}

	beta_sigma = new float[nx];
	float beta = 1.1;
	float H, u, c;

	for (int i = 0; i < 3; i++) {
		I[i][i] = 1.0;
	}


	for (int i = 0; i < nx; i++) {
		H = (lQc[i][2] + lqf[i][2]) / lQc[i][0];
		u = lqf[i][0];
		c = sqrt((g - 1) * (H - 0.5 * pow(u, 2)));

		int sigma = abs(u) + c;
		PMmatrices pmMatrices;
		pmMatrices = eigenValues(lQc[i], lqf[i]);

		float **matrix, **matrix_sum, **matrix_sub;
		matrix = new float *[3];
		matrix_sum = new float *[nx];
		matrix_sub = new float *[nx];

		for (int i = 0; i < 3; i++)
			matrix[i] = new float[3];
		for (int i = 0; i < nx; i++) {
			matrix_sum[i] = new float[3];
			matrix_sub[i] = new float[3];
		}


		matrix = matrixMultiplication(matrixMultiplication(pmMatrices.R, pmMatrices.Gam, 3, 3, 3, 3), pmMatrices.R_inv, 3, 3, 3, 3);

		for (int i = 0; i < 3; i++) {
			temp[i][i] = I[i][i] * beta * sigma;
		}

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				matrix_sum[i][j] = 0.5 * (matrix[i][j] + temp[i][j]);
				matrix_sub[i][j] = 0.5 * (matrix[i][j] - temp[i][j]);
			}
		}

		matrix_plus[i] = matrix_sum[i];
		matrix_minus[i] = matrix_sub[i];
		beta_sigma[i] = beta * sigma;
	}
	Matrices matrices;
	matrices.matrix_plus = matrix_plus;
	matrices.matrix_minus = matrix_minus;
	matrices.beta_sigma = beta_sigma;

	return matrices;
}
