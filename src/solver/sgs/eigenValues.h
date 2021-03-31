#ifndef EIGEN_H
#define EIGEN_H

struct PMmatrices{
    float ** R, ** R_inv, ** Gam, ** Gam_abs;
};

// Calculate eigenvalues, etc. for positive and negative calculation of Jacobian matrix

PMmatrices eigenValues(float* lQc, float* lqf);


#endif
