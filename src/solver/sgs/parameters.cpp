#include "parameters.h"
#include <iostream>
#include <string>

using namespace std;
// Initial values (probably the ones from your teammates??)
// If so you'll have to add them en paramètre d'entrée or just change them manually

extern int nstep = _config.getMaxNumberIterations();//change
extern int nx0 = 100;                               //number of elements
extern float dt = 0.002;                            //input from time method
extern float dx = 0.01;                             //Volume of element  , to be an input

extern int lbound = 1;
extern int nx = nx0 + 2 * lbound;

// slope limiter

extern int k_muscl = 1. / 3.;
extern int b_muscl = (3. - k_muscl) / (1. - k_muscl);


extern float g = 1.4;

extern float norm_ok = 1.0e-6;

extern string dir_name = "sod_lusgs_0.002s_py";
extern string out_name_front = "time";
extern string out_name_back = "d-3";
