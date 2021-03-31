#ifndef YACOBI_A_H
#define YACOBI_A_H

struct YacobiMatrices{
    float * yacobiPlus, * yacobiMinus;
};

// Calculation of Jacobian matrix
YacobiMatrices yacobi_A(float* lQc, float* lqf);

#endif
