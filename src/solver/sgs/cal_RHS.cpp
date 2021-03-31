#include <iostream>
#include <string>
#include <math.h>

#include "parameters.h"
#include "setup.h"
#include "utils.h"
#include "eigenValues.h"
#include "cal_RHS.h"
#include "fvs.h"

using namespace std;


float** cal_RHS(float ** lQc) {
    float ** RHS;
    RHS = new float * [nx];
    for (int i=0; i<nx; i++)
        RHS[i] = new float [3];

    // May need to initialize first
    float** Fplus = fvs(lQc);  

    for (int i=1; i<nx-1; i++) {
        for (int j=0; j<3; j++)
            RHS[i][j] = Fplus[i][j]-Fplus[i-1][j];
    }
    
    // Doubt we need it
    // RHS.tolist()
    return RHS;
}

