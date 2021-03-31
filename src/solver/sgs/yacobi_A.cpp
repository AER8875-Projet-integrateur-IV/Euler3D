#include <iostream>
#include <math.h>
#include <string>

#include "eigenValues.h"
#include "parameters.h"
#include "setup.h"
#include "utils.h"
#include "yacobi_A.h"

using namespace std;

YacobiMatrices yacobi_A(float *lQc, float *lqf) {

	YacobiMatrices yacobiM;
	float *yacobiAp, *yacobiAm;
	float **gSum, **gSub;
	gSum = new float *[3];
	gSub = new float *[3];

	for (int i = 0; i < 3; i++) {
		gSum[i] = new float[3];
		gSub[i] = new float[3];
	}

	PMmatrices pmMatrices = eigenValues(lQc, lqf);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			gSum[i][j] = pmMatrices.Gam[i][j] + pmMatrices.Gam_abs[i][j];
			gSub[i][j] = pmMatrices.Gam[i][j] - pmMatrices.Gam_abs[i][j];
		}
	}

	for (int i = 0; i < 3; i++) {
		yacobiAp = rowMultiplication((rowMultiplication(pmMatrices.R[i], gSum[i], 3)), pmMatrices.R_inv[i], 3);
		yacobiAm = rowMultiplication((rowMultiplication(pmMatrices.R[i], gSub[i], 3)), pmMatrices.R_inv[i], 3);
	}
	yacobiM.yacobiMinus = yacobiAm;
	yacobiM.yacobiPlus = yacobiAp;
}