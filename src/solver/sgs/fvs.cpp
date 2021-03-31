#include <iostream>
#include <math.h>
#include <string>

#include "eigenValues.h"
#include "fvs.h"
#include "muscl.h"
#include "parameters.h"
#include "setup.h"
#include "utils.h"

using namespace std;

// Flux Vector Splitting method
float **fvs(float **lQc) {
	float **Fplus, **plusMatrix, **minusMatrix;
	float **gSum, **gSub;
	Fplus = new float *[nx];
	plusMatrix = new float *[nx];
	minusMatrix = new float *[nx];
	gSum = new float *[3];
	gSub = new float *[3];
	for (int i = 0; i < nx; i++) {
		Fplus[i] = new float[3];
		plusMatrix[i] = new float[3];
		minusMatrix[i] = new float[3];
	}
	for (int i = 0; i < 3; i++) {
		gSum[i] = new float[3];
		gSub[i] = new float[3];
	}

	MUSCL musclValues = muscl(lQc);


	for (int i = 0; i < nx - 1; i++) {
		PMmatrices pMatrices = eigenValues(musclValues.QcL[i], musclValues.qfL[i]);// Calculate positive eigenvalues
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				gSum[i][j] = pMatrices.Gam[i][j] + pMatrices.Gam_abs[i][j];
			}
		}
		plusMatrix = matrixMultiplication(matrixMultiplication(pMatrices.R, gSum, 3, 3, 3, 3), pMatrices.R_inv, 3, 3, 3, 3);

		PMmatrices mMatrices = eigenValues(musclValues.QcR[i], musclValues.qfR[i]);// Calculate negative eigenvalues
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				gSub[i][j] = pMatrices.Gam[i][j] - pMatrices.Gam_abs[i][j];
			}
		}
		minusMatrix = matrixMultiplication(matrixMultiplication(mMatrices.R, gSub, 3, 3, 3, 3), mMatrices.R_inv, 3, 3, 3, 3);

		float **plusMult = matrixMultiplication(plusMatrix, musclValues.QcL, nx, 3, 3, 3);
		float **minusMult = matrixMultiplication(minusMatrix, musclValues.QcR, nx, 3, 3, 3);

		// Calculate flux
		for (int i = 0; i < nx; i++) {
			for (int j = 0; j < 3; j++) {
				Fplus[i][j] = 0.5 * (plusMult[i][j] + minusMult[i][j]);
			}
		}
	}

	return Fplus;
}
