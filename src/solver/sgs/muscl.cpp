#include <iostream>
#include <string>
#include <math.h>

#include "parameters.h"
#include "setup.h"
#include "utils.h"
#include "muscl.h"

using namespace std;

MUSCL muscl(float** lQc) {
    MUSCL musclValues;

   float** lqf=Qctoqf(lQc);
   float ** qfL, ** qfR;
   int dplus_j, dminus_j, dplus_jp, dminus_jp;
   qfL = new float *[nx+1];
   qfR = new float *[nx+1];

   for (int i=0;i<nx+1;i++) {
       qfL[i] = new float [3];
       qfR[i] = new float [3];
   }

    for (int i=0 ;i<nx-2;i++){
        for (int j=0; j<3;j++) {
            dplus_j=lqf[i+1][j]-lqf[i][j];
            dminus_j=lqf[i][j]-lqf[i-1][j];
            dplus_jp=lqf[i+2][j]-lqf[i+1][j];
            dminus_jp=lqf[i+1][j]-lqf[i][j];

            qfL[i][j]=lqf[i][j]+1/4*((1-k_muscl)*minmod(dminus_j,dplus_j,b_muscl)+(1+k_muscl)*minmod(dplus_j,dminus_j,b_muscl));
            qfR[i][j]=lqf[i+1][j]-1/4*((1-k_muscl)*minmod(dplus_jp,dminus_jp,b_muscl)+(1+k_muscl)*minmod(dminus_jp,dplus_jp,b_muscl));

        }        
    }

    // Inside boundary
    for (int j=0; j<3;j++) {
        dplus_jp=lqf[2][j]-lqf[1][j];
        dminus_jp=lqf[1][j]-lqf[0][j];
        qfR[0][j]=lqf[1][j]-1/4*((1-k_muscl)*minmod(dplus_jp,dminus_jp,b_muscl)+(1+k_muscl)*minmod(dminus_jp,dplus_jp,b_muscl));

        dplus_j=lqf[nx-1][j]-lqf[nx-2][j];
        dminus_j=lqf[nx-2][j]-lqf[nx-3][j];
        qfL[nx-2][j]=lqf[nx-2][j]+1/4*((1-k_muscl)*minmod(dminus_j,dplus_j,b_muscl)+(1+k_muscl)*minmod(dplus_j,dminus_j,b_muscl));

    }

    musclValues.QcL=qftoQc(qfL);
    musclValues.QcR=qftoQc(qfR);

    // Outside boundary
    qfL[0]=lqf[0];
    musclValues.QcL[0]=lQc[0];
    qfR[nx-2]=lqf[nx-1];
    musclValues.QcR[nx-2]=lQc[nx-1];

    musclValues.qfL = qfL;
    musclValues.qfR = qfR;
    return musclValues;
    
}