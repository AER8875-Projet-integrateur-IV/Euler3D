#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>
#include <string>

using namespace std;
// Initial values (probably the ones from your teammates??)
// If so you'll have to add them en paramètre d'entrée or just change them manually

int nstep;
int nx0;
float dt;
float dx;

int lbound;
int nx;

// slope limiter

int k_muscl;
int b_muscl;


float g;

float norm_ok;

string dir_name;
string out_name_front;
string out_name_back;

#endif
