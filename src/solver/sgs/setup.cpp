#include <iostream>
#include <string>
#include <math.h>

#include "setup.h"
#include "parameters.h"


using namespace std;

// Basic volume and storage volume

Values setup() {
    float **qf, **Qc;
    // Initializing 2D dynamic arrays
    qf = new float *[nx];
    Qc = new float *[nx];
    for (int i=0; i<nx; i++) {
        qf[i] = new float [3];
        Qc[i] = new float [3];
    }

    float * u, * rho, * p, * e, * x, * bol, * bor;

    // Initializing dynamic arrays
    u = new float[nx]; // Speed
    rho = new float[nx]; // Density
    p = new float[nx]; // Pressure
    e = new float[nx]; // Energy
    x = new float[nx]; // Position
    bol = new float[nx]; // Leftmost virtual cell ?
    bor = new float[nx]; // Rightmost virtual cell ?


    // Not sure what this is for

    // """
    // e=p/(r-1)+rho*u^2/2
    // """


    for (int i=0; i< nx;i++) {
        u[i] = 0.0;

        if (i <= nx*0.5){
            rho[i] = 1.0;
            p[i] = 1.0;
        }
        else
        {
            rho[i] = 0.125;
            p[i] = 0.1;
        }

        e[i] = p[i]/(g-1)+rho[i]*(pow(u[i],2))/2;
        x[i] = i*dx-dx/2;
           
        for (int j =0; j<3; j++){
            if (j == 0){
                bol[j] = rho[0];
                bor[j] = rho[nx-1];
            }

            else if (j == 1){
                bol[j] = u[0]*rho[0];
                bor[j] = u[nx-1]*rho[nx-1];
            }

            else if (j == 2){
                bol[j] = e[0];
                bor[j] = e[nx-1];
            }

        }

        for (int i=0; i<nx; i++){
            for (int j=0; j<3; j++){
                if (j == 0){
                    qf[i][j] = u[i];
                    Qc[i][j] = rho[i];
                }
                else if (j == 1){
                    qf[i][j] = rho[i];
                    Qc[i][j] = u[i]*rho[i];
                }
                else if (j == 2){
                    qf[i][j] = p[i];
                    Qc[i][j] = e[i];
                }
            }
        }
    }
    Values values;
    values.bol = bol;    
    values.bor = bor;
    values.x = x;
    values.qf = qf;
    values.Qc = Qc;

    return values;
}
