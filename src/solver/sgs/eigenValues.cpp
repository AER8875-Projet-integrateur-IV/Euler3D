#include <iostream>
#include <math.h>
#include <string>

#include "eigenValues.h"
#include "parameters.h"
#include "setup.h"
#include "utils.h"

using namespace std;

PMmatrices eigenValues(float *lQc, float *lqf) {
	int H, u, c, b_para, a_para;
	H = (lQc[2] + lqf[2]) / lQc[0];// Enthalpy
	u = lqf[0];
	c = sqrt((g - 1) * (H - 0.5 * pow(u, 2)));
	b_para = (g - 1) / pow(c, 2);
	a_para = 0.5 * b_para * pow(c, 2);
	float **R, **R_inv, **Gam, **Gam_abs;

	// Initialize matrices
	R = new float *[3];
	R_inv = new float *[3];
	Gam = new float *[3];
	Gam_abs = new float *[3];
	for (int i = 0; i < 3; i++) {
		R[i] = new float[3];
		R_inv[i] = new float[3];
		Gam[i] = new float[3];
		Gam_abs[i] = new float[3];
	}

	for (int i = 0; i < 3; i++)
		R[0][i] = 1.0;

	R[1][0] = u - c;
	R[1][1] = u;
	R[1][2] = u + c;
	R[2][0] = H - u * c;
	R[2][1] = 0.5 * pow(u, 2);
	R[2][2] = H + u * c;

	R_inv[0][0] = 0.5 * (a_para + u / c);
	R_inv[0][1] = 0.5 * (-b_para * u - 1 / c);
	R_inv[0][2] = 0.5 * b_para;
	R_inv[1][0] = 1 - a_para;
	R_inv[1][1] = b_para * u;
	R_inv[1][2] = -b_para;
	R_inv[2][0] = 0.5 * (a_para - u / c);
	R_inv[2][1] = 0.5 * (-b_para * u + 1 / c);
	R_inv[2][2] = 0.5 * b_para;

	Gam[0][0] = u - c;
	Gam[0][1] = 0.0;
	Gam[0][2] = 0.0;
	Gam[1][0] = 0.0;
	Gam[1][1] = u;
	Gam[1][2] = 0.0;
	Gam[2][0] = 0.0;
	Gam[2][1] = 0.0;
	Gam[2][2] = u + c;

	Gam_abs[0][0] = abs(u - c);
	Gam_abs[0][1] = 0.0;
	Gam_abs[0][2] = 0.0;
	Gam_abs[1][0] = 0.0;
	Gam_abs[1][1] = abs(u);
	Gam_abs[1][2] = 0.0;
	Gam_abs[2][0] = 0.0;
	Gam_abs[2][1] = 0.0;
	Gam_abs[2][2] = abs(u + c);

	PMmatrices pmMatrices;
	pmMatrices.R = R;
	pmMatrices.R_inv = R_inv;
	pmMatrices.Gam = Gam;
	pmMatrices.Gam_abs = Gam_abs;

	return pmMatrices;
}
