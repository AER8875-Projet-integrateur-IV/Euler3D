#include <iostream>
#include <math.h>
#include <string>

#include "cal_Q.h"
#include "parameters.h"
#include "setup.h"


using namespace std;

float **bound(float **lQc) {
	Values values = setup();

	for (int i = 0; i < 3; i++) {
		lQc[0][i] = 2 * values.bol[i] - lQc[1][i];// Leftmost boundary
		lQc[nx - 1][i] = lQc[nx - 2][i];          // Rightmost boundary
	}
	return lQc;
}