#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "parameters.h"
#include "setup.h"
#include "utils.h"
#include "eigenValues.h"
#include "inner_ite.h"
#include "cal_RHS.h"
#include "cal_for_lusgs.h"
#include "bound.h"

using namespace std;


float** inner_ite(float** lQc) {
    // Matrices matrices;
    
    float** Qcn, ** Qcm, **delta_Q, **delta_Q2, **delta_Q_temp, **qfm, **RHS;
    delta_Q = new float * [nx];
    delta_Q2 = new float * [nx];
    delta_Q_temp = new float * [nx];

    for (int i=0; i<nx; i++) {
        delta_Q[i] = new float [3];
        delta_Q2[i] = new float [3];
        delta_Q_temp[i] = new float [3];
    }

    //Note 1: This may not work: have to iterate and get lQc size
    Qcn = lQc;
    Qcm = lQc;

    // inner iteration
    for (int i=0; i<10; i++){

        // Same as Note 1
        qfm=Qctoqf(Qcm);
        
        

        Matrices matrices;
        matrices = cal_for_lusgs(Qcm,qfm);

        // Note 1
        float **lo_R= cal_RHS(Qcm);

        float* sum_b;
        sum_b = new float[3];
        for (int i=lbound; i<(nx-lbound); i++) {
            sum_b += abs(lo_R[i]);
        }

        int ite, con;
        ite=0;
        con=0;

        // lusgs loop
        while (con==0){
            for (int i=0; i<3; i++)
                delta_Q_temp[i] = delta_Q[i];
            
            float *L, *D, *U;
            L = new float [3];
            D = new float[nx];
            U = new float [3];

            float * tempPlus = rowMultiplication( matrices.matrix_plus[i],delta_Q[i], nx);
            float * tempMinus = rowMultiplication( matrices.matrix_minus[i],delta_Q[i], nx);

            for (int i=0; i<nx; i++) {
                L[i] = dt*(tempPlus[i])/2;
                D[i] = dx+dx+dt*(matrices.beta_sigma[i]);
                U[i] = dt*(tempMinus[i])/2;
            }
            
            float ** RHS ;
            RHS = new float *[nx];
            for (int i=0 ;i<nx; i++) RHS[i] = new float [3];
            
            
            // RHS
            for (int i=lbound; i<(nx-lbound); i++) {
                for (int j=0; j<3;j++) {
                    RHS[i][j] = -(Qcm[i][j]-Qcn[i][j])*dx - dt*lo_R[i][j];
                }
            }
                

    //         (D+L)Q=Q
            for (int i=lbound; i<(nx-lbound); i++) {
                for (int j=0; j<3;j++) {
                    delta_Q[i][j] = (L[i-1]+RHS[i][j]) / D[i];
                }
            }

    //         # (D+U)Q=DQ
            for (int i=lbound; i<(nx-lbound); i++) {
                for (int j=0; j<3;j++) {
                    delta_Q[i][j] = delta_Q[i][j] - U[i+1] / D[i];
                }
            }

    //         Convergence test
            if ((ite+1) % 100 ==0){
                float * sum_b_Ax, *norm2d;
                sum_b_Ax = new float[3];
                norm2d = new float[3];

                for (int i=lbound; i<(nx-lbound); i++) {
                    for (int j=0; j<3;j++) {
                        sum_b_Ax[i] += abs(delta_Q[i][j]-delta_Q_temp[i][j]);
                    }
                }
                for (int i=0; i<3;i++) norm2d[i]=sum_b_Ax[i]/sum_b[i];


                if (norm2d[0] < norm_ok && norm2d[1] < norm_ok && norm2d[2] < norm_ok) con=1;
                ite += 1;
            }

        }

    // Note: doubt we need this here

    //     delta_Q.tolist()
        for (int i=lbound; i<(nx-lbound); i++) {
            for (int j=0; j<3;j++) {
                Qcm[i][j]=Qcm[i][j]+delta_Q[i][j];
            }
        }
        Qcm=bound(Qcm);
    }
        
    return Qcm;
}





