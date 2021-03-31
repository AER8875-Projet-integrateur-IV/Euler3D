#ifndef LUSGS_H
#define LUSGS_H

struct Matrices {
	float **matrix_plus, **matrix_minus, *beta_sigma;
};

// Approximate the Jacobian matrix and calculate beta for the LDU decomposition
Matrices cal_for_lusgs(float **lQc, float **lqf);


#endif
